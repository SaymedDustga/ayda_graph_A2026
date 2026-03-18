from abc import ABC, abstractmethod
from typing import TypeVar, Generic, Callable, Dict, Set, List, Optional, Union, Any, Iterator

NodeType = TypeVar('NodeType')
WeightType = TypeVar('WeightType')

class GraphException(Exception):
    """Exception class for Graph-related errors."""
    pass


class Node(Generic[NodeType]):
    """
    Represents a Node (Vertex) in the graph.

    Attributes:
        value (NodeType): The internal data stored inside the node.
    """

    def __init__(self, value: NodeType):
        """
        Initializes a new node with the given value.

        Args:
            value (NodeType): The data to be stored in the node.
        """
        self._value = value

    @property
    def value(self) -> NodeType:
        """NodeType: Gets the internal value of the node."""
        return self._value

    def __str__(self) -> str:
        """Returns the string representation of the node's value."""
        return str(self._value)


class Arc(Generic[NodeType, WeightType]):
    """
    Represents a directed or undirected Arc (Edge) between two nodes.

    Attributes:
        source (Node[NodeType]): The starting node of the arc.
        target (Node[NodeType]): The ending node of the arc.
    """

    def __init__(self,
                 source: Node[NodeType],
                 target: Node[NodeType],
                 weight: Union[WeightType, Callable[[], WeightType], None] = None):
        """
        Constructs an arc. If weight is a callable (lambda), it evaluates dynamically.
        If it's a fixed value, it wraps it in a lambda automatically to maintain closures logic.

        Args:
            source (Node[NodeType]): The origin node.
            target (Node[NodeType]): The destination node.
            weight (Union[WeightType, Callable, None]): The weight of the arc, a dynamic function, or None.
        """
        self._source = source
        self._target = target

        if callable(weight):
            self._weight_function = weight
        else:
            self._weight_function = lambda: weight

    @property
    def source(self) -> Node[NodeType]:
        """Node[NodeType]: Retrieves the source node."""
        return self._source

    @property
    def target(self) -> Node[NodeType]:
        """Node[NodeType]: Retrieves the target node."""
        return self._target

    def get_weight(self) -> Optional[WeightType]:
        """
        Evaluates and returns the current weight of the arc.

        Returns:
            Optional[WeightType]: The evaluated weight, or None if the graph is unweighted.
        """
        return self._weight_function()

    def set_weight_function(self, new_weight_func: Callable[[], WeightType]) -> None:
        """
        Updates the dynamic weight evaluation function.

        Args:
            new_weight_func (Callable[[], WeightType]): The new lambda function returning a weight.
        """
        self._weight_function = new_weight_func

    def print_arc(self, is_digraph: bool) -> None:
        """
        Prints the arc information to standard output.

        Args:
            is_digraph (bool): True if directed formatting should be used, False for undirected.
        """
        connection = "From: " if is_digraph else "Union With Nodes: "
        direction = " To: " if is_digraph else " And: "

        base_str = f"{connection}{self.source.value}{direction}{self.target.value}"

        weight = self.get_weight()
        if weight is not None:
            print(f"{base_str} Weight: {weight}")
        else:
            print(base_str)


