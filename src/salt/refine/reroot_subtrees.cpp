#include "refine.h"

#include "salt/base/eval.h"
#include "salt/base/mst.h"

namespace salt {

void RerootSubtreesOnce(Tree& tree, double eps) {
    // nearest neighbors
    tree.UpdateId();
    vector<shared_ptr<TreeNode>> nodes = tree.ObtainNodes(), orderedNodes(nodes.size());  // note: all pins should be covered
    vector<Point> points(nodes.size());
    for (int i = 0; i < nodes.size(); ++i) {
        orderedNodes[nodes[i]->id] = nodes[i];
        points[nodes[i]->id] = nodes[i]->loc;
    }
    nodes = orderedNodes;
    vector<vector<int>> nearestNeighbors;
    MstBuilder mstB;
    mstB.GetAllNearestNeighbors(points, nearestNeighbors);

    // prune descendants in nearest neighbors
    vector<int> preOrderIdxes(nodes.size(), -1);
    int globalPreOrderIdx = 0;
    function<void(shared_ptr<TreeNode>)> removeDescendants = [&](shared_ptr<TreeNode> node) {
        preOrderIdxes[node->id] = globalPreOrderIdx++;
        for (auto child : node->children) {
            removeDescendants(child);
        }
        for (auto& neighIdx : nearestNeighbors[node->id]) {
            int neighPreOrderIdx = preOrderIdxes[neighIdx];
            if (neighPreOrderIdx != -1 && neighPreOrderIdx >= preOrderIdxes[node->id]) {
                neighIdx = -1;  // -1 stands for "descendant"
            }
        }
    };
    removeDescendants(tree.source);

    // init path lengths and subtree slacks
    vector<DTYPE> pathLengths(nodes.size());
    tree.PreOrder([&](shared_ptr<TreeNode> node){
        if (node->parent) {
            pathLengths[node->id] = pathLengths[node->parent->id] + node->WireToParent();
        }
        else {
            pathLengths[node->id] = 0;
        }
    });
    vector<DTYPE> slacks(nodes.size());
    tree.PostOrder([&](shared_ptr<TreeNode> node){
        if (node->children.empty()) {
            slacks[node->id] = Dist(node->loc, tree.source->loc) * (1 + eps) - pathLengths[node->id];   // floor here...
        }
        else {
            DTYPE minSlack = Dist(node->loc, tree.source->loc) * (1 + eps) - pathLengths[node->id];
            for (auto child : node->children) {
                minSlack = min(minSlack, slacks[child->id]);
            }
            slacks[node->id] = minSlack;
        }
    });

    // only commit the one with the most wire length savings
    // TODO: try other orders and schemes
    vector<tuple<DTYPE, shared_ptr<TreeNode>, shared_ptr<TreeNode>>> candidateMoves;  // <wl_delta, node, new_parent>
    auto GetNearestPoint = [](shared_ptr<TreeNode> target, shared_ptr<TreeNode> neigh) {
        Box box(neigh->loc, neigh->parent->loc);
        box.Legalize();
        return box.GetNearestPointTo(target->loc);
    };
    for (auto node : nodes) {
        if (!(node->parent)) {
            continue;
        }
        DTYPE bestWireLengthDelta = 0;  // the negative, the better
        shared_ptr<TreeNode> bestNewParent;
        for (int neighIdx : nearestNeighbors[node->id]) {
            if (neighIdx == -1) continue;
            auto neigh = nodes[neighIdx];
            while (neigh->parent) {
                auto neighParent = neigh->parent;
                auto steinerPt = GetNearestPoint(node, neigh);
                DTYPE wireLengthDelta = Dist(node->loc, steinerPt) - node->WireToParent();
                if (wireLengthDelta >= 0) {
                    break;  // no potential for WL improvement, essential for runtime
                }
                else if (wireLengthDelta < bestWireLengthDelta) {
                    DTYPE pathLengthDelta = pathLengths[neighParent->id] + Dist(node->loc, neighParent->loc) - pathLengths[node->id];
                    if (pathLengthDelta <= slacks[node->id]) {
                        bestWireLengthDelta = wireLengthDelta;
                        bestNewParent = neigh;
                    }
                }
                neigh = neigh->parent;
            }
        }
        if (bestNewParent) {
            candidateMoves.emplace_back(bestWireLengthDelta, node, bestNewParent);
        }
    }
    if (candidateMoves.size() > 0) {
        auto move = *min_element(candidateMoves.begin(), candidateMoves.end());
        auto node = get<1>(move), neigh = get<2>(move);
        TreeNode::ResetParent(node);
        auto steinerPt = GetNearestPoint(node, neigh);
        if (steinerPt == neigh->loc) {
            TreeNode::SetParent(node, neigh);
        }
        else if (steinerPt == neigh->parent->loc) {
            TreeNode::SetParent(node, neigh->parent);
        }
        else {
            auto steinerNode = make_shared<TreeNode>(steinerPt);
            TreeNode::SetParent(steinerNode, neigh->parent);
            TreeNode::ResetParent(neigh);
            TreeNode::SetParent(neigh, steinerNode);
            TreeNode::SetParent(node, steinerNode);
        }
    }
}

void Refine::RerootSubtrees(Tree& tree, double eps, int maxIter) {
    WireLengthEvalBase cur(tree), pre;
    for (int i = 0; i < maxIter; ++i) {
        pre = cur;
        RerootSubtreesOnce(tree, eps);
        tree.RemoveTopoRedundantSteiner();
        cur.Update(tree);
        if (cur.wireLength == pre.wireLength) {
            break;
        }
    }
}

}