#pragma once

#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <memory>
#include <exception>
#include <functional>
#include <fstream>
#include <type_traits>
#include <string>

namespace graph_lib
{

/**
 * @brief Exception class for Graph-related errors.
 */
class GraphException : public std::exception
{
private:
    std::string message;

public:
    explicit GraphException(const std::string& message) : message(message) {}

    const char* what() const noexcept override
    {
        return message.c_str();
    }
};

/**
 * @brief Dummy type used to represent unweighted graphs without needing full class specializations.
 * This eliminates the need for boilerplate code associated with 'void' weights.
 */
struct NoWeight {};

// Forward declarations
template <typename NodeType, typename WeightType = NoWeight>
class Arc;

template <typename NodeType, typename WeightType = NoWeight>
class Graph;

/**
 * @brief Represents a Node (Vertex) in the graph.
 * @tparam NodeType The data type stored inside the node.
 */
template <typename NodeType>
class Node
{
private:
    NodeType value;

public:
    explicit Node(const NodeType& val) : value(val) {}

    const NodeType& getValue() const noexcept
    {
        return value;
    }
};

/**
 * @brief Represents a directed or undirected Arc (Edge) between two nodes.
 * @tparam NodeType The type of data stored in the nodes.
 * @tparam WeightType The type of the weight. Defaults to NoWeight for unweighted graphs.
 */
template <typename NodeType, typename WeightType>
class Arc
{
private:
    std::shared_ptr<Node<NodeType>> source;
    std::shared_ptr<Node<NodeType>> target;

    // Using std::function allows for dynamic weight evaluation while maintaining Type Safety.
    // Instead of std::any, users can capture external variables using lambda closures.
    std::function<WeightType()> weight_function;

public:
    /**
     * @brief Constructs an arc with a dynamic weight function.
     */
    Arc(std::shared_ptr<Node<NodeType>> src,
        std::shared_ptr<Node<NodeType>> tgt,
        std::function<WeightType()> weight_func = []() { return WeightType{}; })
        : source(std::move(src)), target(std::move(tgt)), weight_function(std::move(weight_func)) {}

    /**
     * @brief Convenience constructor for fixed weights.
     * Automatically wraps the fixed value in a lambda function.
     */
    Arc(std::shared_ptr<Node<NodeType>> src,
        std::shared_ptr<Node<NodeType>> tgt,
        const WeightType& fixed_weight)
        : source(std::move(src)), target(std::move(tgt)),
          weight_function([fixed_weight]() { return fixed_weight; }) {}

    std::shared_ptr<Node<NodeType>> getSrcNode() const noexcept { return source; }
    std::shared_ptr<Node<NodeType>> getTgtNode() const noexcept { return target; }

    /**
     * @brief Evaluates and returns the current weight of the arc.
     */
    WeightType getWeight() const noexcept
    {
        return weight_function();
    }

    void setWeightFunction(std::function<WeightType()> new_weight_func) noexcept
    {
        weight_function = std::move(new_weight_func);
    }

    void print(bool isDiGraph) const noexcept
    {
        std::cout << (isDiGraph ? "From: " : "Union With Nodes: ")
                  << source->getValue()
                  << (isDiGraph ? " To: " : " And: ")
                  << target->getValue();

        // Compile-time check: Only print weight if the graph actually has weights.
        if constexpr (!std::is_same_v<WeightType, NoWeight>)
        {
            std::cout << " Weight: " << getWeight();
        }
        std::cout << '\n';
    }
};

/**
 * @brief Abstract Base Class for Graphs.
 */
template <typename NodeType, typename WeightType>
class Graph
{
protected:
    // NOTE: std::unordered_set hashes the raw pointer address by default.
    // If you need to identify nodes by their internal value, consider providing a custom hash function.
    std::unordered_set<std::shared_ptr<Node<NodeType>>> nodes;
    std::unordered_set<std::shared_ptr<Arc<NodeType, WeightType>>> arcs;

