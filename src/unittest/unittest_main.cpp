#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include "other_methods/interface.h"
#include "salt/refine/refine.h"

TEST_CASE("Net read/write", "[net_rw]") {
    string netStr =
        "Net 0 read_write_test 4\n"
        "0 90 19\n"
        "1 88 75\n"
        "2 61 98\n"
        "3 64 77\n"
        "Net 1 read_write_cap_test 4 -cap\n"
        "0 90 19 0.2\n"
        "1 88 75 0.2\n"
        "2 61 98 0.4\n"
        "3 64 77 0.1\n";

    istringstream iss(netStr);
    ostringstream oss;
    for (unsigned i = 0; i < 2; ++i) {
        salt::Net net;
        net.Read(iss);
        net.Write(oss);
    }
    REQUIRE(netStr == oss.str());
}

TEST_CASE("Tree read/write", "[tree_rw]") {
    string treeStr =
        "Tree 0 read_write_test 3\n"
        "0 90 19 -1\n"
        "1 61 98 3\n"
        "2 64 77 3\n"
        "3 88 75 0\n"
        "Tree 1 read_write_cap_test 3 -cap\n"
        "0 90 19 -1 0.2\n"
        "1 61 98 3 0.4\n"
        "2 64 77 3 0.1\n"
        "3 88 75 0\n";

    istringstream iss(treeStr);
    ostringstream oss;
    for (unsigned i = 0; i < 2; ++i) {
        salt::Tree tree;
        tree.Read(iss);
        tree.Write(oss);
    }
    REQUIRE(treeStr == oss.str());
}

TEST_CASE("Post refinement by intersection cancelling", "[refine_intersect]") {
    string treeStr1 =  // with same path length, prefer wirelength
        "Tree 0 refine_intersect 5\n"
        "0 0 0 -1\n"
        "1 1 20 0\n"
        "2 20 0 0\n"
        "3 40 30 1\n"
        "4 30 40 2\n";
    string treeStr2 =  // prefer path length (primary)
        "Tree 1 refine_intersect_path_len 6\n"
        "0 0 0 -1\n"
        "1 1 20 5\n"
        "2 20 0 0\n"
        "3 40 30 1\n"
        "4 30 40 2\n"
        "5 -1 1 0\n";
    vector<int> wirelengthGolden = {100, 103};
    vector<int> maxPathLengthGolden = {70, 70};

    int i = 0;
    for (const auto& treeStr : {treeStr1, treeStr2}) {
        istringstream iss(treeStr);
        salt::Tree tree;
        tree.Read(iss);

        salt::Refine::CancelIntersect(tree);

        salt::WireLengthEval eval(tree);
        REQUIRE(eval.wireLength == wirelengthGolden[i]);
        REQUIRE(eval.maxPathLength == maxPathLengthGolden[i]);
        ++i;
    }
}

TEST_CASE("Post refinement by flipping", "[refine_flip]") {
    string treeStr =
        "Tree 0 refine_flip 3\n"
        "0 60 100 -1\n"
        "1 100 0 0\n"
        "2 0 10 1\n";

    istringstream iss(treeStr);
    salt::Tree tree;
    tree.Read(iss);

    salt::Refine::Flip(tree);

    salt::WireLengthEval eval(tree);
    REQUIRE(eval.wireLength == 200);
    REQUIRE(eval.maxPathLength == 150);
}

TEST_CASE("Post refinement by U shifting", "[refine_shift]") {
    string treeStr =
        "Tree 0 refine_shift 4\n"
        "0 100 100 -1\n"
        "1 80 0 0\n"
        "2 20 10 1\n"
        "3 0 50 2\n";

    istringstream iss(treeStr);
    salt::Tree tree;
    tree.Read(iss);

    salt::Refine::Flip(tree);
    salt::Refine::UShift(tree);  // only works after L flipping

    salt::WireLengthEval eval(tree);
    REQUIRE(eval.wireLength == 240);
    REQUIRE(eval.maxPathLength == 170);
}

TEST_CASE("Detailed result of complete SALT and statistics of all", "[complete]") {
    string netStr =
        "Net 0 result_test 11\n"
        "0 90 19\n"
        "1 88 75\n"
        "2 61 98\n"
        "3 64 77\n"
        "4 45 27\n"
        "5 42 79\n"
        "6 49 93\n"
        "7 37 49\n"
        "8 54 73\n"
        "9 67 30\n"
        "10 40 99\n";
    istringstream iss(netStr);
    salt::Net net;
    net.Read(iss);

    SECTION("Detailed result complete of SALT") {
        string saltGoldenStr =
            "Tree 0 result_test 11\n"
            "0 90 19 -1\n"
            "1 88 75 11\n"
            "2 61 98 15\n"
            "3 64 77 12\n"
            "4 45 27 16\n"
            "5 42 79 14\n"
            "6 49 93 14\n"
            "7 37 49 4\n"
            "8 54 73 13\n"
            "9 67 30 16\n"
            "10 40 99 15\n"
            "11 88 27 0\n"
            "12 64 75 1\n"
            "13 54 75 12\n"
            "14 49 79 13\n"
            "15 49 98 6\n"
            "16 67 27 11\n";

        salt::Tree tree;
        GetATree(net, tree, Method::SALT_R3, 1);
        ostringstream oss;
        tree.Write(oss);

        REQUIRE(oss.str() == saltGoldenStr);
    }

    SECTION("Statistics of all") {
        string statGoldenStr =
            "   FLUTE wl=211 mp=158 ap=111.3 ms=2.72414 as=1.32768 md=4.2975e-16 ad=4.29455e-16 mnd=1.0023 and=1.00161\n"
            "     RSA wl=260 mp=130 ap=86.3 ms=1 as=1 md=5.28693e-16 ad=5.28376e-16 mnd=1.23306 and=1.23232\n"
            "     MST wl=235 mp=182 ap=125.3 ms=3.13793 as=1.48981 md=4.78926e-16 ad=4.78542e-16 mnd=1.11699 and=1.11609\n"
            "      PD wl=274 mp=130 ap=89.3 ms=1.11321 as=1.03613 md=5.57039e-16 ad=5.5664e-16 mnd=1.29917 and=1.29824\n"
            "      ES wl=256 mp=146 ap=102.1 ms=2.24138 as=1.20608 md=5.20983e-16 ad=5.20623e-16 mnd=1.21508 and=1.21424\n"
            " SALT_R3 wl=229 mp=132 ap=89.1 ms=1.22222 as=1.02667 md=4.65601e-16 ad=4.65301e-16 mnd=1.08591 and=1.08521\n";

        salt::ElmoreDelayEval::unitRes = 0.0012675;  // Ohm/dbu
        salt::ElmoreDelayEval::unitCap = 8e-20;      // Farad/dbu
        ostringstream oss;
        for (auto type :
             {Method::FLUTE, Method::RSA, Method::MST, Method::PD, Method::ES, Method::SALT_R3}) {
            salt::Tree tree;
            GetATree(net, tree, type, 1);
            salt::WireLengthEval wl(tree);
            salt::ElmoreDelayEval ed(25.35, tree);  // Ohm
            oss << setw(8) << (+type)._to_string() << wl << ed << endl;
        }

        REQUIRE(oss.str() == statGoldenStr);
    }
}