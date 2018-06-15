#include "bonn.h"
#include "salt/base/flute.h"

#include <cfloat>
#include <climits>
#include <queue>
#include <stack>

namespace salt {

void BonnBuilder::Run(const Net &net, Tree &saltTree, double epsilon) {
    assert(unitCap >= 0);
    double halfEpsilon = epsilon / 2;

    // get tree from flute
    FluteBuilder fluteB;
    fluteB.Run(net, saltTree);

    // init data
    int numNodes = saltTree.UpdateId();
    vector<double> downCaps(numNodes, 0.0);
    vector<DTYPE> minDists(numNodes, 0);
    vector<shared_ptr<TreeNode>> minDistNodes(numNodes);

    // post-order traversal
    saltTree.PostOrderCopy([&](shared_ptr<TreeNode> node) { // TODO: make it no copy
        if (node->parent && node->parent != saltTree.source) {
            double downCap = (node->pin) ? node->pin->cap : 0.0;
            DTYPE minDist = Dist(node->loc, saltTree.source->loc);
            auto minDistNode = node;
            for (auto child : node->children) {
                downCap += downCaps[child->id] + unitCap * child->WireToParent();
                if (minDists[child->id] < minDist) {
                    minDist = minDists[child->id];
                    minDistNode = minDistNodes[child->id];
                }
                minDist = min(minDists[child->id], minDist);
            }
            downCaps[node->id] = downCap;
            minDists[node->id] = minDist;
            minDistNodes[node->id] = minDistNode;
            if (downCap + unitCap * node->WireToParent() >= unitCap * halfEpsilon * minDist) {
                TreeNode::ResetParent(node);
                auto tap = minDistNode;
                TreeNode::Reroot(tap);
                TreeNode::SetParent(tap, saltTree.source);
            }
        }
    });

    saltTree.RemoveTopoRedundantSteiner();
}

double BonnBuilder::unitCap = 0;

}  // namespace salt