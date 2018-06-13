#include "salt/salt.h"

#include "salt/base/eval.h"  // for salt::WireLengthEval

int main(int argc, char **argv) {
    printlog("================================================================================");
    printlog("                SALT (Steiner Shallow-Light Tree) - Gengjie CHEN                ");
    printlog("================================================================================");

    // Parse args
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <net_file> <epsilon>" << endl;
        return 1;
    }
    string netFile(argv[1]);
    double eps = atof(argv[2]);
    salt::Net net;
    net.Read(netFile);
    printlog("Run SALT algorithm on net", net.name, "with", net.pins.size(), "pins using epsilon =", eps);

    // Run SALT
    salt::Tree tree;
    salt::SaltBuilder saltB;
    saltB.Run(net, tree, eps);

    // Report
    printlog("Tree topology is as follows:");
    cout << tree;
    salt::WireLengthEval eval(tree);
    printlog("Wire length is", eval.wireLength);
    printlog("Max path length is", eval.maxPathLength);
    printlog("Avg path length is", eval.avgPathLength);
    printlog("Max stretch (shallowness) is", eval.maxStretch);
    printlog("Avg stretch is", eval.avgStretch);
    tree.Write("SALT");

    printlog("================================================================================");
    printlog("                                      Done ...                                  ");
    printlog("================================================================================");

    return 0;
}