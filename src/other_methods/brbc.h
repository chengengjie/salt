#pragma once

#include "salt/base/tree.h"

namespace salt {

// BRBC/ABP algorithm
class BrbcBuilder {
public:
    void Run(const Net& net, Tree& tree, double eps);
    void ExportPath(const Net& net, Tree& path);

protected:
    virtual inline bool Comp(DTYPE curDist, DTYPE shortestDist, double eps) { return curDist > eps * shortestDist; }
    virtual inline bool Update(DTYPE& curDist, DTYPE shortestDist) { curDist = 0; }
};

// Simplified KRY algorithm
// Difference with BRBC: check distance estimate to source (instead of accumulated distance from last breakpoint)
class KrySimBuilder : public BrbcBuilder {
protected:
    inline bool Comp(DTYPE curDist, DTYPE shortestDist, double eps) { return curDist > (1 + eps) * shortestDist; }
    inline bool Update(DTYPE& curDist, DTYPE shortestDist) { curDist = shortestDist; }
};

}  // namespace salt