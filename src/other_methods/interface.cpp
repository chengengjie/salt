#include "interface.h"

#include "salt/base/flute.h"
#include "salt/base/mst.h"
#include "salt/base/rsa.h"
#include "salt/salt.h"

#include "brbc.h"
#include "es.h"
#include "kry.h"
#include "pd.h"

// TODO: replace switch...case... by hash tables
void GetATree(const salt::Net& net, salt::Tree& tree, Method type, double eps, bool checkTree) {
    if (eps < 0) {
        cerr << "Error: invalid epsilon value" << endl;
        return;
    }
    switch (type) {
        case Method::FLUTE: {
            salt::FluteBuilder fluteB;
            fluteB.Run(net, tree);
            break;
        }
        case Method::RSA: {
            salt::RsaBuilder rsaB;
            rsaB.Run(net, tree);
            break;
        }
        case Method::ESRSA: {
            salt::EsRsaBuilder rsaB;
            rsaB.Run(net, tree);
            break;
        }
        case Method::MST: {
            salt::MstBuilder mstB;
            mstB.Run(net, tree);
            break;
        }
        case Method::BRBC: {
            salt::BrbcBuilder brbcB;
            brbcB.Run(net, tree, eps);
            break;
        }
        case Method::KRYS: {
            salt::KrySimBuilder krysB;
            krysB.Run(net, tree, eps);
            break;
        }
        case Method::KRY: {
            salt::KryBuilder kryB;
            kryB.Run(net, tree, eps);
            break;
        }
        case Method::PD: {
            salt::PdBuilder pdB;
            pdB.Run(net, tree, eps);
            break;
        }
        case Method::ES: {
            salt::EsBuilder esB;
            esB.Run(net, tree, eps);
            break;
        }
        case Method::BONN: {
            salt::BonnBuilder bonnB;
            bonnB.Run(net, tree, eps);
            break;
        }
        case Method::SALT_R0:
        case Method::SALT_R1:
        case Method::SALT_R2:
        case Method::SALT_R3: {
            salt::SaltBuilder saltB;
            saltB.Run(net, tree, eps, type._to_string()[6] - '0');
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