    // Adjacency list is critical for the O((V+E) log V) time complexity in algorithms like Dijkstra.
    std::unordered_map<std::shared_ptr<Node<NodeType>>, std::vector<std::shared_ptr<Arc<NodeType, WeightType>>>> adj_list;

public:
    using NodeValueType = NodeType;
    using ArcWeightType = WeightType;
    using NodePtr = std::shared_ptr<Node<NodeType>>;
    using ArcPtr  = std::shared_ptr<Arc<NodeType, WeightType>>;

    virtual ~Graph() = default;

    std::shared_ptr<Node<NodeType>> addNode(const NodeType& value)
    {
        auto node = std::make_shared<Node<NodeType>>(value);
        nodes.insert(node);
        return node;
    }

    void addNode(std::shared_ptr<Node<NodeType>> node)
    {
        nodes.insert(std::move(node));
    }

    // Pure virtual methods to enforce implementation in derived classes
    virtual std::shared_ptr<Arc<NodeType, WeightType>> addArc(
        std::shared_ptr<Node<NodeType>> source,
        std::shared_ptr<Node<NodeType>> target,
        std::function<WeightType()> weight_func) = 0;

    virtual std::shared_ptr<Arc<NodeType, WeightType>> addArc(
        std::shared_ptr<Node<NodeType>> source,
        std::shared_ptr<Node<NodeType>> target,
        const WeightType& fixed_weight) = 0;

    virtual void printGraph() const = 0;
    virtual void toDotFile(const std::string& filename) const = 0;

    const auto& getNodes() const noexcept { return nodes; }
    const auto& getArcs() const noexcept { return arcs; }

    /**
     * @brief Retrieves all incident arcs for a given node. Essential for efficient graph traversal.
     */
    const std::vector<std::shared_ptr<Arc<NodeType, WeightType>>>& getIncidentArcs(const std::shared_ptr<Node<NodeType>>& node) const
    {
        static const std::vector<std::shared_ptr<Arc<NodeType, WeightType>>> empty_list;
        auto it = adj_list.find(node);
        return it != adj_list.end() ? it->second : empty_list;
    }
};

/**
 * @brief Implementation of a Directed Graph.
 */
template <typename NodeType, typename WeightType = NoWeight>
class DirectedGraph : public Graph<NodeType, WeightType>
{
public:
    std::shared_ptr<Arc<NodeType, WeightType>> addArc(
        std::shared_ptr<Node<NodeType>> source,
        std::shared_ptr<Node<NodeType>> target,
        std::function<WeightType()> weight_func = [] { return WeightType{}; }) override
    {
        if (this->nodes.find(source) == this->nodes.end() || this->nodes.find(target) == this->nodes.end())
        {
            throw GraphException("Nodes Not Found!");
        }

        auto arc = std::make_shared<Arc<NodeType, WeightType>>(source, target, std::move(weight_func));
        this->arcs.insert(arc);
        this->adj_list[source].push_back(arc);
        return arc;
    }

    std::shared_ptr<Arc<NodeType, WeightType>> addArc(
        std::shared_ptr<Node<NodeType>> source,
        std::shared_ptr<Node<NodeType>> target,
        const WeightType& value) override
    {
        if (this->nodes.find(source) == this->nodes.end() || this->nodes.find(target) == this->nodes.end())
        {
            throw GraphException("Nodes Not Found!");
        }

        auto arc = std::make_shared<Arc<NodeType, WeightType>>(source, target, value);
        this->arcs.insert(arc);
        this->adj_list[source].push_back(arc);
        return arc;
    }

    void printGraph() const noexcept override
    {
        for (const auto& arc : this->arcs)
        {
            arc->print(true);
        }
    }

    void toDotFile(const std::string& filename) const override
    {
        std::ofstream file(filename);
        if (!file.is_open())
        {
            throw GraphException("The file could not be opened for writing.");
        }

        file << "digraph G {\n";
        for (const auto& node : this->nodes)
        {
            file << "  \"" << node->getValue() << "\";\n";
        }

        for (const auto& arc : this->arcs)
        {
            file << "  \"" << arc->getSrcNode()->getValue() << "\" -> \"" << arc->getTgtNode()->getValue() << "\"";

            // SFINAE / Compile-time evaluation: Only append label if the graph has weights
            if constexpr (!std::is_same_v<WeightType, NoWeight>)
            {
                file << " [label=\"" << arc->getWeight() << "\"]";
            }
            file << ";\n";
        }
        file << "}\n";
    }