class Graph(ABC, Generic[NodeType, WeightType]):
    """
    Abstract Base Class for Graphs.
    Contains the shared state and common persistence logic for all graph types.
    """

    def __init__(self):
        """Initializes an empty graph with internal data structures."""
        self._nodes: Set[Node[NodeType]] = set()
        self._arcs: Set[Arc[NodeType, WeightType]] = set()

        # Adjacency lists are critical for the O((V+E) log V) time complexity in algorithms.
        self._out_adj_list: Dict[Node[NodeType], List[Arc[NodeType, WeightType]]] = {}
        self._in_adj_list: Dict[Node[NodeType], List[Arc[NodeType, WeightType]]] = {}

    # --- Python Data Model Magic Methods ---

    def __len__(self) -> int:
        """Returns the total number of nodes in the graph in O(1) time."""
        return len(self._nodes)

    def __contains__(self, node: Node[NodeType]) -> bool:
        """Checks if a specific node exists within the graph in O(1) time."""
        return node in self._nodes

    def __iter__(self) -> Iterator[Node[NodeType]]:
        """Allows pythonic iteration over all nodes in the graph (e.g., 'for node in graph:')."""
        return iter(self._nodes)

    # ---------------------------------------

    def add_node_by_value(self, value: NodeType) -> Node[NodeType]:
        """
        Creates a new node by value and inserts it into the graph.

        Args:
            value (NodeType): The data for the new node.

        Returns:
            Node[NodeType]: The newly created and inserted node.
        """
        node = Node(value)
        self._nodes.add(node)
        self._out_adj_list[node] = []
        self._in_adj_list[node] = []
        return node

    def add_node(self, node: Node[NodeType]) -> None:
        """
        Inserts an already existing node instance into the graph.

        Args:
            node (Node[NodeType]): The node to insert.
        """
        self._nodes.add(node)
        if node not in self._out_adj_list:
            self._out_adj_list[node] = []
        if node not in self._in_adj_list:
            self._in_adj_list[node] = []

    @abstractmethod
    def add_arc(self, source: Node[NodeType], target: Node[NodeType], weight: Union[WeightType, Callable[[], WeightType], None] = None) -> Arc[NodeType, WeightType]:
        """Pure virtual method to enforce implementation in derived classes."""
        pass

    @abstractmethod
    def print_graph(self) -> None:
        """Pure virtual method to print the graph structure."""
        pass

    @abstractmethod
    def to_dot_file(self, filename: str) -> None:
        """Pure virtual method to export the graph to a Graphviz DOT file."""
        pass

    @property
    def nodes(self) -> Set[Node[NodeType]]:
        """Set[Node]: Retrieves the set of all nodes in the graph."""
        return self._nodes

    @property
    def arcs(self) -> Set[Arc[NodeType, WeightType]]:
        """Set[Arc]: Retrieves the set of all arcs in the graph."""
        return self._arcs

    def get_outgoing_arcs(self, node: Node[NodeType]) -> List[Arc[NodeType, WeightType]]:
        return self._out_adj_list.get(node, [])

    def get_incoming_arcs(self, node: Node[NodeType]) -> List[Arc[NodeType, WeightType]]:
        return self._in_adj_list.get(node, [])

    def get_out_degree(self, node: Node[NodeType]) -> int:
        return len(self.get_outgoing_arcs(node))

    def get_in_degree(self, node: Node[NodeType]) -> int:
        return len(self.get_incoming_arcs(node))

    def get_degree(self, node: Node[NodeType]) -> int:
        return self.get_out_degree(node) + self.get_in_degree(node)

    def save_to_file(self, filename: str) -> None:
        """
        Persists the graph's current state into a text file.
        Follows a sequential serialization format compatible with the C++ implementation.

        Args:
            filename (str): The destination file path.

        Raises:
            GraphException: If the file cannot be opened for writing.
        """
        try:
            with open(filename, 'w') as file:
                # 1. Write total number of nodes
                file.write(f"{len(self._nodes)}\n")

                # 2. Map nodes to integer indices for O(1) serialization
                node_to_index: Dict[Node[NodeType], int] = {}
                ordered_nodes = list(self._nodes)

                for index, node in enumerate(ordered_nodes):
                    node_to_index[node] = index
                    file.write(f"{node.value}\n")

                # 3. Write total number of arcs
                file.write(f"{len(self._arcs)}\n")

                # 4. Write arc connections
                for arc in self._arcs:
                    src_idx = node_to_index[arc.source]
                    tgt_idx = node_to_index[arc.target]
                    weight = arc.get_weight()

                    # Compile-time evaluation equivalent: Only write weight if it exists
                    if weight is not None:
                        file.write(f"{src_idx} {tgt_idx} {weight}\n")
                    else:
                        file.write(f"{src_idx} {tgt_idx}\n")
        except IOError:
            raise GraphException("Could not open file for writing.")

    @classmethod
    def load_from_file(cls, filename: str, value_cast: Callable[[str], Any] = str, weight_cast: Callable[[str], Any] = float) -> 'Graph':
        """
        Instantiates a new graph by deserializing a text file.
        Fully compatible with files generated by the C++ Graph library.

        Args:
            filename (str): The source file path.
            value_cast (Callable): Function to cast node values from string (e.g., int, str).
            weight_cast (Callable): Function to cast arc weights from string (e.g., float, int).

        Returns:
            Graph: The reconstructed DirectedGraph or UndirectedGraph.

        Raises:
            GraphException: If the file cannot be opened for reading.
        """
        g = cls() # Factory pattern: Instantiates DirectedGraph or UndirectedGraph automatically
        try:
            with open(filename, 'r') as file:
                tokens = file.read().split()
                if not tokens:
                    return g

                token_idx = 0
                num_nodes = int(tokens[token_idx])
                token_idx += 1

                index_to_node: List[Node[NodeType]] = []
                for _ in range(num_nodes):
                    val = value_cast(tokens[token_idx])
                    token_idx += 1
                    index_to_node.append(g.add_node_by_value(val))

                if token_idx >= len(tokens):
                    return g

                num_arcs = int(tokens[token_idx])
                token_idx += 1

                # Token counting to infer if the graph is weighted
                # Total remaining tokens / number of arcs. 3 means weighted (src, tgt, weight), 2 means unweighted.
                tokens_per_arc = (len(tokens) - token_idx) // num_arcs if num_arcs > 0 else 0

                for _ in range(num_arcs):
                    src_idx = int(tokens[token_idx])
                    tgt_idx = int(tokens[token_idx+1])

                    if tokens_per_arc >= 3:
                        weight_val = weight_cast(tokens[token_idx+2])
                        g.add_arc(index_to_node[src_idx], index_to_node[tgt_idx], weight_val)
                    else:
                        g.add_arc(index_to_node[src_idx], index_to_node[tgt_idx], None)

                    token_idx += tokens_per_arc

        except IOError:
            raise GraphException("Could not open file for reading.")

        return g


