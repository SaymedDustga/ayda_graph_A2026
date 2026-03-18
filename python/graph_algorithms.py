import heapq
from collections import deque
from typing import TypeVar, Dict, List, Set, Callable, Optional, Tuple

from graph import Graph, Node, Arc

NodeType = TypeVar('NodeType')
WeightType = TypeVar('WeightType')
GraphType = TypeVar('GraphType', bound=Graph)


class Dijkstra:
    """Implements Dijkstra's algorithm for finding shortest paths in weighted graphs."""

    @staticmethod
    def get_minimum_paths_tree(graph: GraphType, source: Node[NodeType]) -> GraphType:
        """
        Computes the Shortest Paths Tree (SPT) from a source node using a Min-Heap.

        Args:
            graph (GraphType): The input weighted graph.
            source (Node[NodeType]): The source node for path computation.

        Returns:
            GraphType: A new graph containing only the arcs that form the shortest paths tree.

        Time complexity: O((V + E) log V)
        """
        if not isinstance(graph, Graph): raise TypeError("Expected a Graph instance.")
        if source not in graph: raise ValueError("Source node not in graph.")

        # TODO: Implement Dijkstra's Algorithm using Python's heapq.
        # Remember to handle ties in the priority queue properly (e.g., using id(node)).
        raise NotImplementedError("Dijkstra.get_minimum_paths_tree is not implemented yet!")


class GraphTraversals:
    """Provides graph traversal algorithms (BFS, DFS) and related operations."""

    @staticmethod
    def traverse_bfs(graph: GraphType, start: Node[NodeType], op: Callable[[Node[NodeType]], None]) -> None:
        """
        Performs a Breadth-First Search traversal starting from a given node.

        Args:
            graph (GraphType): The graph to traverse.
            start (Node[NodeType]): The starting node.
            op (Callable): Unary operation (Visitor pattern) invoked for each visited node.
        """
        if not isinstance(graph, Graph): raise TypeError("Expected a Graph instance.")
        if start not in graph: raise ValueError("Source node not in graph.")

        # TODO: Implement BFS using collections.deque
        # HINT: Use graph.get_outgoing_arcs() and call op(current_node) when processed.
        raise NotImplementedError("traverse_bfs is not implemented yet!")

    @staticmethod
    def find_shortest_path_bfs(graph: GraphType, start: Node[NodeType], end: Node[NodeType]) -> List[Node[NodeType]]:
        """
        Finds the shortest path between two nodes in an unweighted graph using BFS.
        """
        if not isinstance(graph, Graph): raise TypeError("Expected a Graph instance.")
        if start not in graph or end not in graph: raise ValueError("Start or End node not in graph.")

        # TODO: Implement shortest path finding using BFS and a predecessors tracking dictionary.
        raise NotImplementedError("find_shortest_path_bfs is not implemented yet!")

    @staticmethod
    def traverse_dfs(graph: GraphType, start: Node[NodeType], op: Callable[[Node[NodeType]], None]) -> None:
        """
        Performs a Depth-First Search traversal starting from a node.
        """
        if not isinstance(graph, Graph): raise TypeError("Expected a Graph instance.")
        if start not in graph: raise ValueError("Source node not in graph.")

        # TODO: Implement DFS. (Hint: Create a private recursive helper method `_dfs_recursive`).
        # HINT: Use graph.get_outgoing_arcs() and call op(current_node) when processed.
        raise NotImplementedError("traverse_dfs is not implemented yet!")

    @staticmethod
    def build_spanning_tree_dfs(graph: GraphType, start: Node[NodeType]) -> GraphType:
        """Builds a spanning tree using Depth-First Search traversal."""
        if not isinstance(graph, Graph): raise TypeError("Expected a Graph instance.")
        if start not in graph: raise ValueError("Source node not in graph.")

        # TODO: Traverse the graph using DFS and build a new GraphType.
        raise NotImplementedError("build_spanning_tree_dfs is not implemented yet!")

    @staticmethod
    def build_spanning_tree_bfs(graph: GraphType, start: Node[NodeType]) -> GraphType:
        """Builds a spanning tree using Breadth-First Search traversal."""
        if not isinstance(graph, Graph): raise TypeError("Expected a Graph instance.")
        if start not in graph: raise ValueError("Source node not in graph.")

        # TODO: Traverse the graph using BFS and build a new GraphType.
        raise NotImplementedError("build_spanning_tree_bfs is not implemented yet!")


class GraphProperties:
    """Analyzes graph properties such as cycles, connected components, and dependencies."""

    @staticmethod
    def has_cycle(graph: GraphType) -> bool:
        """
        Detects cycles in a directed graph using Kahn's algorithm.
        Returns True if the graph contains at least one cycle, False otherwise.
        """
        if not isinstance(graph, Graph): raise TypeError("Expected a Graph instance.")

        # TODO: Implement cycle detection using in-degrees.
        # HINT: You can easily get in-degrees using graph.get_in_degree(node).
        raise NotImplementedError("has_cycle is not implemented yet!")

    @staticmethod
    def compute_connected_components(graph: GraphType) -> List[List[Node[NodeType]]]:
        """
        Groups nodes into connected components. Primarily designed for undirected graphs.
        """
        if not isinstance(graph, Graph): raise TypeError("Expected a Graph instance.")

        # TODO: Iterate over all nodes, launching a BFS/DFS for unvisited ones to form components.
        raise NotImplementedError("compute_connected_components is not implemented yet!")

    @staticmethod
    def compute_strongly_connected_components(graph: GraphType) -> List[List[Node[NodeType]]]:
        """
        Computes Strongly Connected Components (SCC) using Kosaraju's Algorithm.
        """
        if not isinstance(graph, Graph): raise TypeError("Expected a Graph instance.")

        # TODO: Implement Kosaraju's Algorithm (Requires reversing the graph).
        raise NotImplementedError("compute_strongly_connected_components is not implemented yet!")


class GraphTopological:
    """Provides topological sorting and ranking for directed acyclic graphs (DAGs)."""

    @staticmethod
    def get_sort(graph: GraphType) -> List[Node[NodeType]]:
        """
        Computes a topological ordering of the graph's nodes.
        Raises a RuntimeError if the graph contains a cycle.
        """
        if not isinstance(graph, Graph): raise TypeError("Expected a Graph instance.")

        # TODO: Implement Topological Sort (Kahn's Algorithm).
        raise NotImplementedError("get_sort is not implemented yet!")

    @staticmethod
    def get_ranks(graph: GraphType) -> Dict[Node[NodeType], int]:
        """
        Computes topological ranks (depth levels) for each node.
        Raises a RuntimeError if the graph contains a cycle.
        """
        if not isinstance(graph, Graph): raise TypeError("Expected a Graph instance.")

        # TODO: Compute depth ranks based on dependencies.
        raise NotImplementedError("get_ranks is not implemented yet!")