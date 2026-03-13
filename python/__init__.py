"""
Graph Library - A comprehensive graph data structure and algorithms library.

This package provides a complete graph implementation with:
- Directed and Undirected graphs with template-like type safety
- Weighted and unweighted edges with dynamic weight evaluation
- Advanced graph algorithms (Dijkstra, BFS, DFS, Topological sort, etc.)
- Graph properties analysis (cycles, connectivity, SCC)
- Graph builders for random graphs and grids
- Persistence (save/load) compatible with C++ version

Example:
    >>> from graph_lib import DirectedGraph, Dijkstra
    >>> g = DirectedGraph[str, float]()
    >>> a = g.add_node_by_value("A")
    >>> b = g.add_node_by_value("B")
    >>> g.add_arc(a, b, 10.5)
    >>> spt = Dijkstra.get_minimum_paths_tree(g, a)
"""

__version__ = "1.0.0"
__author__ = "Gerardo A. Rosetti M."
__all__ = [
    # Core classes
    "Graph",
    "DirectedGraph",
    "UndirectedGraph",
    "Node",
    "Arc",

    # Algorithms
    "Dijkstra",
    "GraphTraversals",
    "GraphProperties",
    "GraphTopological",

    # Builders
    "GraphBuilder",

    # Exceptions
    "GraphException",
]

# Import order matters for circular dependencies
from .graph import (
    GraphException,
    Node,
    Arc,
    Graph,
    DirectedGraph,
    UndirectedGraph,
)

from .graph_algorithms import (
    Dijkstra,
    GraphTraversals,
    GraphProperties,
    GraphTopological,
)

from .graph_builders import GraphBuilder


def create_directed_graph():
    """Create a new directed graph instance."""
    return DirectedGraph()


def create_undirected_graph():
    """Create a new undirected graph instance."""
    return UndirectedGraph()