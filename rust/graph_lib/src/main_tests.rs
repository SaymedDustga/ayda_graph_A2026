/// @file student_main.rs
/// @brief Safe test runner for students. Catches "unimplemented" and displays pending status.

mod graph;
mod graph_algorithms;
mod graph_builders;

use graph::{DirectedGraph, Graph, UndirectedGraph};
use graph_algorithms::{Dijkstra, GraphProperties, GraphTopological, GraphTraversals};

use std::panic;

type TestGraph = DirectedGraph<String, f64>;
type UndirectedTestGraph = UndirectedGraph<String, f64>;

// Helper function to safely run tests and catch "unimplemented!()" panics
fn run_test<F>(test_name: &str, test_func: F)
where
    F: FnOnce() + std::panic::UnwindSafe,
{
    print!("Running {:<30} ", test_name);

    let result = panic::catch_unwind(test_func);

    match result {
        Ok(_) => println!("\x1b[32m[PASSED]\x1b[0m"), // Green
        Err(err) => {
            let msg_opt = if let Some(msg) = err.downcast_ref::<&str>() {
                Some(msg.to_string())
            } else if let Some(msg) = err.downcast_ref::<String>() {
                Some(msg.clone())
            } else {
                None
            };

            if let Some(msg) = msg_opt {
                if msg.contains("not implemented yet") {
                    println!("\x1b[33m[PENDING]\x1b[0m ({})", msg.split(" is ").next().unwrap_or("Function"));
                } else {
                    println!("\x1b[31m[FAILED]\x1b[0m ({})", msg);
                }
            } else {
                println!("\x1b[31m[FAILED]\x1b[0m (Assertion Error or Unknown Panic)");
            }
        }
    }
}

fn test_traverse_bfs() {
    let mut g = UndirectedTestGraph::new();
    let n1 = g.add_node_by_value("1".to_string());
    let n2 = g.add_node_by_value("2".to_string());
    let n3 = g.add_node_by_value("3".to_string());
    let n4 = g.add_node_by_value("4".to_string());
    g.add_arc(n1.clone(), n2.clone(), 0.0);
    g.add_arc(n1.clone(), n3.clone(), 0.0);
    g.add_arc(n2.clone(), n4.clone(), 0.0);

    let mut visited_nodes = Vec::new();
    GraphTraversals::traverse_bfs(&g, n1.clone(), |n| visited_nodes.push(n));

    assert_eq!(visited_nodes.len(), 4, "BFS should visit all 4 nodes");
    assert_eq!(visited_nodes[0], n1, "BFS should start at root");
}

fn test_traverse_dfs() {
    let mut g = UndirectedTestGraph::new();
    let n1 = g.add_node_by_value("1".to_string());
    let n2 = g.add_node_by_value("2".to_string());
    let n3 = g.add_node_by_value("3".to_string());
    let n4 = g.add_node_by_value("4".to_string());
    g.add_arc(n1.clone(), n2.clone(), 0.0);
    g.add_arc(n2.clone(), n3.clone(), 0.0);
    g.add_arc(n3.clone(), n4.clone(), 0.0);

    let mut visited_nodes = Vec::new();
    GraphTraversals::traverse_dfs(&g, n1.clone(), |n| visited_nodes.push(n));

    assert_eq!(visited_nodes.len(), 4, "DFS should visit all 4 nodes");
    assert_eq!(visited_nodes[0], n1, "DFS should start at root");
}

fn test_shortest_path_bfs() {
    let mut g = UndirectedTestGraph::new();
    let n1 = g.add_node_by_value("1".to_string());
    let n2 = g.add_node_by_value("2".to_string());
    let n3 = g.add_node_by_value("3".to_string());
    let n4 = g.add_node_by_value("4".to_string());
    g.add_arc(n1.clone(), n2.clone(), 0.0);
    g.add_arc(n1.clone(), n3.clone(), 0.0);
    g.add_arc(n2.clone(), n4.clone(), 0.0);
    g.add_arc(n3.clone(), n4.clone(), 0.0);

    let path = GraphTraversals::find_shortest_path_bfs(&g, n1.clone(), n4.clone());
    assert_eq!(path.len(), 3, "Shortest path should have 3 nodes");
}

fn test_spanning_trees() {
    let mut g = UndirectedTestGraph::new();
    let n1 = g.add_node_by_value("1".to_string());
    let n2 = g.add_node_by_value("2".to_string());
    let n3 = g.add_node_by_value("3".to_string());
    let n4 = g.add_node_by_value("4".to_string());
    g.add_arc(n1.clone(), n2.clone(), 0.0);
    g.add_arc(n1.clone(), n3.clone(), 0.0);
    g.add_arc(n2.clone(), n4.clone(), 0.0);
    g.add_arc(n3.clone(), n4.clone(), 0.0);

    let tree_dfs = GraphTraversals::build_spanning_tree_dfs(&g, n1.clone());
    assert_eq!(tree_dfs.get_nodes().len(), 4);
    assert_eq!(tree_dfs.get_arcs().len(), 3);
}

