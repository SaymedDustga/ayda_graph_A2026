#pragma once

#include <graph.hpp>
#include <random>
#include <string>
#include <vector>
#include <functional>

namespace graph_lib
{

/**
 * @brief Factory class to generate standard graph topologies.
 * * Separates creational logic from the core graph data structures.
 * Useful for generating test cases, grids, and standard mathematical graphs.
 * * @tparam GraphType The specific graph class to instantiate (e.g., DirectedGraph or UndirectedGraph).
 */
template <typename GraphType>
class GraphBuilder
{
public:
    using NodeType = typename GraphType::NodeValueType;
    using NodePtr = std::shared_ptr<Node<NodeType>>;

    /**
     * @brief Builds a Complete Graph (K_n), where every node is connected to every other node.
     * @param n Total number of nodes to generate.
     * @param node_name_generator A lambda function providing a value/name for each node index.
     * @return GraphType A newly instantiated complete graph.
     */
    static GraphType buildCompleteGraph(int n, std::function<NodeType(int)> node_name_generator)
    {
        GraphType g;
        std::vector<NodePtr> nodes;
        for (int i = 0; i < n; ++i)
        {
            nodes.push_back(g.addNode(node_name_generator(i)));
        }

        for (int i = 0; i < n; ++i)
        {
            for (int j = i + 1; j < n; ++j)
            {
                addDefaultArc(g, nodes[i], nodes[j]);
            }
        }
        return g;
    }

    /**
     * @brief Builds a linear Path Graph (P_n).
     * @param n Total number of nodes to generate.
     * @param node_name_generator A lambda function providing a value/name for each node index.
     * @return GraphType A newly instantiated path graph.
     */
    static GraphType buildPathGraph(int n, std::function<NodeType(int)> node_name_generator)
    {
        GraphType g;
        std::vector<NodePtr> nodes;
        for (int i = 0; i < n; ++i)
        {
            nodes.push_back(g.addNode(node_name_generator(i)));
        }

        for (int i = 0; i < n - 1; ++i)
        {
            addDefaultArc(g, nodes[i], nodes[i + 1]);
        }
        return g;
    }

    /**
     * @brief Builds a Cycle Graph (C_n) where the last node connects back to the first.
     * @param n Total number of nodes to generate.
     * @param node_name_generator A lambda function providing a value/name for each node index.
     * @return GraphType A newly instantiated cycle graph.
     */
    static GraphType buildCycleGraph(int n, std::function<NodeType(int)> node_name_generator)
    {
        GraphType g;
        std::vector<NodePtr> nodes;
        for (int i = 0; i < n; ++i)
        {
            nodes.push_back(g.addNode(node_name_generator(i)));
        }

        for (int i = 0; i < n; ++i)
        {
            addDefaultArc(g, nodes[i], nodes[(i + 1) % n]);
        }
        return g;
    }

    /**
     * @brief Builds a Random Graph based on the Erdős-Rényi model (Bernoulli distribution).
     * @param n Total number of nodes to generate.
     * @param p Probability of connecting any two discrete nodes (0.0 to 1.0).
     * @param node_name_generator A lambda function providing a value/name for each node index.
     * @return GraphType A newly instantiated random graph.
     */
    static GraphType buildRandomGraph(int n, double p, std::function<NodeType(int)> node_name_generator)
    {
        GraphType g;
        std::vector<NodePtr> nodes;

        for (int i = 0; i < n; ++i)
        {
            nodes.push_back(g.addNode(node_name_generator(i)));
        }

        std::random_device rd;
        std::mt19937 gen(rd());
        std::bernoulli_distribution d(p);

        for (int i = 0; i < n; ++i)
        {
            for (int j = 0; j < n; ++j)
            {
                if (i == j) continue;

                // Bernoulli trial to determine edge existence
                if (d(gen))
                {
                    addDefaultArc(g, nodes[i], nodes[j]);
                }
            }
        }
        return g;
    }

    /**
     * @brief Builds a Rectangular Grid (Lattice Graph).
     * @param rows Number of rows in the grid.
     * @param cols Number of columns in the grid.
     * @param node_name_generator A lambda function providing a value/name based on row/col indices.
     * @return GraphType A newly instantiated rectangular grid graph.
     */
    static GraphType buildRectangularGrid(int rows, int cols, std::function<NodeType(int, int)> node_name_generator)
    {
        GraphType g;
        std::vector<std::vector<NodePtr>> grid(rows, std::vector<NodePtr>(cols));

        // 1. Initialize the grid matrix with nodes
        for (int i = 0; i < rows; ++i)
        {
            for (int j = 0; j < cols; ++j)
            {
                grid[i][j] = g.addNode(node_name_generator(i, j));
            }
        }

        // 2. Connect Right (0, 1) and Down (1, 0) strictly to avoid duplicate/overlapping edges
        for (int i = 0; i < rows; ++i)
        {
            for (int j = 0; j < cols; ++j)
            {
                if (j + 1 < cols) addDefaultArc(g, grid[i][j], grid[i][j + 1]);
                if (i + 1 < rows) addDefaultArc(g, grid[i][j], grid[i + 1][j]);
            }
        }

        return g;
    }

private:
    /**
     * @brief Helper method to safely instantiate an arc with a default weight (or NoWeight).
     * Utilizes compile-time evaluation (if constexpr) to maintain type safety.
     */
    static void addDefaultArc(GraphType& g, NodePtr src, NodePtr tgt)
    {
        using WeightType = typename GraphType::ArcWeightType;
        if constexpr (!std::is_same_v<WeightType, NoWeight>)
        {
            g.addArc(src, tgt, WeightType{}); // Uses default constructor for the weight type (e.g., 0.0)
        }
        else
        {
            g.addArc(src, tgt, NoWeight{});
        }
    }
};

} // namespace graph_lib