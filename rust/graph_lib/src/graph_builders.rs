/// @file graph_builders.rs
/// @brief Factory module to generate standard graph topologies.

use crate::graph::{Graph, Node};
use std::rc::Rc;
use rand::Rng;

pub struct GraphBuilder;

impl GraphBuilder {
    pub fn build_complete_graph<G, N, W, F>(n: usize, mut node_name_generator: F, default_weight: W) -> G
    where G: Graph<N, W> + Default, F: FnMut(usize) -> N, W: Clone + 'static,
    {
        let mut g = G::default();
        let mut nodes = Vec::new();
        for i in 0..n { nodes.push(g.add_node_by_value(node_name_generator(i))); }

        for i in 0..n {
            for j in (i + 1)..n {
                g.add_arc(nodes[i].clone(), nodes[j].clone(), default_weight.clone());
            }
        }
        g
    }

    pub fn build_path_graph<G, N, W, F>(n: usize, mut node_name_generator: F, default_weight: W) -> G
    where G: Graph<N, W> + Default, F: FnMut(usize) -> N, W: Clone + 'static,
    {
        let mut g = G::default();
        let mut nodes = Vec::new();
        for i in 0..n { nodes.push(g.add_node_by_value(node_name_generator(i))); }

        for i in 0..(n - 1) { g.add_arc(nodes[i].clone(), nodes[i + 1].clone(), default_weight.clone()); }
        g
    }

    pub fn build_cycle_graph<G, N, W, F>(n: usize, mut node_name_generator: F, default_weight: W) -> G
    where G: Graph<N, W> + Default, F: FnMut(usize) -> N, W: Clone + 'static,
    {
        let mut g = G::default();
        let mut nodes = Vec::new();
        for i in 0..n { nodes.push(g.add_node_by_value(node_name_generator(i))); }

        for i in 0..n { g.add_arc(nodes[i].clone(), nodes[(i + 1) % n].clone(), default_weight.clone()); }
        g
    }

    pub fn build_random_graph<G, N, W, F>(n: usize, p: f64, mut node_name_generator: F, default_weight: W) -> G
    where G: Graph<N, W> + Default, F: FnMut(usize) -> N, W: Clone + 'static,
    {
        let mut g = G::default();
        let mut nodes = Vec::new();
        let mut rng = rand::thread_rng();
        for i in 0..n { nodes.push(g.add_node_by_value(node_name_generator(i))); }

        for i in 0..n {
            for j in (i + 1)..n {
                if rng.gen_bool(p) {
                    g.add_arc(nodes[i].clone(), nodes[j].clone(), default_weight.clone());
                }
            }
        }
        g
    }

    pub fn build_rectangular_grid<G, N, W, F>(rows: usize, cols: usize, mut node_name_generator: F, default_weight: W) -> G
    where G: Graph<N, W> + Default, F: FnMut(usize, usize) -> N, W: Clone + 'static,
    {
        let mut g = G::default();
        let mut grid: Vec<Vec<Option<Rc<Node<N>>>>> = vec![vec![None; cols]; rows];

        for i in 0..rows {
            for j in 0..cols { grid[i][j] = Some(g.add_node_by_value(node_name_generator(i, j))); }
        }

        for i in 0..rows {
            for j in 0..cols {
                if j + 1 < cols { g.add_arc(grid[i][j].clone().unwrap(), grid[i][j + 1].clone().unwrap(), default_weight.clone()); }
                if i + 1 < rows { g.add_arc(grid[i][j].clone().unwrap(), grid[i + 1][j].clone().unwrap(), default_weight.clone()); }
            }
        }
        g
    }
}