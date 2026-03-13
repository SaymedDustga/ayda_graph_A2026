#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <stdexcept>
#include <unordered_set>
#include <graph.hpp>
#include <graph_algorithms.hpp>
#include <graph_builders.hpp>

using namespace graph_lib;
using TestGraph = DirectedGraph<std::string, double>;
using UndirectedTestGraph = UndirectedGraph<std::string, double>;

// Helper function to safely run tests and catch "Not Implemented" errors
void runTest(const std::string& testName, std::function<void()> testFunc)
{
    std::cout << "Running " << testName << "... ";
    try {
        testFunc();
        std::cout << "\033[32m[PASSED]\033[0m\n"; // Green
    } catch (const std::logic_error& e) {
        std::cout << "\033[33m[PENDING]\033[0m (" << e.what() << ")\n"; // Yellow
    } catch (const std::exception& e) {
        std::cout << "\033[31m[FAILED]\033[0m (" << e.what() << ")\n"; // Red
    }
}

void testTraverseBFS()
{
    UndirectedTestGraph g;
    auto n1 = g.addNode("1"); auto n2 = g.addNode("2");
    auto n3 = g.addNode("3"); auto n4 = g.addNode("4");
    g.addArc(n1, n2, 0.0); g.addArc(n1, n3, 0.0); g.addArc(n2, n4, 0.0);

    std::vector<UndirectedTestGraph::NodePtr> visited_nodes;
    GraphTraversals<UndirectedTestGraph>::traverseBFS(g, n1, [&visited_nodes](UndirectedTestGraph::NodePtr n) {
        visited_nodes.push_back(n);
    });

    assert(visited_nodes.size() == 4 && "BFS should visit all 4 connected nodes exactly once.");
    assert(visited_nodes[0] == n1 && "BFS should start at the specified root node.");
}

void testTraverseDFS()
{
    UndirectedTestGraph g;
    auto n1 = g.addNode("1"); auto n2 = g.addNode("2");
    auto n3 = g.addNode("3"); auto n4 = g.addNode("4");
    g.addArc(n1, n2, 0.0); g.addArc(n2, n3, 0.0); g.addArc(n3, n4, 0.0);

    std::vector<UndirectedTestGraph::NodePtr> visited_nodes;
    GraphTraversals<UndirectedTestGraph>::traverseDFS(g, n1, [&visited_nodes](UndirectedTestGraph::NodePtr n) {
        visited_nodes.push_back(n);
    });

    assert(visited_nodes.size() == 4 && "DFS should visit all 4 connected nodes exactly once.");
    assert(visited_nodes[0] == n1 && "DFS should start at the specified root node.");
}

void testShortestPathBFS()
{
    UndirectedTestGraph g;
    auto n1 = g.addNode("1"); auto n2 = g.addNode("2");
    auto n3 = g.addNode("3"); auto n4 = g.addNode("4");
    g.addArc(n1, n2, 0.0); g.addArc(n1, n3, 0.0);
    g.addArc(n2, n4, 0.0); g.addArc(n3, n4, 0.0);

    auto path = GraphTraversals<UndirectedTestGraph>::findShortestPathBFS(g, n1, n4);
    assert(path.size() == 3 && "Shortest path from 1 to 4 should have 3 nodes");
    assert((path[1] == n2 || path[1] == n3) && "Middle node must be 2 or 3");
}

void testSpanningTreeDFS()
{
    UndirectedTestGraph g;
    auto n1 = g.addNode("1"); auto n2 = g.addNode("2");
    auto n3 = g.addNode("3"); auto n4 = g.addNode("4");
    g.addArc(n1, n2, 0.0); g.addArc(n1, n3, 0.0);
    g.addArc(n2, n4, 0.0); g.addArc(n3, n4, 0.0); // Cycle exists

    auto tree = GraphTraversals<UndirectedTestGraph>::buildSpanningTreeDFS(g, n1);
    assert(tree.getNodes().size() == 4 && "Spanning tree must include all nodes");
    assert(tree.getArcs().size() == 3 && "Spanning tree of 4 nodes must have exactly 3 arcs (no cycles)");
}

void testTopologicalSort()
{
    TestGraph g;
    auto n1 = g.addNode("CS101"); auto n2 = g.addNode("CS102");
    auto n3 = g.addNode("CS201"); auto n4 = g.addNode("CS301");

    g.addArc(n1, n2, 0.0); g.addArc(n2, n3, 0.0); g.addArc(n3, n4, 0.0);

    auto topo_order = GraphTopological<TestGraph>::getSort(g);
    assert(topo_order.size() == 4);
    assert(topo_order[0] == n1 && "First course must be CS101");
    assert(topo_order[3] == n4 && "Last course must be CS301");
}

void testTopologicalRanks()
{
    TestGraph g;
    auto n1 = g.addNode("CS101"); auto n2 = g.addNode("CS102");
    auto n3 = g.addNode("CS201"); auto n4 = g.addNode("CS301");

    g.addArc(n1, n2, 0.0); g.addArc(n2, n3, 0.0); g.addArc(n3, n4, 0.0);

    auto ranks = GraphTopological<TestGraph>::getRanks(g);
    assert(ranks[n1] == 0 && "CS101 should have rank 0");
    assert(ranks[n4] == 3 && "CS301 should have rank 3");
}

void testDijkstra()
{
    TestGraph g;
    auto nA = g.addNode("A"); auto nB = g.addNode("B"); auto nC = g.addNode("C");
    auto nD = g.addNode("D"); auto nE = g.addNode("E");

    g.addArc(nA, nB, 10.0); g.addArc(nA, nC, 3.0); g.addArc(nB, nD, 2.0);
    g.addArc(nC, nB, 1.0);  g.addArc(nC, nD, 8.0); g.addArc(nC, nE, 2.0);
    g.addArc(nE, nD, 9.0);

    auto spt = Dijkstra<TestGraph>::getMinimumPathsTree(g, nA);
    assert(spt.getNodes().size() == 5);
    assert(spt.getArcs().size() == 4);
}

void testKosarajuSCC()
{
    TestGraph g;
    auto nA = g.addNode("A"); auto nB = g.addNode("B");
    auto nC = g.addNode("C"); auto nD = g.addNode("D"); auto nE = g.addNode("E");

    g.addArc(nA, nB, 1.0); g.addArc(nB, nC, 1.0); g.addArc(nC, nA, 1.0);
    g.addArc(nC, nD, 1.0); // Bridge
    g.addArc(nD, nE, 1.0); g.addArc(nE, nD, 1.0);

    auto scc = GraphProperties<TestGraph>::computeStronglyConnectedComponents(g);
    assert(scc.size() == 2 && "Graph should have exactly 2 SCCs");
}

int main()
{
    std::cout << "==========================================\n";
    std::cout << "   GRAPH LIBRARY - STUDENT AUTOGRADER     \n";
    std::cout << "==========================================\n\n";

    runTest("Traverse BFS", testTraverseBFS);
    runTest("Traverse DFS", testTraverseDFS);
    runTest("Shortest Path (BFS)", testShortestPathBFS);
    runTest("Spanning Tree (DFS)", testSpanningTreeDFS);

    std::cout << "------------------------------------------\n";

    runTest("Topological Sort", testTopologicalSort);
    runTest("Topological Ranks", testTopologicalRanks);

    std::cout << "------------------------------------------\n";

    runTest("Dijkstra's Algorithm", testDijkstra);
    runTest("Kosaraju's SCC Algorithm", testKosarajuSCC);

    std::cout << "\n==========================================\n";
    return 0;
}