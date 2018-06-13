#pragma once

#include "salt/salt.h"

namespace salt {

// KRY algorithm
// Difference with BRBC:
//  1. check distance estimate to source (instead of accumulated distance from last breakpoint)
//  2. work on MST (instead of Hamiltonian path) directly
class KryBuilder : public SaltBase {
public:
    void Run(const Net& net, Tree& tree, double eps);

protected:
    bool Relax(const shared_ptr<TreeNode>& u, const shared_ptr<TreeNode>& v);  // from u to v
    void DFS(const shared_ptr<TreeNode>& mstNode, const shared_ptr<TreeNode>& slNode, double eps);
};

}  // namespace salt