#pragma once

#include "bonn.h"
#include "salt/base/eval.h"
#include "salt/base/tree.h"
#include "salt/utils/enum.h"

// Note: need to set salt::BonnBuilder::unitCap

BETTER_ENUM(TreeType, int, FLUTE, RSA, ESRSA, MST, BRBC, KRYS, KRY, PD, ES, BONN, SALT, SALT_R);

void GetATree(const salt::Net& net, salt::Tree& tree, TreeType type, double eps, bool checkTree = true);