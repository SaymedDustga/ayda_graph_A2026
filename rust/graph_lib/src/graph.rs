/// @file graph.rs
/// @brief Core graph data structures and traits.

use std::cell::RefCell;
use std::collections::{HashMap, HashSet};
use std::fmt::Display;
use std::fs::File;
use std::hash::{Hash, Hasher};
use std::io::{Read, Write};
use std::rc::Rc;
use std::str::FromStr;
use std::sync::atomic::{AtomicUsize, Ordering};

static ID_COUNTER: AtomicUsize = AtomicUsize::new(0);

fn next_id() -> usize {
    ID_COUNTER.fetch_add(1, Ordering::SeqCst)
}

/// Represents a Node (Vertex) in the graph.
#[derive(Debug)]
pub struct Node<N> {
    pub id: usize,
    pub value: N,
}

impl<N> Node<N> {
    pub fn new(value: N) -> Self {
        Self { id: next_id(), value }
    }
}

impl<N> Hash for Node<N> {
    fn hash<H: Hasher>(&self, state: &mut H) { self.id.hash(state); }
}
impl<N> PartialEq for Node<N> {
    fn eq(&self, other: &Self) -> bool { self.id == other.id }
}
impl<N> Eq for Node<N> {}

/// Represents a directed or undirected Arc (Edge) between two nodes.
pub struct Arc<N, W> {
    pub id: usize,
    pub source: Rc<Node<N>>,
    pub target: Rc<Node<N>>,
    weight_function: RefCell<Option<Box<dyn Fn() -> W>>>,
}

impl<N, W> Arc<N, W> {
    pub fn new(source: Rc<Node<N>>, target: Rc<Node<N>>, weight_func: Option<Box<dyn Fn() -> W>>) -> Self {
        Self { id: next_id(), source, target, weight_function: RefCell::new(weight_func) }
    }

    pub fn new_fixed(source: Rc<Node<N>>, target: Rc<Node<N>>, weight: W) -> Self
    where W: Clone + 'static, {
        Self { id: next_id(), source, target, weight_function: RefCell::new(Some(Box::new(move || weight.clone()))) }
    }

    pub fn new_unweighted(source: Rc<Node<N>>, target: Rc<Node<N>>) -> Self {
        Self { id: next_id(), source, target, weight_function: RefCell::new(None) }
    }

    /// Evaluates and returns the current weight of the arc.
    pub fn get_weight(&self) -> Option<W> {
        let func = self.weight_function.borrow();
        func.as_ref().map(|f| f())
    }

    pub fn print(&self, is_digraph: bool) where N: Display, W: Display {
        let connection = if is_digraph { "From: " } else { "Union With Nodes: " };
        let direction = if is_digraph { " To: " } else { " And: " };

        print!("{}{}{}{}", connection, self.source.value, direction, self.target.value);
        if let Some(w) = self.get_weight() { println!(" Weight: {}", w); } else { println!(); }
    }
}

impl<N, W> Hash for Arc<N, W> {
    fn hash<H: Hasher>(&self, state: &mut H) { self.id.hash(state); }
}
impl<N, W> PartialEq for Arc<N, W> {
    fn eq(&self, other: &Self) -> bool { self.id == other.id }
}
impl<N, W> Eq for Arc<N, W> {}

/// Abstract Base Trait for Graphs.
pub trait Graph<N, W> {
    fn add_node_by_value(&mut self, value: N) -> Rc<Node<N>>;
    fn add_node(&mut self, node: Rc<Node<N>>);

    fn add_arc(&mut self, source: Rc<Node<N>>, target: Rc<Node<N>>, weight: W) -> Rc<Arc<N, W>> where W: Clone + 'static;
    fn add_unweighted_arc(&mut self, source: Rc<Node<N>>, target: Rc<Node<N>>) -> Rc<Arc<N, W>>;
    fn add_arc_func(&mut self, source: Rc<Node<N>>, target: Rc<Node<N>>, weight_func: Box<dyn Fn() -> W>) -> Rc<Arc<N, W>>;

    fn print_graph(&self) where N: Display, W: Display;
    fn to_dot_file(&self, filename: &str) -> std::io::Result<()> where N: Display, W: Display;

    fn get_nodes(&self) -> &HashSet<Rc<Node<N>>>;
    fn get_arcs(&self) -> &HashSet<Rc<Arc<N, W>>>;

    fn get_outgoing_arcs(&self, node: &Rc<Node<N>>) -> Vec<Rc<Arc<N, W>>>;
    fn get_incoming_arcs(&self, node: &Rc<Node<N>>) -> Vec<Rc<Arc<N, W>>>;