    /**
     * @brief Persists the graph's current state into a text file.
     * Follows a sequential serialization format: Node count, Node data, Arc count, Arc connections.
     * @param filename The destination file path.
     * @throws GraphException if the file cannot be opened.
     */
    void saveToFile(const std::string& filename) const
    {
        std::ofstream file(filename);
        if (!file.is_open()) throw GraphException("Could not open file for writing.");

        // 1. Write total number of nodes
        file << this->nodes.size() << "\n";

        // 2. Map nodes to integer indices for O(1) serialization and write node values
        std::unordered_map<std::shared_ptr<Node<NodeType>>, int> node_to_index;
        int index = 0;

        std::vector<std::shared_ptr<Node<NodeType>>> ordered_nodes(this->nodes.begin(), this->nodes.end());
        for (const auto& node : ordered_nodes)
        {
            node_to_index[node] = index++;
            file << node->getValue() << "\n";
        }

        // 3. Write total number of arcs
        file << this->arcs.size() << "\n";

        // 4. Write arc connections using node indices and evaluate weight functions
        for (const auto& arc : this->arcs)
        {
            file << node_to_index[arc->getSrcNode()] << " " << node_to_index[arc->getTgtNode()];

            // Compile-time evaluation: Only write weight if the graph is weighted
            if constexpr (!std::is_same_v<WeightType, NoWeight>)
            {
                file << " " << arc->getWeight();
            }
            file << "\n";
        }
    }

    /**
     * @brief Instantiates a new graph by deserializing a text file.
     * @param filename The source file path.
     * @return DirectedGraph<NodeType, WeightType> The reconstructed graph.
     * @throws GraphException if the file cannot be opened or parsed.
     */
    static DirectedGraph<NodeType, WeightType> loadFromFile(const std::string& filename)
    {
        DirectedGraph<NodeType, WeightType> g;
        std::ifstream file(filename);
        if (!file.is_open()) throw GraphException("Could not open file for reading.");

        int num_nodes;
        if (!(file >> num_nodes)) return g;

        std::vector<std::shared_ptr<Node<NodeType>>> index_to_node(num_nodes);
        for (int i = 0; i < num_nodes; ++i)
        {
            NodeType value;
            file >> value;
            index_to_node[i] = g.addNode(value);
        }

        int num_arcs;
        if (!(file >> num_arcs)) return g;

        for (int i = 0; i < num_arcs; ++i)
        {
            int src_idx, tgt_idx;
            file >> src_idx >> tgt_idx;

            if constexpr (!std::is_same_v<WeightType, NoWeight>)
            {
                WeightType weight;
                file >> weight;
                g.addArc(index_to_node[src_idx], index_to_node[tgt_idx], weight);
            }
            else
            {
                g.addArc(index_to_node[src_idx], index_to_node[tgt_idx], NoWeight{});
            }
        }
        return g;
    }
};

/**
 * @brief Implementation of an Undirected Graph.
 */
template <typename NodeType, typename WeightType = NoWeight>
class UndirectedGraph : public Graph<NodeType, WeightType>
{
public:
    std::shared_ptr<Arc<NodeType, WeightType>> addArc(
        std::shared_ptr<Node<NodeType>> source,
        std::shared_ptr<Node<NodeType>> target,
        std::function<WeightType()> weight_func = [] { return WeightType{}; }) override
    {
        if (this->nodes.find(source) == this->nodes.end() || this->nodes.find(target) == this->nodes.end())
        {
            throw GraphException("Nodes Not Found!");
        }

        auto arc = std::make_shared<Arc<NodeType, WeightType>>(source, target, std::move(weight_func));
        this->arcs.insert(arc);

        // Undirected: The edge is incident to both source and target.
        this->adj_list[source].push_back(arc);
        this->adj_list[target].push_back(arc);

        return arc;
    }

