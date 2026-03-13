/**
 * @file examples.cpp
 * @brief Detailed tutorial on how to use the Graph Library.
 * * Compile and run this file to see how to instantiate graphs, add nodes,
 * use dynamic weights, save/load from files, and use the Builders.
 */

#include <iostream>
#include <string>
#include <graph.hpp>
#include <graph_builders.hpp>

using namespace graph_lib;

void tutorialBasicGraph()
{
    std::cout << "--- 1. BASIC GRAPH CREATION ---\n";
    // Create a Directed Graph with String nodes and Double weights
    DirectedGraph<std::string, double> g;

    // Add nodes
    auto n1 = g.addNode("Caracas");
    auto n2 = g.addNode("Merida");
    auto n3 = g.addNode("Valencia");

    // Add arcs with fixed weights (Distance in km)
    g.addArc(n1, n2, 680.5);
    g.addArc(n1, n3, 170.0);

    g.printGraph();
    std::cout << "\n";
}

void tutorialDynamicWeights()
{
    std::cout << "--- 2. DYNAMIC WEIGHTS (LAMBDAS) ---\n";
    DirectedGraph<std::string, double> g;
    auto nA = g.addNode("A");
    auto nB = g.addNode("B");

    double traffic_jam_multiplier = 1.0;
    double base_time = 30.0; // 30 minutes

    // The weight dynamically reads the traffic variable
    auto arc = g.addArc(nA, nB, [&traffic_jam_multiplier, base_time]() {
        return base_time * traffic_jam_multiplier;
    });

    std::cout << "Time with no traffic: " << arc->getWeight() << " mins\n";

    traffic_jam_multiplier = 2.5; // Rush hour!
    std::cout << "Time during rush hour: " << arc->getWeight() << " mins\n\n";
}

void tutorialBuildersAndExport()
{
    std::cout << "--- 3. GRAPH BUILDERS & EXPORTING ---\n";

    // We want to build a 2x2 grid of unweighted undirected nodes
    auto name_generator = [](int r, int c) {
        return "N_" + std::to_string(r) + "_" + std::to_string(c);
    };

    auto grid = GraphBuilder<UndirectedGraph<std::string, NoWeight>>::buildRectangularGrid(2, 2, name_generator);

    std::cout << "Built a grid with " << grid.getNodes().size() << " nodes.\n";

    // Export to DOT format to visualize in Graphviz
    grid.toDotFile("my_grid.gv");
    std::cout << "Exported graph to 'my_grid.gv'. You can render it using Graphviz.\n\n";
}

void tutorialSaveAndLoad()
{
    std::cout << "--- 4. PERSISTENCE (SAVE / LOAD) ---\n";
    DirectedGraph<std::string, float> g;
    auto n1 = g.addNode("Start");
    auto n2 = g.addNode("End");
    g.addArc(n1, n2, 99.9f);

    std::string filename = "tutorial_graph.txt";
    g.saveToFile(filename);
    std::cout << "Graph saved to disk.\n";

    // Load it back
    auto loaded_g = DirectedGraph<std::string, float>::loadFromFile(filename);
    std::cout << "Graph loaded from disk! It has " << loaded_g.getArcs().size() << " arcs.\n";
}

int main()
{
    std::cout << "Welcome to the Graph Library Tutorial!\n\n";
    tutorialBasicGraph();
    tutorialDynamicWeights();
    tutorialBuildersAndExport();
    tutorialSaveAndLoad();
    return 0;
}