    fn get_out_degree(&self, node: &Rc<Node<N>>) -> usize { self.get_outgoing_arcs(node).len() }
    fn get_in_degree(&self, node: &Rc<Node<N>>) -> usize { self.get_incoming_arcs(node).len() }

    fn contains(&self, node: &Rc<Node<N>>) -> bool {
        self.get_nodes().contains(node)
    }

    /// Persists the graph's current state into a text file.
    fn save_to_file(&self, filename: &str) -> std::io::Result<()> where N: Display, W: Display {
        let mut file = File::create(filename)?;
        writeln!(file, "{}", self.get_nodes().len())?;

        let mut node_to_index = HashMap::new();
        let ordered_nodes: Vec<_> = self.get_nodes().iter().cloned().collect();

        for (index, node) in ordered_nodes.iter().enumerate() {
            node_to_index.insert(node.clone(), index);
            writeln!(file, "{}", node.value)?;
        }

        writeln!(file, "{}", self.get_arcs().len())?;

        for arc in self.get_arcs() {
            let src_idx = node_to_index[&arc.source];
            let tgt_idx = node_to_index[&arc.target];
            if let Some(weight) = arc.get_weight() {
                writeln!(file, "{} {} {}", src_idx, tgt_idx, weight)?;
            } else {
                writeln!(file, "{} {}", src_idx, tgt_idx)?;
            }
        }
        Ok(())
    }
}

/// Implementation of a Directed Graph.
pub struct DirectedGraph<N, W> {
    pub nodes: HashSet<Rc<Node<N>>>,
    pub arcs: HashSet<Rc<Arc<N, W>>>,
    out_adj_list: HashMap<Rc<Node<N>>, Vec<Rc<Arc<N, W>>>>,
    in_adj_list: HashMap<Rc<Node<N>>, Vec<Rc<Arc<N, W>>>>,
}

impl<N, W> DirectedGraph<N, W> {
    pub fn new() -> Self {
        Self { nodes: HashSet::new(), arcs: HashSet::new(), out_adj_list: HashMap::new(), in_adj_list: HashMap::new() }
    }

    pub fn load_from_file(filename: &str) -> std::io::Result<Self> where N: FromStr, W: FromStr + Clone + 'static {
        let mut file = File::open(filename)?;
        let mut content = String::new();
        file.read_to_string(&mut content)?;
        let tokens: Vec<String> = content.split_whitespace().map(|s| s.to_string()).collect();

        let mut g = Self::new();
        if tokens.is_empty() { return Ok(g); }

        let mut token_idx = 0;
        let num_nodes: usize = tokens[token_idx].parse().unwrap_or(0); token_idx += 1;

        let mut index_to_node = Vec::new();
        for _ in 0..num_nodes {
            if let Ok(val) = tokens[token_idx].parse::<N>() { index_to_node.push(g.add_node_by_value(val)); }
            token_idx += 1;
        }

        if token_idx >= tokens.len() { return Ok(g); }
        let num_arcs: usize = tokens[token_idx].parse().unwrap_or(0); token_idx += 1;
        let tokens_per_arc = if num_arcs > 0 { (tokens.len() - token_idx) / num_arcs } else { 0 };

        for _ in 0..num_arcs {
            let src_idx: usize = tokens[token_idx].parse().unwrap();
            let tgt_idx: usize = tokens[token_idx + 1].parse().unwrap();
            if tokens_per_arc >= 3 {
                if let Ok(weight) = tokens[token_idx + 2].parse::<W>() { g.add_arc(index_to_node[src_idx].clone(), index_to_node[tgt_idx].clone(), weight); }
            } else {
                g.add_unweighted_arc(index_to_node[src_idx].clone(), index_to_node[tgt_idx].clone());
            }
            token_idx += tokens_per_arc;
        }
        Ok(g)
    }
}

impl<N, W> Default for DirectedGraph<N, W> {
    fn default() -> Self {
        Self::new()
    }
}

impl<N, W> Graph<N, W> for DirectedGraph<N, W> {
    fn add_node_by_value(&mut self, value: N) -> Rc<Node<N>> {
        let node = Rc::new(Node::new(value));
        self.nodes.insert(node.clone());
        self.out_adj_list.insert(node.clone(), Vec::new());
        self.in_adj_list.insert(node.clone(), Vec::new());
        node
    }

    fn add_node(&mut self, node: Rc<Node<N>>) {
        self.nodes.insert(node.clone());
        self.out_adj_list.entry(node.clone()).or_insert_with(Vec::new);
        self.in_adj_list.entry(node.clone()).or_insert_with(Vec::new);
    }