    std::shared_ptr<Arc<NodeType, WeightType>> addArc(
        std::shared_ptr<Node<NodeType>> source,
        std::shared_ptr<Node<NodeType>> target,
        const WeightType& value) override
    {
        if (this->nodes.find(source) == this->nodes.end() || this->nodes.find(target) == this->nodes.end())
        {
            throw GraphException("Nodes Not Found!");
        }

        auto arc = std::make_shared<Arc<NodeType, WeightType>>(source, target, value);
        this->arcs.insert(arc);

        // Undirected: The edge is incident to both source and target.
        this->adj_list[source].push_back(arc);
        this->adj_list[target].push_back(arc);

        return arc;
    }

    void printGraph() const noexcept override
    {
        for (const auto& arc : this->arcs)
        {
            arc->print(false);
        }
    }

    void toDotFile(const std::string& filename) const override
    {
        std::ofstream file(filename);
        if (!file.is_open())
        {
            throw GraphException("The file could not be opened for writing.");
        }

        file << "graph G {\n";
        for (const auto& node : this->nodes)
        {
            file << "  \"" << node->getValue() << "\";\n";
        }

        for (const auto& arc : this->arcs)
        {
            file << "  \"" << arc->getSrcNode()->getValue() << "\" -- \"" << arc->getTgtNode()->getValue() << "\"";

            // SFINAE / Compile-time evaluation: Only append label if the graph has weights
            if constexpr (!std::is_same_v<WeightType, NoWeight>)
            {
                file << " [label=\"" << arc->getWeight() << "\"]";
            }
            file << ";\n";
        }
        file << "}\n";
    }

    /**
     * @brief Persists the graph's current state into a text file.
     * Follows a sequential serialization format: Node count, Node data, Arc count, Arc connections.
     * @param filename The destination file path.
     * @throws GraphException if the file cannot be opened.
     */
    void saveToFile(const std::string& filename) const
    {
        std::ofstream file(filename);
        if (!file.is_open()) throw GraphException("Could not open file for writing.");

        // 1. Write total number of nodes
        file << this->nodes.size() << "\n";

        // 2. Map nodes to integer indices for O(1) serialization and write node values
        std::unordered_map<std::shared_ptr<Node<NodeType>>, int> node_to_index;
        int index = 0;

        std::vector<std::shared_ptr<Node<NodeType>>> ordered_nodes(this->nodes.begin(), this->nodes.end());
        for (const auto& node : ordered_nodes)
        {
            node_to_index[node] = index++;
            file << node->getValue() << "\n";
        }

        // 3. Write total number of arcs
        file << this->arcs.size() << "\n";

        // 4. Write arc connections using node indices and evaluate weight functions
        for (const auto& arc : this->arcs)
        {
            file << node_to_index[arc->getSrcNode()] << " " << node_to_index[arc->getTgtNode()];

            // Compile-time evaluation: Only write weight if the graph is weighted
            if constexpr (!std::is_same_v<WeightType, NoWeight>)
            {
                file << " " << arc->getWeight();
            }
            file << "\n";
        }
    }

    /**
     * @brief Instantiates a new graph by deserializing a text file.
     * @param filename The source file path.
     * @return DirectedGraph<NodeType, WeightType> The reconstructed graph.
     * @throws GraphException if the file cannot be opened or parsed.
     */
    static DirectedGraph<NodeType, WeightType> loadFromFile(const std::string& filename)
    {
        DirectedGraph<NodeType, WeightType> g;
        std::ifstream file(filename);
        if (!file.is_open()) throw GraphException("Could not open file for reading.");

        int num_nodes;
        if (!(file >> num_nodes)) return g;

        std::vector<std::shared_ptr<Node<NodeType>>> index_to_node(num_nodes);
        for (int i = 0; i < num_nodes; ++i)
        {
            NodeType value;
            file >> value;
            index_to_node[i] = g.addNode(value);
        }

        int num_arcs;
        if (!(file >> num_arcs)) return g;

        for (int i = 0; i < num_arcs; ++i)
        {
            int src_idx, tgt_idx;
            file >> src_idx >> tgt_idx;

            if constexpr (!std::is_same_v<WeightType, NoWeight>)
            {
                WeightType weight;
                file >> weight;
                g.addArc(index_to_node[src_idx], index_to_node[tgt_idx], weight);
            }
            else
            {
                g.addArc(index_to_node[src_idx], index_to_node[tgt_idx], NoWeight{});
            }
        }
        return g;
    }
};

} // namespace graph_lib