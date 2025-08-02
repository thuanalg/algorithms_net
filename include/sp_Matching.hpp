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


inline int
sp_Matching_test()
{
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

*/