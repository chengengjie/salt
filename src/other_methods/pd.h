#pragma once

#include "salt/base/tree.h"

namespace salt {

// Prim-Dijstra trade-off
class PdBuilder {
public:
    void Run(const Net& net, Tree& tree, double eps);
};

}  // namespace salt