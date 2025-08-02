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
#include <set>
#include <utility>

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
sp_Matching_webgraphviz(vector<pair<int, int>> &edges, int u, int v)
{
	std::string buy_nodes = "U0; U1; U2; U3;";
	std::string sell_nodes = "V0; V1; V2; V3;";
	std::string edges_str = "\n\tU0 -- V1; \n\tU0 -- V2;";
	buy_nodes.clear();
	sell_nodes.clear();
	edges_str.clear();
	for (int i = 0; i < u; ++i) {
		char tmp[100];
		snprintf(tmp, 100, "U%d; ", i);
		buy_nodes += tmp;
	}
	for (int i = 0; i < v; ++i) {
		char tmp[100];
		snprintf(tmp, 100, "V%d; ", i);
		sell_nodes += tmp;
	}
	for (auto &[u, v] : edges) {
		char tmp[100];
		snprintf(tmp, 100, "\n\tU%d -- V%d;", u, v);
		edges_str += tmp;
	}
	std::string graph = std::format(R"(graph Bipartite {{
    rankdir=LR;ranksep=6.0;
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
	    buy_nodes, sell_nodes, edges_str);
	std::string str = graph;
	return 0;
}

inline int
sp_Matching_test()
{
	srand(time(NULL));

	int U = 12; // Number of Buying Orders
	int V = 12; // Number of Selling Orders
	int E = 30; // Number of generating edges.

	vector<pair<int, int>> edges;

	set<pair<int, int>> used; // Avoid duplicating

	while ((int)edges.size() < E) {
		int u = rand() % U;
		int v = rand() % V;
		if (!used.count({u, v})) {
			edges.push_back({u, v});
			used.insert({u, v});
		}
	}
	sp_Matching_webgraphviz(edges, U, V);
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