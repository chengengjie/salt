#include "kry.h"

#include "salt/base/mst.h"

namespace salt {

void KryBuilder::Run(const Net& net, Tree& tree, double eps) {
    // MST
    Tree mst;
    MstBuilder mstB;
    mstB.Run(net, mst);

    // init
    Init(mst, net.source());

    // DFS
    DFS(mst.source, slSrc, eps);
    Finalize(net, tree);
}

bool KryBuilder::Relax(const shared_ptr<TreeNode>& u, const shared_ptr<TreeNode>& v) {
    DTYPE newDist = curDists[u->id] + Dist(u->loc, v->loc);
    if (curDists[v->id] > newDist) {
        curDists[v->id] = newDist;
        v->parent = u;
        return true;
    } else
        return false;
}

void KryBuilder::DFS(const shared_ptr<TreeNode>& mstNode, const shared_ptr<TreeNode>& slNode, double eps) {
    if (curDists[slNode->id] > (1 + eps) * shortestDists[slNode->id]) {
        slNode->parent = slSrc;
        curDists[slNode->id] = shortestDists[slNode->id];
    }
    for (auto c : mstNode->children) {
        Relax(slNode, slNodes[c->id]);
        DFS(c, slNodes[c->id], eps);
        Relax(slNodes[c->id], slNode);
    }
}

}  // namespace salt