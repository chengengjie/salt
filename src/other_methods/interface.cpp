#include "interface.h"

#include "salt/base/flute.h"
#include "salt/base/mst.h"
#include "salt/base/rsa.h"
#include "salt/salt.h"

#include "brbc.h"
#include "es.h"
#include "kry.h"
#include "pd.h"

void GetATree(const salt::Net& net, salt::Tree& tree, TreeType type, double eps, bool checkTree) {
    if (eps < 0) {
        cerr << "Error: invalid epsilon value" << endl;
        return;
    }
    switch (type) {
        case TreeType::FLUTE: {
            salt::FluteBuilder fluteB;
            fluteB.Run(net, tree);
            break;
        }
        case TreeType::RSA: {
            salt::RsaBuilder rsaB;
            rsaB.Run(net, tree);
            break;
        }
        case TreeType::ESRSA: {
            salt::EsRsaBuilder rsaB;
            rsaB.Run(net, tree);
            break;
        }
        case TreeType::MST: {
            salt::MstBuilder mstB;
            mstB.Run(net, tree);
            break;
        }
        case TreeType::BRBC: {
            salt::BrbcBuilder brbcB;
            brbcB.Run(net, tree, eps);
            break;
        }
        case TreeType::KRYS: {
            salt::KrySimBuilder krysB;
            krysB.Run(net, tree, eps);
            break;
        }
        case TreeType::KRY: {
            salt::KryBuilder kryB;
            kryB.Run(net, tree, eps);
            break;
        }
        case TreeType::PD: {
            salt::PdBuilder pdB;
            pdB.Run(net, tree, eps);
            break;
        }
        case TreeType::ES: {
            salt::EsBuilder esB;
            esB.Run(net, tree, eps);
            break;
        }
        case TreeType::BONN: {
            salt::BonnBuilder bonnB;
            bonnB.Run(net, tree, eps);
            break;
        }
        case TreeType::SALT:
        case TreeType::SALT_R: {
            salt::SaltBuilder saltB;
            saltB.Run(net, tree, eps, type == +TreeType::SALT_R);
            break;
        }
        default:
            log() << "Error: unkown tree type" << endl;
    }

    if (checkTree) {
        tree.QuickCheck();
        tree.UpdateId();
    }
}