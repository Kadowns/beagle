//
// Created by Ricardo on 5/22/2021.
//

#ifndef BEAGLE_GRAPH_H
#define BEAGLE_GRAPH_H

#include <vector>
#include <set>
#include <unordered_map>
#include <queue>
#include <cassert>

namespace beagle {

template<typename V, typename E>
class Graph {
private:

    struct Node {

        typedef std::unordered_map<size_t, E> ConnectionMap;

        Node(V&& v) : data(std::move(v)) {}
        Node(const V& v) : data(v) {}
        V data;
        ConnectionMap connectionsTo;
        std::set<size_t> connectionsFrom;
    };

public:

    size_t insert(V&& v){
        if (m_freeIndices.empty()){
            m_nodes.emplace_back(std::move(v));
            return m_nodes.size() - 1;
        }
        size_t index = m_freeIndices.back();
        m_freeIndices.pop_back();
        m_nodes[index].data = std::move(v);
        return index;
    }

    size_t insert(const V& v){
        if (m_freeIndices.empty()){
            m_nodes.emplace_back(v);
            return m_nodes.size() - 1;
        }
        size_t index = m_freeIndices.back();
        m_freeIndices.pop_back();
        m_nodes[index].data = v;
        return index;
    }

    void erase(size_t index){
        assert(m_nodes.size() > index && "Index out of bounds");
        assert(std::find(m_freeIndices.begin(), m_freeIndices.end(), index) == m_freeIndices.end() && "Index was already erased");
        m_freeIndices.emplace_back(index);

        auto& node = m_nodes[index];
        for (auto [connectedIndex, edge] : node.connectionsTo){
            m_nodes[connectedIndex].connectionsFrom.erase(index);
        }
        node.connectionsTo.clear();

        for (auto connectedIndex : node.connectionsFrom){
            m_nodes[connectedIndex].connectionsTo.erase(index);
        }
        node.connectionsFrom.clear();
    }

    void connect(size_t from, size_t to, const E& edge){
        m_nodes[from].connectionsTo.emplace(to, edge);
        m_nodes[to].connectionsFrom.insert(from);
    }

    void disconnect(size_t from, size_t to){
        m_nodes[from].connectionsTo.erase(to);
        m_nodes[to].connectionsFrom.erase(from);
    }

    V& vertex(size_t index){
        assert(m_nodes.size() > index && "Index out of bounds");
        assert(std::find(m_freeIndices.begin(), m_freeIndices.end(), index) == m_freeIndices.end() && "Index was already erased");
        return m_nodes[index].data;
    }

    size_t size() const {
        return m_nodes.size() - m_freeIndices.size();
    }

    size_t capacity() const {
        return m_nodes.capacity();
    }

    class GraphIterator : public std::iterator<std::input_iterator_tag, size_t> {
    public:
        bool operator==(const GraphIterator& rhs) const { return m_i == rhs.m_i; }

        bool operator!=(const GraphIterator& rhs) const { return m_i != rhs.m_i; }

        V& operator*() {
            return m_owner->vertex(m_i);
        }

        GraphIterator& operator++() {
            m_i++;
            next();
            return *this;
        }

    protected:
        friend Graph<V, E>;
        GraphIterator(Graph<V, E>* owner, size_t i) : m_owner(owner), m_i(i), m_freeCursor(0) {}

        bool valid_vertex(){
            const std::vector<size_t> &freeList = m_owner->m_freeIndices;
            if (m_freeCursor < freeList.size() && freeList[m_freeCursor] == m_i) {
                ++m_freeCursor;
                return false;
            }
            return true;
        }

        void next(){
            while (m_i < m_owner->m_nodes.size() && !valid_vertex()) {
                ++m_i;
            }
        }

    protected:
        Graph<V, E>* m_owner;
        size_t m_i;
        size_t m_freeCursor;

    };

    GraphIterator begin() { return GraphIterator(this, 0); }
    GraphIterator end() { return GraphIterator(this, m_nodes.size()); }

    GraphIterator begin() const { return GraphIterator(this, 0); }
    GraphIterator end() const { return GraphIterator(this, m_nodes.size()); }

    class EdgesFromView {
    public:
        typedef typename Node::ConnectionMap::const_iterator ConstIterator;

        explicit EdgesFromView(const Node& owner) : m_owner(owner) {}

        ConstIterator begin() const { return m_owner.connectionsTo.begin(); }
        ConstIterator end() const { return m_owner.connectionsTo.end(); }
    private:
        const Node& m_owner;
    };

    EdgesFromView edges_from(size_t index) const {
        return EdgesFromView(m_nodes[index]);
    }

private:
    std::vector<Node> m_nodes;
    std::vector<size_t> m_freeIndices;
};


}


#endif //BEAGLE_GRAPH_H