    fn add_arc(&mut self, source: Rc<Node<N>>, target: Rc<Node<N>>, weight: W) -> Rc<Arc<N, W>> where W: Clone + 'static {
        let arc = Rc::new(Arc::new_fixed(source.clone(), target.clone(), weight));
        self.arcs.insert(arc.clone());
        self.out_adj_list.get_mut(&source).unwrap().push(arc.clone());
        self.in_adj_list.get_mut(&target).unwrap().push(arc.clone());
        arc
    }

    fn add_unweighted_arc(&mut self, source: Rc<Node<N>>, target: Rc<Node<N>>) -> Rc<Arc<N, W>> {
        let arc = Rc::new(Arc::new_unweighted(source.clone(), target.clone()));
        self.arcs.insert(arc.clone());
        self.out_adj_list.get_mut(&source).unwrap().push(arc.clone());
        self.in_adj_list.get_mut(&target).unwrap().push(arc.clone());
        arc
    }

    fn add_arc_func(&mut self, source: Rc<Node<N>>, target: Rc<Node<N>>, weight_func: Box<dyn Fn() -> W>) -> Rc<Arc<N, W>> {
        let arc = Rc::new(Arc::new(source.clone(), target.clone(), Some(weight_func)));
        self.arcs.insert(arc.clone());
        self.out_adj_list.get_mut(&source).unwrap().push(arc.clone());
        self.in_adj_list.get_mut(&target).unwrap().push(arc.clone());
        arc
    }

    fn print_graph(&self) where N: Display, W: Display { for arc in &self.arcs { arc.print(true); } }
    fn to_dot_file(&self, filename: &str) -> std::io::Result<()> where N: Display, W: Display {
        let mut file = File::create(filename)?;
        writeln!(file, "digraph G {{")?;
        for node in &self.nodes { writeln!(file, "  \"{}\";", node.value)?; }
        for arc in &self.arcs {
            if let Some(w) = arc.get_weight() { writeln!(file, "  \"{}\" -> \"{}\" [label=\"{}\"];", arc.source.value, arc.target.value, w)?; }
            else { writeln!(file, "  \"{}\" -> \"{}\";", arc.source.value, arc.target.value)?; }
        }
        writeln!(file, "}}")?;
        Ok(())
    }

    fn get_nodes(&self) -> &HashSet<Rc<Node<N>>> { &self.nodes }
    fn get_arcs(&self) -> &HashSet<Rc<Arc<N, W>>> { &self.arcs }
    fn get_outgoing_arcs(&self, node: &Rc<Node<N>>) -> Vec<Rc<Arc<N, W>>> { self.out_adj_list.get(node).cloned().unwrap_or_default() }
    fn get_incoming_arcs(&self, node: &Rc<Node<N>>) -> Vec<Rc<Arc<N, W>>> { self.in_adj_list.get(node).cloned().unwrap_or_default() }
}

/// Implementation of an Undirected Graph.
pub struct UndirectedGraph<N, W> {
    pub nodes: HashSet<Rc<Node<N>>>,
    pub arcs: HashSet<Rc<Arc<N, W>>>,
    out_adj_list: HashMap<Rc<Node<N>>, Vec<Rc<Arc<N, W>>>>,
    in_adj_list: HashMap<Rc<Node<N>>, Vec<Rc<Arc<N, W>>>>,
}

impl<N, W> UndirectedGraph<N, W> {
    pub fn new() -> Self {
        Self { nodes: HashSet::new(), arcs: HashSet::new(), out_adj_list: HashMap::new(), in_adj_list: HashMap::new() }
    }

    pub fn load_from_file(filename: &str) -> std::io::Result<Self> where N: FromStr, W: FromStr + Clone + 'static {
        let mut file = File::open(filename)?;
        let mut content = String::new();
        file.read_to_string(&mut content)?;
        let tokens: Vec<String> = content.split_whitespace().map(|s| s.to_string()).collect();

        let mut g = Self::new();
        if tokens.is_empty() { return Ok(g); }

        let mut token_idx = 0;
        let num_nodes: usize = tokens[token_idx].parse().unwrap_or(0); token_idx += 1;

        let mut index_to_node = Vec::new();
        for _ in 0..num_nodes {
            if let Ok(val) = tokens[token_idx].parse::<N>() { index_to_node.push(g.add_node_by_value(val)); }
            token_idx += 1;
        }

        if token_idx >= tokens.len() { return Ok(g); }
        let num_arcs: usize = tokens[token_idx].parse().unwrap_or(0); token_idx += 1;
        let tokens_per_arc = if num_arcs > 0 { (tokens.len() - token_idx) / num_arcs } else { 0 };