class DirectedGraph(Graph[NodeType, WeightType]):
    """Implementation of a Directed Graph structure."""

    def add_arc(self, source: Node[NodeType], target: Node[NodeType], weight: Union[WeightType, Callable[[], WeightType], None] = None) -> Arc[NodeType, WeightType]:
        """
        Adds a directed arc from the source node to the target node.

        Raises:
            GraphException: If the source or target node is not present in the graph.
        """
        if source not in self._nodes or target not in self._nodes:
            raise GraphException("Nodes Not Found!")

        arc = Arc(source, target, weight)
        self._arcs.add(arc)

        # Directed: The edge is outgoing from source and incoming to target.
        self._out_adj_list[source].append(arc)
        self._in_adj_list[target].append(arc)

        return arc

    def print_graph(self) -> None:
        """Prints all directed arcs in the graph."""
        for arc in self._arcs:
            arc.print_arc(is_digraph=True)

    def to_dot_file(self, filename: str) -> None:
        """
        Generates a DOT file for Graphviz visualization of a directed graph.

        Args:
            filename (str): The output file path (e.g., 'graph.dot').

        Raises:
            GraphException: If the file cannot be written.
        """
        try:
            with open(filename, 'w') as file:
                file.write("digraph G {\n")
                for node in self._nodes:
                    file.write(f'  "{node.value}";\n')

                for arc in self._arcs:
                    weight = arc.get_weight()
                    if weight is not None:
                        file.write(f'  "{arc.source.value}" -> "{arc.target.value}" [label="{weight}"];\n')
                    else:
                        file.write(f'  "{arc.source.value}" -> "{arc.target.value}";\n')
                file.write("}\n")
        except IOError:
            raise GraphException("The file could not be opened for writing.")


class UndirectedGraph(Graph[NodeType, WeightType]):
    """Implementation of an Undirected Graph structure."""

    def add_arc(self, source: Node[NodeType], target: Node[NodeType], weight: Union[WeightType, Callable[[], WeightType], None] = None) -> Arc[NodeType, WeightType]:
        """
        Adds an undirected arc (bidirectional connection) between source and target nodes.

        Raises:
            GraphException: If the source or target node is not present in the graph.
        """
        if source not in self._nodes or target not in self._nodes:
            raise GraphException("Nodes Not Found!")

        arc = Arc(source, target, weight)
        self._arcs.add(arc)

        # Undirected: The edge is incident to both source and target.
        self._out_adj_list[source].append(arc)
        self._in_adj_list[source].append(arc)
        self._out_adj_list[target].append(arc)
        self._in_adj_list[target].append(arc)

        return arc

    def print_graph(self) -> None:
        """Prints all undirected connections in the graph."""
        for arc in self._arcs:
            arc.print_arc(is_digraph=False)

    def to_dot_file(self, filename: str) -> None:
        """
        Generates a DOT file for Graphviz visualization of an undirected graph.

        Args:
            filename (str): The output file path (e.g., 'graph.dot').

        Raises:
            GraphException: If the file cannot be written.
        """
        try:
            with open(filename, 'w') as file:
                file.write("graph G {\n")
                for node in self._nodes:
                    file.write(f'  "{node.value}";\n')

                for arc in self._arcs:
                    weight = arc.get_weight()
                    if weight is not None:
                        file.write(f'  "{arc.source.value}" -- "{arc.target.value}" [label="{weight}"];\n')
                    else:
                        file.write(f'  "{arc.source.value}" -- "{arc.target.value}";\n')
                file.write("}\n")
        except IOError:
            raise GraphException("The file could not be opened for writing.")