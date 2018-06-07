#include "bonn.h"
#include "salt/base/flute.h"

#include <cfloat>
#include <climits>
#include <queue>
#include <stack>

namespace salt {

// DFS:get order/cap/mindist/minid/wl
void DFS(Tree &saltTree,
         const double unit_cap,
         vector<shared_ptr<TreeNode>> &order,
         vector<double> &nodecap,
         vector<double> &mindist,
         vector<shared_ptr<TreeNode>> &minnode) {
    int numNodes = saltTree.UpdateId();

    stack<shared_ptr<TreeNode>> travel;
    nodecap.resize(numNodes, 0);
    mindist.resize(numNodes, DBL_MAX);
    minnode.resize(numNodes, saltTree.source);
    vector<bool> visit(numNodes, false);

    travel.push(saltTree.source);

    while (!travel.empty()) {
        shared_ptr<TreeNode> cur = travel.top();
        int id = cur->id;
        if (visit[id]) {
            travel.pop();
            // update dist and add cap of children to it
            double dist = Dist(cur->loc, saltTree.source->loc);
            if (dist < mindist[id]) {
                mindist[id] = dist;
                minnode[id] = cur;
            }
            if (cur->pin != nullptr) {
                nodecap[id] += cur->pin->cap;
            }
            // add its children cap and the wire cap
            for (auto ch : cur->children) {
                nodecap[id] += nodecap[ch->id];
                double wire = Dist(cur->loc, ch->loc);
                nodecap[id] += wire * unit_cap;
                if (mindist[ch->id] < mindist[id]) {
                    mindist[id] = mindist[ch->id];
                    minnode[id] = minnode[ch->id];
                }
            }
        } else {
            visit[id] = true;
            order.push_back(cur);

            for (auto ch : cur->children) {
                travel.push(ch);
            }
        }
    }
}

// BONN:netid/tree/order/cap/mindist/minid
void BONNalgo(Tree &saltTree,
              double epsilon,
              const double unit_cap,
              vector<shared_ptr<TreeNode>> &order,
              vector<double> &cap,
              vector<double> &mindist,
              vector<shared_ptr<TreeNode>> &minnode) {
    int numNodes = order.size();
    queue<shared_ptr<TreeNode>> travel;
    travel.push(saltTree.source);

    for (int k = numNodes - 1; k > 0; k--) {
        shared_ptr<TreeNode> cur = order[k];
        shared_ptr<TreeNode> parent = cur->parent;

        double dist = Dist(cur->loc, parent->loc);
        double md = min(mindist[cur->id], (double)Dist(parent->loc, saltTree.source->loc));

        if (cap[cur->id] + dist * unit_cap >= epsilon * 0.5 * md * unit_cap) {
            // change tree structure: from minnode to cur
            shared_ptr<TreeNode> next = minnode[cur->id];
            shared_ptr<TreeNode> newparent = saltTree.source;
            shared_ptr<TreeNode> oldparent = next->parent;
            while (next != cur) {
                oldparent = next->parent;
                // modify parent
                next->parent = newparent;
                // delete and add children
                newparent->children.push_back(next);
                for (vector<shared_ptr<TreeNode>>::iterator it = oldparent->children.begin();
                     it != oldparent->children.end();
                     it++) {
                    if (*it == next) {
                        // erase it
                        oldparent->children.erase(it);
                        break;
                    }
                }
                newparent = next;
                next = oldparent;
            }
            oldparent = cur->parent;
            cur->parent = newparent;
            // delete and add children
            newparent->children.push_back(cur);
            for (vector<shared_ptr<TreeNode>>::iterator it = oldparent->children.begin();
                 it != oldparent->children.end();
                 it++) {
                if (*it == cur) {
                    // erase it
                    oldparent->children.erase(it);
                    break;
                }
            }
            // update oldparent to source if minnode[cur->id]
            shared_ptr<TreeNode> update = oldparent;
            while (update != saltTree.source) {
                if (minnode[update->id] == minnode[cur->id]) {
                    double distance = DBL_MAX;
                    mindist[update->id] = distance = Dist(update->loc, saltTree.source->loc);
                    minnode[update->id] = update;
                    for (auto ch : update->children) {
                        if (mindist[ch->id] < distance && minnode[ch->id] != minnode[cur->id]) {
                            mindist[update->id] = mindist[ch->id];
                            minnode[update->id] = minnode[ch->id];
                        }
                    }
                    update = update->parent;
                } else
                    break;
            }
        }
    }
}

double BonnBuilder::unitCap = -1;

void BonnBuilder::Run(const Net &net, Tree &saltTree, double epsilon) {
    assert(unitCap > 0);

    // get tree from flute
    FluteBuilder fluteB;
    fluteB.Run(net, saltTree);

    vector<shared_ptr<TreeNode>> order;
    vector<double> nodecap;
    vector<double> mindist;
    vector<shared_ptr<TreeNode>> minnode;
    // dfs to get necessary information:order,nodecap, mindist, minid
    DFS(saltTree, unitCap, order, nodecap, mindist, minnode);
    // bonn algo to obtain a new tree
    BONNalgo(saltTree, epsilon, unitCap, order, nodecap, mindist, minnode);

    saltTree.RemoveTopoRedundantSteiner();
}

}  // namespace salt