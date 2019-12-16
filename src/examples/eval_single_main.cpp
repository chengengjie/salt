#include "other_methods/interface.h"

#include <fstream>
#include <iomanip>

// Constants
// not needed by most tree construction methods (except Bonn's Algorithm)
// for evaluation by Elmore delay
double dbuPerMicron = 2000;
double unitResistance = 0.0012675;  // Ohm/dbu
double unitCapacitance = 8e-20;     // Farad/dbu
double driverResistance = 25.35;    // Ohm
void SetUnitRCSing();

// From argument parsing
string netFile;
int pinNum = 10, seed = 0;
double eps = 1;
bool ParseArgs(int argc, char **argv);

int main(int argc, char **argv) {
    printlog("================================================================================");
    printlog("                SALT (Steiner Shallow-Light Tree) - Gengjie CHEN                ");
    printlog("================================================================================");

    // Prepare
    if (!ParseArgs(argc, argv)) return 1;
    SetUnitRCSing();
    printlog("Epsilon is", eps);

    // Read or create a net
    salt::Net net;
    if (!netFile.empty()) {
        net.Read(netFile);
    } else {
        printlog("Generate a random net (seed =", seed, ", #pins =", pinNum, ")");
        printlog();
        srand(seed);
        net.RanInit(0, pinNum);
        net.Write("");
    }

    vector<Method> methods = {Method::RSA,
                              Method::FLUTE,
                              Method::BRBC,
                              Method::KRY,
                              Method::PD,
                              Method::ES,
                              Method::BONN,
                              Method::SALT_R0,
                              Method::SALT_R1,
                              Method::SALT_R2,
                              Method::SALT_R3};
    printlog("method norWL maxStretch avgStretch maxNorDelay avgNorDelay time");
    for (auto method : methods) {
        salt::Tree tree;
        utils::timer time;
        GetATree(net, tree, method, eps);
        salt::CompleteEval eval(driverResistance, tree);
        log() << setw(8) << method << '\t' << eval.norWL << '\t' << eval.maxStretch << '\t' << eval.avgStretch << '\t'
              << eval.maxNorDelay << '\t' << eval.avgNorDelay << '\t' << time.elapsed() << endl;
        tree.Write(method._to_string());
    }

    printlog("================================================================================");
    printlog("                                      Done ...                                  ");
    printlog("================================================================================");

    return 0;
}

bool ParseArgs(int argc, char **argv) {
    for (int i = 1; i < argc; ++i) {
        if (string(argv[i]) == "-net")
            netFile = string(argv[++i]);
        else if (string(argv[i]) == "-pin")
            pinNum = atoi(argv[++i]);
        else if (string(argv[i]) == "-seed")
            seed = atoi(argv[++i]);
        else if (string(argv[i]) == "-eps")
            eps = atof(argv[++i]);
        else {
            cerr << "Unknown parameter: " << argv[i] << endl;
            cerr << "Usage 1: " << argv[0] << " -net <.net> [-eps <epsilon>]" << endl;
            cerr << "Usage 2: " << argv[0] << " -pin <pin_num> [-seed <rand_seed> -eps <epsilon>]" << endl;
            return false;
        }
    }

    return true;
}

void SetUnitRCSing() {
    printlog("dbu_per_micron :", dbuPerMicron);
    printlog("unit_resistance :", unitResistance, "Ohm/dbu");
    printlog("unit_capacitance :", unitCapacitance, "Farad/dbu");
    printlog("driver_resistance :", driverResistance, "Ohm");
    printlog();

    salt::ElmoreDelayEval::unitRes = unitResistance;
    salt::ElmoreDelayEval::unitCap = unitCapacitance;
    salt::BonnBuilder::unitCap = unitCapacitance;
}
