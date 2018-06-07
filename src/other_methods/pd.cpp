#include "pd.h"

namespace salt {

void PdBuilder::Run(const Net& net, Tree& tree, double eps) {
    double weight = 1 + eps;  // corresponds to 1/c in the paper
    const auto& pins = net.pins;
    const auto& src = net.source();

    // Init
    // dist
    vector<bool> visited(pins.size(), false);
    vector<DTYPE> dist(pins.size(), -1);  // -1: no visited
    int numUnvisited = pins.size();
    // cost
    vector<pair<double, double>> cost(pins.size(),
                                      pair<double, double>(numeric_limits<int>::max(), 0));  // <dist, edge>
    cost[src->id].first = 0;
    // pre
    vector<int> pre(pins.size(), 0);
    pre[src->id] = -1;

    int next = src->id;
    while (numUnvisited > 0) {
        assert(next != -1);
        int cur = next;

        if (cur != src->id)
            dist[cur] = dist[pre[cur]] + Dist(pins[pre[cur]]->loc, pins[cur]->loc);
        else
            dist[cur] = 0;
        visited[cur] = true;
        --numUnvisited;

        next = -1;
        pair<double, double> minCost(numeric_limits<int>::max(), 0);
        for (size_t i = 0; i < pins.size(); ++i) {
            if (!visited[i]) {
                double edge = Dist(pins[i]->loc, pins[cur]->loc);
                pair<double, double> newCost(dist[cur] + weight * edge, edge);
                if (newCost < cost[i]) {
                    cost[i] = newCost;
                    pre[i] = cur;
                }
                if (cost[i] < minCost) {
                    minCost = cost[i];
                    next = i;
                }
            }
        }
    }

    vector<shared_ptr<TreeNode>> nodes(net.pins.size());
    for (int i = 0; i < net.pins.size(); ++i) {
        auto p = net.pins[i];
        nodes[i] = make_shared<TreeNode>(p->loc, p, p->id);
    }
    for (int i = 0; i < net.pins.size(); ++i) {
        if (pre[i] != -1) TreeNode::SetParent(nodes[i], nodes[pre[i]]);
    }

    tree.source = nodes[net.source()->id];
    tree.net = &net;
}

}  // namespace salt