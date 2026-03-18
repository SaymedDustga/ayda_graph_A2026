/// @file examples.rs
/// @brief Detailed tutorial on how to use the Graph Library in Rust.
///
/// Run this file to see how to instantiate graphs, add nodes,
/// use dynamic weights, save/load from files, and use the Builders.

mod graph;
mod graph_builders;

use graph::{DirectedGraph, Graph, UndirectedGraph};
use graph_builders::GraphBuilder;
use std::rc::Rc;
use std::cell::RefCell;

fn tutorial_basic_graph() {
    println!("--- 1. BASIC GRAPH CREATION ---");
    // Create a Directed Graph with String nodes and f64 weights
    let mut g: DirectedGraph<String, f64> = DirectedGraph::new();

    // Add nodes
    let n1 = g.add_node_by_value("Caracas".to_string());
    let n2 = g.add_node_by_value("Merida".to_string());
    let n3 = g.add_node_by_value("Valencia".to_string());

    // Add arcs with fixed weights (Distance in km)
    g.add_arc(n1.clone(), n2.clone(), 680.5);
    g.add_arc(n1.clone(), n3.clone(), 170.0);

    g.print_graph();
    println!();
}

fn tutorial_dynamic_weights() {
    println!("--- 2. DYNAMIC WEIGHTS (LAMBDAS) ---");
    let mut g: DirectedGraph<String, f64> = DirectedGraph::new();
    let n_a = g.add_node_by_value("A".to_string());
    let n_b = g.add_node_by_value("B".to_string());

    // In Rust, mutable captured state inside a closure requires Rc and RefCell
    let traffic_jam_multiplier = Rc::new(RefCell::new(1.0));
    let base_time = 30.0; // 30 minutes

    // Clone the Rc pointer to move into the closure
    let multiplier_clone = traffic_jam_multiplier.clone();

    let arc = g.add_arc_func(n_a, n_b, Box::new(move || {
        base_time * (*multiplier_clone.borrow())
    }));

    println!("Time with no traffic: {} mins", arc.get_weight().unwrap());

    // Modify the external state
    *traffic_jam_multiplier.borrow_mut() = 2.5; // Rush hour!

    println!("Time during rush hour: {} mins\n", arc.get_weight().unwrap());
}

fn tutorial_builders_and_export() {
    println!("--- 3. GRAPH BUILDERS & EXPORTING ---");

    // We want to build a 2x2 grid of 0.0 weighted undirected nodes
    let name_generator = |r, c| format!("N_{}_{}", r, c);

    let grid: UndirectedGraph<String, f64> = GraphBuilder::build_rectangular_grid(2, 2, name_generator, 0.0);

    println!("Built a grid with {} nodes.", grid.get_nodes().len());

    // Export to DOT format to visualize in Graphviz
    grid.to_dot_file("my_grid.gv").expect("Failed to write to file");
    println!("Exported graph to 'my_grid.gv'. You can render it using Graphviz.\n");
}

fn tutorial_save_and_load() {
    println!("--- 4. PERSISTENCE (SAVE / LOAD) ---");
    let mut g: DirectedGraph<String, f64> = DirectedGraph::new();
    let n1 = g.add_node_by_value("Start".to_string());
    let n2 = g.add_node_by_value("End".to_string());
    g.add_arc(n1, n2, 99.9);

    let filename = "tutorial_graph.txt";
    g.save_to_file(filename).expect("Failed to save graph");
    println!("Graph saved to disk.");

    // Load it back
    let loaded_g: DirectedGraph<String, f64> = DirectedGraph::load_from_file(filename).expect("Failed to load graph");
    println!("Graph loaded from disk! It has {} arcs.\n", loaded_g.get_arcs().len());
}

fn main() {
    println!("Welcome to the Rust Graph Library Tutorial!\n");
    tutorial_basic_graph();
    tutorial_dynamic_weights();
    tutorial_builders_and_export();
    tutorial_save_and_load();
}