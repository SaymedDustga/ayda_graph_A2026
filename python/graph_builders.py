import random
from typing import TypeVar, Type, Callable, List, Optional

from graph import Graph, Node

NodeType = TypeVar('NodeType')
GraphType = TypeVar('GraphType', bound=Graph)

class GraphBuilder:
    """
    Factory class to generate standard graph topologies.

    Separates creational logic from the core graph data structures.
    Useful for generating test cases, grids, and standard mathematical graphs.
    """

    @staticmethod
    def build_complete_graph(graph_class: Type[GraphType], n: int, node_name_generator: Callable[[int], NodeType], weight: Optional[float] = 1.0) -> GraphType:
        """
        Builds a Complete Graph (K_n), where every node is connected to every other node.

        Args:
            graph_class (Type[GraphType]): The class of the graph to instantiate.
            n (int): Total number of nodes to generate.
            node_name_generator (Callable[[int], NodeType]): A function providing a value/name for each node index.
            weight (Optional[float]): Default weight to assign to all generated arcs.

        Returns:
            GraphType: A newly instantiated complete graph.
        """
        g = graph_class()
        nodes = [g.add_node_by_value(node_name_generator(i)) for i in range(n)]

        for i in range(n):
            for j in range(i + 1, n):
                g.add_arc(nodes[i], nodes[j], weight)

        return g

    @staticmethod
    def build_path_graph(graph_class: Type[GraphType], n: int, node_name_generator: Callable[[int], NodeType], weight: Optional[float] = 1.0) -> GraphType:
        """
        Builds a linear Path Graph (P_n).

        Args:
            graph_class (Type[GraphType]): The class of the graph to instantiate.
            n (int): Total number of nodes to generate.
            node_name_generator (Callable[[int], NodeType]): A function providing a value/name for each node index.
            weight (Optional[float]): Default weight to assign to all generated arcs.

        Returns:
            GraphType: A newly instantiated path graph.
        """
        g = graph_class()
        nodes = [g.add_node_by_value(node_name_generator(i)) for i in range(n)]

        for i in range(n - 1):
            g.add_arc(nodes[i], nodes[i + 1], weight)

        return g

    @staticmethod
    def build_cycle_graph(graph_class: Type[GraphType], n: int, node_name_generator: Callable[[int], NodeType], weight: Optional[float] = 1.0) -> GraphType:
        """
        Builds a Cycle Graph (C_n) where the last node connects back to the first.

        Args:
            graph_class (Type[GraphType]): The class of the graph to instantiate.
            n (int): Total number of nodes to generate.
            node_name_generator (Callable[[int], NodeType]): A function providing a value/name for each node index.
            weight (Optional[float]): Default weight to assign to all generated arcs.

        Returns:
            GraphType: A newly instantiated cycle graph.
        """
        g = graph_class()
        nodes = [g.add_node_by_value(node_name_generator(i)) for i in range(n)]

        for i in range(n):
            # The modulo operator ensures the last node connects back to the 0th node
            g.add_arc(nodes[i], nodes[(i + 1) % n], weight)

        return g

    @staticmethod
    def build_random_graph(graph_class: Type[GraphType], n: int, p: float, node_name_generator: Callable[[int], NodeType]) -> GraphType:
        """
        Builds a Random Graph based on the Erdős-Rényi model (Bernoulli distribution).

        Args:
            graph_class (Type[GraphType]): The class of the graph to instantiate (e.g., DirectedGraph).
            n (int): Total number of nodes to generate.
            p (float): Probability of connecting any two discrete nodes (0.0 to 1.0).
            node_name_generator (Callable[[int], NodeType]): A function providing a value/name for each node index.

        Returns:
            GraphType: A newly instantiated random graph.
        """
        g = graph_class()
        nodes: List[Node[NodeType]] = []

        for i in range(n):
            nodes.append(g.add_node_by_value(node_name_generator(i)))

        for i in range(n):
            for j in range(n):
                if i == j:
                    continue

                # Bernoulli trial to determine edge existence
                if random.random() < p:
                    GraphBuilder._add_default_arc(g, nodes[i], nodes[j])

        return g

    @staticmethod
    def build_rectangular_grid(graph_class: Type[GraphType], rows: int, cols: int, node_name_generator: Callable[[int, int], NodeType]) -> GraphType:
        """
        Builds a Rectangular Grid (Lattice Graph).

        Args:
            graph_class (Type[GraphType]): The class of the graph to instantiate.
            rows (int): Number of rows in the grid.
            cols (int): Number of columns in the grid.
            node_name_generator (Callable[[int, int], NodeType]): A function providing a value/name based on row/col indices.

        Returns:
            GraphType: A newly instantiated rectangular grid graph.
        """
        g = graph_class()

        # 1. Initialize the grid matrix with placeholders
        grid: List[List[Node[NodeType]]] = [[None for _ in range(cols)] for _ in range(rows)]

        # 2. Populate the grid matrix with actual nodes
        for i in range(rows):
            for j in range(cols):
                grid[i][j] = g.add_node_by_value(node_name_generator(i, j))

        # 3. Connect Right (0, 1) and Down (1, 0) strictly to avoid duplicate/overlapping edges
        for i in range(rows):
            for j in range(cols):
                if j + 1 < cols:
                    GraphBuilder._add_default_arc(g, grid[i][j], grid[i][j + 1])
                if i + 1 < rows:
                    GraphBuilder._add_default_arc(g, grid[i][j], grid[i + 1][j])

        return g

    @staticmethod
    def _add_default_arc(g: Graph, src: Node, tgt: Node) -> None:
        """
        Helper method to safely instantiate an arc with a default weight.
        We default to 0.0 to maintain compatibility with weighted algorithms.
        """
        g.add_arc(src, tgt, 0.0)