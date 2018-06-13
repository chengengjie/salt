#include "brbc.h"

#include "graph.h"
#include "salt/base/mst.h"

#include <boost/graph/dijkstra_shortest_paths.hpp>

namespace salt {

void BrbcBuilder::ExportPath(const Net& net, Tree& path) {
    Tree mst;
    MstBuilder mstB;
    mstB.Run(net, mst);
    path.net = &net;
    path.source = make_shared<TreeNode>(net.source()->loc, net.source(), net.source()->id);
    auto cur = path.source;
    mst.PreOrder([&](const shared_ptr<TreeNode>& tn) {
        if (!tn->parent) return;
        auto p = tn->pin;
        auto pre = cur;
        cur = make_shared<TreeNode>(p->loc, p, p->id);
        TreeNode::SetParent(cur, pre);
    });
}

void BrbcBuilder::Run(const Net& net, Tree& tree, double eps) {
    // init graph by MST
    Tree mst;
    MstBuilder mstB;
    mstB.Run(net, mst);
    Graph g = Graph(net.pins.size());
    boost::property_map<Graph, boost::edge_weight_t>::type weightmap = get(boost::edge_weight, g);
    mst.PreOrder([&](const shared_ptr<TreeNode>& tn) {
        if (!tn->parent) return;
        Edge e;
        bool inserted;
        tie(e, inserted) = add_edge(tn->pin->id, tn->parent->pin->id, g);  // Vertex is type int?
        weightmap[e] = Dist(tn->loc, tn->parent->loc);
    });

    // depth-first tour (n-1 vertices)
    DTYPE curDist = 0, shortestDist;
    Point preLoc = net.source()->loc;
    mst.PreOrder([&](const shared_ptr<TreeNode>& tn) {
        if (!tn->parent) return;
        auto p = tn->pin;
        curDist += Dist(p->loc, preLoc);
        shortestDist = Dist(p->loc, net.source()->loc);
        if (this->Comp(curDist, shortestDist, eps)) {
            this->Update(curDist, shortestDist);
            auto res = add_edge(p->id, net.source()->id, g);
            weightmap[res.first] = shortestDist;
        }
        preLoc = p->loc;
    });

    // Dijkstra
    vector<Vertex> p(num_vertices(g));
    vector<DTYPE> d(num_vertices(g));
    Vertex s = vertex(net.source()->id, g);
    dijkstra_shortest_paths(
        g,
        s,
        predecessor_map(boost::make_iterator_property_map(p.begin(), get(boost::vertex_index, g)))
            .distance_map(boost::make_iterator_property_map(d.begin(), get(boost::vertex_index, g))));

    // transform to saltTree
    vector<shared_ptr<TreeNode>> nodes(net.pins.size());
    for (int i = 0; i < net.pins.size(); ++i) {
        auto p = net.pins[i];
        nodes[i] = make_shared<TreeNode>(p->loc, p, p->id);
    }
    boost::graph_traits<Graph>::vertex_iterator vi, vend;
    for (boost::tie(vi, vend) = vertices(g); vi != vend; ++vi) {
        int down = *vi, up = p[*vi];
        if (up == down) continue;
        TreeNode::SetParent(nodes[down], nodes[up]);
    }

    tree.source = nodes[net.source()->id];
    tree.net = &net;
}

}  // namespace salt