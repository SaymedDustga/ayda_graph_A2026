#pragma once

#include <graph.hpp>
#include <unordered_map>
#include <queue>
#include <limits>
#include <algorithm>
#include <type_traits>
#include <stdexcept>

namespace graph_lib
{

/**
 * @brief Implements Dijkstra's algorithm for finding shortest paths in weighted graphs.
 * @note The graph must have non-negative weights.
 */
template <typename GraphType>
class Dijkstra
{
public:
    using WeightType = typename GraphType::ArcWeightType;
    using NodePtr    = typename GraphType::NodePtr;
    using ArcPtr     = typename GraphType::ArcPtr;

    struct NodeInfo
    {
        WeightType distance;    ///< Current shortest distance from source.
        NodePtr predecessor;    ///< Previous node in the shortest path.
        ArcPtr incoming_arc;    ///< Arc used to reach this node.
    };

    /**
     * @brief Computes the Shortest Paths Tree (SPT) from a source node.
     * @param graph The input weighted graph.
     * @param source The source node for path computation.
     * @return GraphType A new graph containing only the arcs that form the shortest paths tree.
     * @par Time complexity: O((V + E) log V)
     */
    static GraphType getMinimumPathsTree(const GraphType& graph, NodePtr source)
    {
        static_assert(!std::is_same_v<WeightType, void>, "Error: Dijkstra requires a weighted graph.");

        // TODO: Implement Dijkstra's Algorithm using a Min-Heap (Priority Queue).
        throw std::logic_error("getMinimumPathsTree is not implemented yet!");
    }
};

/**
 * @brief Provides graph traversal algorithms (BFS, DFS) and related operations.
 */
template <typename GraphType>
class GraphTraversals
{
public:
    using NodePtr = typename GraphType::NodePtr;

    /**
     * @brief Performs a Breadth-First Search traversal starting from a node.
     * @param graph The graph to traverse.
     * @param start The starting node.
     * @param op Unary operation (Visitor pattern) invoked for each visited node.
     * @par Time complexity: O(V + E)
     */
    static void traverseBFS(const GraphType& graph, NodePtr start, std::function<void(NodePtr)> op)
    {
        // TODO: Implement BFS using a Queue. Ensure nodes are only visited once.
        // HINT: Use graph.getOutgoingArcs() and call op(current_node) when a node is processed.
        throw std::logic_error("traverseBFS is not implemented yet!");
    }

    /**
     * @brief Finds the shortest path between two nodes in an unweighted graph using BFS.
     * @param graph The graph to search.
     * @param start The source node.
     * @param end The destination node.
     * @return std::vector<NodePtr> A vector containing the nodes in the path, or empty if no path exists.
     */
    static std::vector<NodePtr> findShortestPathBFS(const GraphType& graph, NodePtr start, NodePtr end)
    {
        // TODO: Implement shortest path finding using BFS and a predecessors map.
        throw std::logic_error("findShortestPathBFS is not implemented yet!");
    }

    /**
     * @brief Performs a Depth-First Search traversal starting from a node.
     * @param graph The graph to traverse.
     * @param start The starting node.
     * @param op Unary operation (Visitor pattern) invoked for each visited node.
     * @par Time complexity: O(V + E)
     */
    static void traverseDFS(const GraphType& graph, NodePtr start, std::function<void(NodePtr)> op)
    {
        // TODO: Implement DFS (You may want to create a private recursive helper function).
        // HINT: Use graph.getOutgoingArcs() and call op(current_node) when a node is processed.
        throw std::logic_error("traverseDFS is not implemented yet!");
    }

    /**
     * @brief Builds a spanning tree using Depth-First Search.
     * @param graph The original graph.
     * @param start The root node for the spanning tree.
     * @return GraphType A new graph representing the spanning tree.
     */
    static GraphType buildSpanningTreeDFS(const GraphType& graph, NodePtr start)
    {
        // TODO: Traverse the graph using DFS and add the discovered edges to a new GraphType.
        throw std::logic_error("buildSpanningTreeDFS is not implemented yet!");
    }

    /**
     * @brief Builds a spanning tree using Breadth-First Search.
     * @param graph The original graph.
     * @param start The root node for the spanning tree.
     * @return GraphType A new graph representing the spanning tree.
     */
    static GraphType buildSpanningTreeBFS(const GraphType& graph, NodePtr start)
    {
        // TODO: Traverse the graph using BFS and add the discovered edges to a new GraphType.
        throw std::logic_error("buildSpanningTreeBFS is not implemented yet!");
    }
};

/**
 * @brief Analyzes graph properties such as cycles and connectivity.
 */
template <typename GraphType>
class GraphProperties
{
public:
    using NodePtr = typename GraphType::NodePtr;

    /**
     * @brief Detects cycles in a directed graph using Kahn's algorithm.
     * @param graph The graph to analyze.
     * @return true If the graph contains at least one cycle.
     */
    static bool hasCycle(const GraphType& graph)
    {
        // TODO: Implement cycle detection using Kahn's Algorithm concepts.
        // HINT: You can easily get in-degrees using graph.getInDegree(node).
        throw std::logic_error("hasCycle is not implemented yet!");
    }

    /**
     * @brief Groups nodes into connected components (Primarily for undirected graphs).
     * @param graph The graph to analyze.
     * @return std::vector<std::vector<NodePtr>> A list containing the connected components.
     */
    static std::vector<std::vector<NodePtr>> computeConnectedComponents(const GraphType& graph)
    {
        // TODO: Iterate over all nodes, launching a BFS/DFS for unvisited ones to form components.
        throw std::logic_error("computeConnectedComponents is not implemented yet!");
    }

    /**
     * @brief Computes Strongly Connected Components (SCC) using Kosaraju's Algorithm.
     * @param graph The directed graph to analyze.
     * @return std::vector<std::vector<NodePtr>> A list of strongly connected components.
     */
    static std::vector<std::vector<NodePtr>> computeStronglyConnectedComponents(const GraphType& graph)
    {
        // TODO: Implement Kosaraju's Algorithm.
        throw std::logic_error("computeStronglyConnectedComponents is not implemented yet!");
    }
};

/**
 * @brief Provides topological sorting and ranking for directed acyclic graphs (DAGs).
 */
template <typename GraphType>
class GraphTopological
{
public:
    using NodePtr = typename GraphType::NodePtr;

    /**
     * @brief Computes a topological ordering of the graph's nodes.
     * @param graph The directed acyclic graph to sort.
     * @return std::vector<NodePtr> A vector containing nodes in topological order.
     * @throws std::runtime_error If the graph contains a cycle.
     */
    static std::vector<NodePtr> getSort(const GraphType& graph)
    {
        // TODO: Implement Topological Sort using Kahn's Algorithm. Throw an exception on cycle.
        throw std::logic_error("getSort is not implemented yet!");
    }

    /**
     * @brief Computes topological ranks (depth levels) for each node.
     * @param graph The directed acyclic graph to analyze.
     * @return std::unordered_map<NodePtr, int> A map from nodes to their ranks.
     * @throws std::runtime_error If the graph contains a cycle.
     */
    static std::unordered_map<NodePtr, int> getRanks(const GraphType& graph)
    {
        // TODO: Compute ranks. Nodes with no dependencies have rank 0, their successors have rank 1...
        throw std::logic_error("getRanks is not implemented yet!");
    }
};

} // namespace graph_lib