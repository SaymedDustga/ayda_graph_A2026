"""
Detailed tutorial on how to use the Graph Library in Python.
Run this file to see how to instantiate graphs, add nodes, use dynamic weights,
save/load from files, and use the Builders.
"""

from graph import DirectedGraph, UndirectedGraph
from graph_builders import GraphBuilder

def tutorial_basic_graph() -> None:
    print("--- 1. BASIC GRAPH CREATION ---")
    g = DirectedGraph()

    # Add nodes
    n1 = g.add_node_by_value("Caracas")
    n2 = g.add_node_by_value("Merida")
    n3 = g.add_node_by_value("Valencia")

    # Add arcs with fixed weights (Distance in km)
    g.add_arc(n1, n2, 680.5)
    g.add_arc(n1, n3, 170.0)

    g.print_graph()
    print()

def tutorial_dynamic_weights() -> None:
    print("--- 2. DYNAMIC WEIGHTS (LAMBDAS) ---")
    g = DirectedGraph()
    n_a = g.add_node_by_value("A")
    n_b = g.add_node_by_value("B")

    # Use a dictionary to allow the lambda to capture mutable state
    context = {"traffic_multiplier": 1.0}
    base_time = 30.0 # 30 minutes

    # The weight dynamically reads the context
    arc = g.add_arc(n_a, n_b, lambda: base_time * context["traffic_multiplier"])

    print(f"Time with no traffic: {arc.get_weight()} mins")

    context["traffic_multiplier"] = 2.5 # Rush hour!
    print(f"Time during rush hour: {arc.get_weight()} mins\n")

def tutorial_builders_and_export() -> None:
    print("--- 3. GRAPH BUILDERS & EXPORTING ---")

    name_generator = lambda r, c: f"N_{r}_{c}"

    # Build a 2x2 grid
    grid = GraphBuilder.build_rectangular_grid(UndirectedGraph, 2, 2, name_generator)

    print(f"Built a grid with {len(grid.nodes)} nodes.")

    # Export to DOT format to visualize in Graphviz
    grid.to_dot_file("my_grid.gv")
    print("Exported graph to 'my_grid.gv'. You can render it using Graphviz.\n")

def tutorial_save_and_load() -> None:
    print("--- 4. PERSISTENCE (SAVE / LOAD) ---")
    g = DirectedGraph()
    n1 = g.add_node_by_value("Start")
    n2 = g.add_node_by_value("End")
    g.add_arc(n1, n2, 99.9)

    filename = "tutorial_graph.txt"
    g.save_to_file(filename)
    print("Graph saved to disk.")

    # Load it back (Specifying the casting types)
    loaded_g = DirectedGraph.load_from_file(filename, value_cast=str, weight_cast=float)
    print(f"Graph loaded from disk! It has {len(loaded_g.arcs)} arcs.")

if __name__ == "__main__":
    print("Welcome to the Graph Library Tutorial!\n")
    tutorial_basic_graph()
    tutorial_dynamic_weights()
    tutorial_builders_and_export()
    tutorial_save_and_load()