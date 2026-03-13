import sys
from graph import DirectedGraph, UndirectedGraph
from graph_algorithms import Dijkstra, GraphTraversals, GraphProperties, GraphTopological

def run_test(test_name: str, test_func: callable) -> None:
    """Helper function to safely run tests and catch NotImplemented errors."""
    print(f"Running {test_name.ljust(30)} ", end="")
    try:
        test_func()
        print("\033[32m[PASSED]\033[0m") # Green
    except NotImplementedError as e:
        print(f"\033[33m[PENDING]\033[0m ({str(e).split(' is ')[0]})") # Yellow
    except AssertionError as e:
        print(f"\033[31m[FAILED]\033[0m (Assertion Error: {e})") # Red
    except Exception as e:
        print(f"\033[31m[ERROR]\033[0m ({e})") # Red

def test_traverse_bfs() -> None:
    g = UndirectedGraph()
    n1 = g.add_node_by_value("1")
    n2 = g.add_node_by_value("2")
    n3 = g.add_node_by_value("3")
    n4 = g.add_node_by_value("4")
    g.add_arc(n1, n2, 0.0); g.add_arc(n1, n3, 0.0); g.add_arc(n2, n4, 0.0)

    visited_nodes = []
    def visit(n): visited_nodes.append(n)

    GraphTraversals.traverse_bfs(g, n1, visit)

    assert len(visited_nodes) == 4, "BFS should visit all 4 connected nodes exactly once."
    assert visited_nodes[0] == n1, "BFS should start at the specified root node."

def test_traverse_dfs() -> None:
    g = UndirectedGraph()
    n1 = g.add_node_by_value("1")
    n2 = g.add_node_by_value("2")
    n3 = g.add_node_by_value("3")
    n4 = g.add_node_by_value("4")
    g.add_arc(n1, n2, 0.0); g.add_arc(n2, n3, 0.0); g.add_arc(n3, n4, 0.0)

    visited_nodes = []
    def visit(n): visited_nodes.append(n)

    GraphTraversals.traverse_dfs(g, n1, visit)

    assert len(visited_nodes) == 4, "DFS should visit all 4 connected nodes exactly once."
    assert visited_nodes[0] == n1, "DFS should start at the specified root node."

def test_shortest_path_bfs() -> None:
    g = UndirectedGraph()
    n1 = g.add_node_by_value("1"); n2 = g.add_node_by_value("2")
    n3 = g.add_node_by_value("3"); n4 = g.add_node_by_value("4")
    g.add_arc(n1, n2, 0.0); g.add_arc(n1, n3, 0.0)
    g.add_arc(n2, n4, 0.0); g.add_arc(n3, n4, 0.0)

    path = GraphTraversals.find_shortest_path_bfs(g, n1, n4)
    assert len(path) == 3, "Shortest path from 1 to 4 should have 3 nodes"
    assert path[1] == n2 or path[1] == n3, "Middle node must be 2 or 3"

def test_spanning_tree_dfs() -> None:
    g = UndirectedGraph()
    n1 = g.add_node_by_value("1"); n2 = g.add_node_by_value("2")
    n3 = g.add_node_by_value("3"); n4 = g.add_node_by_value("4")
    g.add_arc(n1, n2, 0.0); g.add_arc(n1, n3, 0.0)
    g.add_arc(n2, n4, 0.0); g.add_arc(n3, n4, 0.0)

    tree = GraphTraversals.build_spanning_tree_dfs(g, n1)
    assert len(tree.nodes) == 4, "Spanning tree must include all nodes"
    assert len(tree.arcs) == 3, "Spanning tree of 4 nodes must have exactly 3 arcs"

def test_topological_sort() -> None:
    g = DirectedGraph()
    n1 = g.add_node_by_value("CS101"); n2 = g.add_node_by_value("CS102")
    n3 = g.add_node_by_value("CS201"); n4 = g.add_node_by_value("CS301")

    g.add_arc(n1, n2, 0.0); g.add_arc(n2, n3, 0.0); g.add_arc(n3, n4, 0.0)

    topo_order = GraphTopological.get_sort(g)
    assert len(topo_order) == 4
    assert topo_order[0] == n1, "First course must be CS101"
    assert topo_order[3] == n4, "Last course must be CS301"

def test_topological_ranks() -> None:
    g = DirectedGraph()
    n1 = g.add_node_by_value("CS101"); n2 = g.add_node_by_value("CS102")
    n3 = g.add_node_by_value("CS201"); n4 = g.add_node_by_value("CS301")

    g.add_arc(n1, n2, 0.0); g.add_arc(n2, n3, 0.0); g.add_arc(n3, n4, 0.0)

    ranks = GraphTopological.get_ranks(g)
    assert ranks[n1] == 0, "CS101 should have rank 0"
    assert ranks[n4] == 3, "CS301 should have rank 3"

def test_dijkstra() -> None:
    g = DirectedGraph()
    n_a = g.add_node_by_value("A"); n_b = g.add_node_by_value("B")
    n_c = g.add_node_by_value("C"); n_d = g.add_node_by_value("D")
    n_e = g.add_node_by_value("E")

    g.add_arc(n_a, n_b, 10.0); g.add_arc(n_a, n_c, 3.0); g.add_arc(n_b, n_d, 2.0)
    g.add_arc(n_c, n_b, 1.0);  g.add_arc(n_c, n_d, 8.0); g.add_arc(n_c, n_e, 2.0)
    g.add_arc(n_e, n_d, 9.0)

    spt = Dijkstra.get_minimum_paths_tree(g, n_a)
    assert len(spt.nodes) == 5
    assert len(spt.arcs) == 4

def test_kosaraju_scc() -> None:
    g = DirectedGraph()
    n_a = g.add_node_by_value("A"); n_b = g.add_node_by_value("B")
    n_c = g.add_node_by_value("C"); n_d = g.add_node_by_value("D")
    n_e = g.add_node_by_value("E")

    g.add_arc(n_a, n_b, 1.0); g.add_arc(n_b, n_c, 1.0); g.add_arc(n_c, n_a, 1.0)
    g.add_arc(n_c, n_d, 1.0) # Bridge
    g.add_arc(n_d, n_e, 1.0); g.add_arc(n_e, n_d, 1.0)

    scc = GraphProperties.compute_strongly_connected_components(g)
    assert len(scc) == 2, "Graph should have exactly 2 SCCs"

def main() -> None:
    print("==========================================")
    print("   GRAPH LIBRARY - STUDENT AUTOGRADER     ")
    print("==========================================\n")

    run_test("Traverse BFS", test_traverse_bfs)
    run_test("Traverse DFS", test_traverse_dfs)
    run_test("Shortest Path (BFS)", test_shortest_path_bfs)
    run_test("Spanning Tree (DFS)", test_spanning_tree_dfs)

    print("-" * 42)

    run_test("Topological Sort", test_topological_sort)
    run_test("Topological Ranks", test_topological_ranks)

    print("-" * 42)

    run_test("Dijkstra's Algorithm", test_dijkstra)
    run_test("Kosaraju's SCC Algorithm", test_kosaraju_scc)

    print("\n==========================================")

if __name__ == "__main__":
    main()