        for _ in 0..num_arcs {
            let src_idx: usize = tokens[token_idx].parse().unwrap();
            let tgt_idx: usize = tokens[token_idx + 1].parse().unwrap();
            if tokens_per_arc >= 3 {
                if let Ok(weight) = tokens[token_idx + 2].parse::<W>() { g.add_arc(index_to_node[src_idx].clone(), index_to_node[tgt_idx].clone(), weight); }
            } else {
                g.add_unweighted_arc(index_to_node[src_idx].clone(), index_to_node[tgt_idx].clone());
            }
            token_idx += tokens_per_arc;
        }
        Ok(g)
    }
}

impl<N, W> Default for UndirectedGraph<N, W> {
    fn default() -> Self {
        Self::new()
    }
}

impl<N, W> Graph<N, W> for UndirectedGraph<N, W> {
    fn add_node_by_value(&mut self, value: N) -> Rc<Node<N>> {
        let node = Rc::new(Node::new(value));
        self.nodes.insert(node.clone());
        self.out_adj_list.insert(node.clone(), Vec::new());
        self.in_adj_list.insert(node.clone(), Vec::new());
        node
    }

    fn add_node(&mut self, node: Rc<Node<N>>) {
        self.nodes.insert(node.clone());
        self.out_adj_list.entry(node.clone()).or_insert_with(Vec::new);
        self.in_adj_list.entry(node.clone()).or_insert_with(Vec::new);
    }

    fn add_arc(&mut self, source: Rc<Node<N>>, target: Rc<Node<N>>, weight: W) -> Rc<Arc<N, W>> where W: Clone + 'static {
        let arc = Rc::new(Arc::new_fixed(source.clone(), target.clone(), weight));
        self.arcs.insert(arc.clone());
        self.out_adj_list.get_mut(&source).unwrap().push(arc.clone());
        self.in_adj_list.get_mut(&source).unwrap().push(arc.clone());
        self.out_adj_list.get_mut(&target).unwrap().push(arc.clone());
        self.in_adj_list.get_mut(&target).unwrap().push(arc.clone());
        arc
    }

    fn add_unweighted_arc(&mut self, source: Rc<Node<N>>, target: Rc<Node<N>>) -> Rc<Arc<N, W>> {
        let arc = Rc::new(Arc::new_unweighted(source.clone(), target.clone()));
        self.arcs.insert(arc.clone());
        self.out_adj_list.get_mut(&source).unwrap().push(arc.clone());
        self.in_adj_list.get_mut(&source).unwrap().push(arc.clone());
        self.out_adj_list.get_mut(&target).unwrap().push(arc.clone());
        self.in_adj_list.get_mut(&target).unwrap().push(arc.clone());
        arc
    }

    fn add_arc_func(&mut self, source: Rc<Node<N>>, target: Rc<Node<N>>, weight_func: Box<dyn Fn() -> W>) -> Rc<Arc<N, W>> {
        let arc = Rc::new(Arc::new(source.clone(), target.clone(), Some(weight_func)));
        self.arcs.insert(arc.clone());
        self.out_adj_list.get_mut(&source).unwrap().push(arc.clone());
        self.in_adj_list.get_mut(&source).unwrap().push(arc.clone());
        self.out_adj_list.get_mut(&target).unwrap().push(arc.clone());
        self.in_adj_list.get_mut(&target).unwrap().push(arc.clone());
        arc
    }

    fn print_graph(&self) where N: Display, W: Display { for arc in &self.arcs { arc.print(false); } }
    fn to_dot_file(&self, filename: &str) -> std::io::Result<()> where N: Display, W: Display {
        let mut file = File::create(filename)?;
        writeln!(file, "graph G {{")?;
        for node in &self.nodes { writeln!(file, "  \"{}\";", node.value)?; }
        for arc in &self.arcs {
            if let Some(w) = arc.get_weight() { writeln!(file, "  \"{}\" -- \"{}\" [label=\"{}\"];", arc.source.value, arc.target.value, w)?; }
            else { writeln!(file, "  \"{}\" -- \"{}\";", arc.source.value, arc.target.value)?; }
        }
        writeln!(file, "}}")?;
        Ok(())
    }

    fn get_nodes(&self) -> &HashSet<Rc<Node<N>>> { &self.nodes }
    fn get_arcs(&self) -> &HashSet<Rc<Arc<N, W>>> { &self.arcs }
    fn get_outgoing_arcs(&self, node: &Rc<Node<N>>) -> Vec<Rc<Arc<N, W>>> { self.out_adj_list.get(node).cloned().unwrap_or_default() }
    fn get_incoming_arcs(&self, node: &Rc<Node<N>>) -> Vec<Rc<Arc<N, W>>> { self.in_adj_list.get(node).cloned().unwrap_or_default() }
}