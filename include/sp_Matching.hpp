#ifndef __sp_Matching__
#define __sp_Matching__

#include <type_traits> // C++11
#include <concepts> // C++20
#include <ranges> // C++20
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <map>
#include <vector>
#include <unordered_map>
#include <format>

using namespace std;

template <typename T> using EqualKcoord = std::pair<int, int>;
template <typename T>

inline int
sp_Matching(
    std::vector<T> &vec, T &k, std::vector<EqualKcoord<int>> &output)
	requires std::is_arithmetic_v<T>
{
	return 0;
}

template <typename T>
inline int
sp_Matching_webgraphviz(std::vector<T> &left, std::vector<T> &right)
{

	return 0;
}

inline int
sp_Matching_test()
{

    std::string buy_nodes = "U0; U1; U2; U3;"; 
    std::string sell_nodes = "V0; V1; V2; V3;"; 
    std::string edges = "\n\tU0 -- V1; \n\tU0 -- V2; \n\tU1 -- V0; \n\tU1 -- V2; \n\tU2 -- V2; \n\tU0 -- V0; \n\tU1 -- V1; \n\tU3 -- V1; \n\tU3 -- V3;"; 
    std::string graph = std::format(R"(graph Bipartite {{
    rankdir=LR;
    node [shape=circle];

    subgraph cluster_U {{
        label = "Buy Orders";
        color=lightblue;
        {}
    }}

    subgraph cluster_V {{
        label = "Sell Orders";
        color=lightpink;
        {}
    }}

    {}
    
}})",
	buy_nodes, sell_nodes, edges);


	return 0;
}
#endif // __sp_Matching__
// http://www.webgraphviz.com/
/*
graph Bipartite {
    rankdir=LR; 
    node [shape=circle];

    subgraph cluster_U {
	    label = "Buy Orders";
	    color=lightblue;
	    U0; U1; U2;
    }

    subgraph cluster_V {
	    label = "Sell Orders";
	    color=lightpink;
	    V0; V1; V2;
    }

    U0 -- V1;
    U0 -- V2;
    U1 -- V0;
    U1 -- V2;
    U2 -- V2;
}

graph Bipartite {
    rankdir=LR;
    node [shape=circle];

    subgraph cluster_U {
	    label = "Buy Orders";
	    color=lightblue;
	    {}
    }

    subgraph cluster_V {
	    label = "Sell Orders";
	    color=lightpink;
	    {}
    }

    U0 -- V1;
    U0 -- V2;
    U1 -- V0;
    U1 -- V2;
    U2 -- V2;
}


*/