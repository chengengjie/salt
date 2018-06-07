#pragma once

#include "salt/base/tree.h"

namespace salt {

// Bonn algorithm
class BonnBuilder {
public:
    static double unitCap;
    void Run(const Net &net, Tree &saltTree, double epsilon);
};

}  // namespace salt