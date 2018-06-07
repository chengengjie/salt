#pragma once

#include "brbc.h"
#include "salt/base/rsa.h"

namespace salt {

class EsRsaBuilder : public RsaBase {
public:
    void Run(const Net& net, Tree& tree);
};

// ES algorithm
class EsBuilder : public BrbcBuilder {
public:
    void Run(const Net& net, Tree& tree, double eps) {
        BrbcBuilder::Run(net, tree, eps);
        salt::EsRsaBuilder rsaB;
        rsaB.ReplaceRootChildren(tree);
    }
};

}  // namespace salt