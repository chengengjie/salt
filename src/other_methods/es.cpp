#include "es.h"

#include "salt/base/mst.h"

namespace salt {

void EsRsaBuilder::Run(const Net& net, Tree& tree) {
    // Shift all pins to make source (0,0)
    auto oriSrcLoc = net.source()->loc;
    for (auto& p : net.pins) p->loc -= oriSrcLoc;

    // MST path (excluding source)
    Tree mst;
    MstBuilder mstB;
    mstB.Run(net, mst);
    vector<shared_ptr<TreeNode>> nodes;
    mst.PreOrder([&](const shared_ptr<TreeNode>& tn) {
        if (!tn->parent) return;
        auto p = tn->pin;
        nodes.push_back(make_shared<TreeNode>(p->loc, p, p->id));
    });

    // iterative merging
    while (nodes.size() > 1) {
        vector<shared_ptr<TreeNode>> nodes2;
        for (size_t i = 1; i < nodes.size(); i += 2) {
            auto a = nodes[i - 1];
            auto b = nodes[i];
            Point newP;
            newP.x = MaxOvlp(a->loc.x, b->loc.x);
            newP.y = MaxOvlp(a->loc.y, b->loc.y);
            if (newP == a->loc) {
                TreeNode::SetParent(b, a);
                nodes2.push_back(a);
            } else if (newP == b->loc) {
                TreeNode::SetParent(a, b);
                nodes2.push_back(b);
            } else {
                shared_ptr<TreeNode> newN = make_shared<TreeNode>(newP);
                TreeNode::SetParent(a, newN);
                TreeNode::SetParent(b, newN);
                nodes2.push_back(newN);
            }
        }
        if (nodes.size() % 2 == 1) {
            nodes2.push_back(nodes.back());
        }
        nodes = move(nodes2);
    }

    // connect to source
    auto last = nodes[0];
    if (last->loc == (net.source())->loc) {
        tree.source = last;
        last->id = net.source()->id;
        last->pin = net.source();
    } else {
        tree.source = make_shared<TreeNode>(net.source()->loc, net.source(), net.source()->id);
        TreeNode::SetParent(last, tree.source);
    }

    // shift all pins back
    for (auto& p : net.pins) p->loc += oriSrcLoc;
    tree.PreOrder([&](const shared_ptr<TreeNode>& node) { node->loc += oriSrcLoc; });
    tree.net = &net;
}

}  // namespace salt