fn test_topological_sort() {
    let mut g = TestGraph::new();
    let n1 = g.add_node_by_value("CS101".to_string());
    let n2 = g.add_node_by_value("CS102".to_string());
    let n3 = g.add_node_by_value("CS201".to_string());
    let n4 = g.add_node_by_value("CS301".to_string());

    g.add_arc(n1.clone(), n2.clone(), 0.0);
    g.add_arc(n2.clone(), n3.clone(), 0.0);
    g.add_arc(n3.clone(), n4.clone(), 0.0);

    let topo_order = GraphTopological::get_sort(&g);
    assert_eq!(topo_order.len(), 4);
    assert_eq!(topo_order[0], n1);
    assert_eq!(topo_order[3], n4);
}

fn test_topological_ranks() {
    let mut g = TestGraph::new();
    let n1 = g.add_node_by_value("CS101".to_string());
    let n2 = g.add_node_by_value("CS102".to_string());
    let n3 = g.add_node_by_value("CS201".to_string());
    let n4 = g.add_node_by_value("CS301".to_string());

    g.add_arc(n1.clone(), n2.clone(), 0.0);
    g.add_arc(n2.clone(), n3.clone(), 0.0);
    g.add_arc(n3.clone(), n4.clone(), 0.0);

    let ranks = GraphTopological::get_ranks(&g);
    assert_eq!(ranks[&n1], 0);
    assert_eq!(ranks[&n4], 3);
}

fn test_dijkstra() {
    let mut g = TestGraph::new();
    let n_a = g.add_node_by_value("A".to_string());
    let n_b = g.add_node_by_value("B".to_string());
    let n_c = g.add_node_by_value("C".to_string());
    let n_d = g.add_node_by_value("D".to_string());
    let n_e = g.add_node_by_value("E".to_string());

    g.add_arc(n_a.clone(), n_b.clone(), 10.0);
    g.add_arc(n_a.clone(), n_c.clone(), 3.0);
    g.add_arc(n_b.clone(), n_d.clone(), 2.0);
    g.add_arc(n_c.clone(), n_b.clone(), 1.0);
    g.add_arc(n_c.clone(), n_d.clone(), 8.0);
    g.add_arc(n_c.clone(), n_e.clone(), 2.0);
    g.add_arc(n_e.clone(), n_d.clone(), 9.0);

    let spt = Dijkstra::get_minimum_paths_tree(&g, n_a.clone());
    assert_eq!(spt.get_nodes().len(), 5);
    assert_eq!(spt.get_arcs().len(), 4);
}

fn test_kosaraju_scc() {
    let mut g = TestGraph::new();
    let n_a = g.add_node_by_value("A".to_string());
    let n_b = g.add_node_by_value("B".to_string());
    let n_c = g.add_node_by_value("C".to_string());
    let n_d = g.add_node_by_value("D".to_string());
    let n_e = g.add_node_by_value("E".to_string());

    g.add_arc(n_a.clone(), n_b.clone(), 1.0);
    g.add_arc(n_b.clone(), n_c.clone(), 1.0);
    g.add_arc(n_c.clone(), n_a.clone(), 1.0);
    g.add_arc(n_c.clone(), n_d.clone(), 1.0); // Bridge
    g.add_arc(n_d.clone(), n_e.clone(), 1.0);
    g.add_arc(n_e.clone(), n_d.clone(), 1.0);

    let scc = GraphProperties::compute_strongly_connected_components(&g);
    assert_eq!(scc.len(), 2, "Graph should have exactly 2 SCCs");
}

fn main() {
    println!("==========================================");
    println!("   GRAPH LIBRARY - STUDENT AUTOGRADER     ");
    println!("==========================================\n");

    run_test("Traverse BFS", test_traverse_bfs);
    run_test("Traverse DFS", test_traverse_dfs);
    run_test("Shortest Path (BFS)", test_shortest_path_bfs);
    run_test("Spanning Tree (DFS)", test_spanning_trees);

    println!("------------------------------------------");

    run_test("Topological Sort", test_topological_sort);
    run_test("Topological Ranks", test_topological_ranks);

    println!("------------------------------------------");

    run_test("Dijkstra's Algorithm", test_dijkstra);
    run_test("Kosaraju's SCC", test_kosaraju_scc);

    println!("\n==========================================");
}