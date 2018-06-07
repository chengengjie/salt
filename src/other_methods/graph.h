#pragma once

#include <boost/graph/adjacency_list.hpp>

namespace salt {

using Graph = boost::adjacency_list<boost::vecS,
                                    boost::vecS,
                                    boost::undirectedS,
                                    boost::no_property,
                                    boost::property<boost::edge_weight_t, DTYPE>>;
using Edge = boost::graph_traits<Graph>::edge_descriptor;
using Vertex = boost::graph_traits<Graph>::vertex_descriptor;

}  // namespace salt