/*
Simple implementation of Iterative Deepening A*
*/
#include <iostream>
#include <vector>
#include <limits.h>
#include <utility>
#include <algorithm>
#include <unordered_map>
#include <string>

using namespace std;

enum Outcome
{
    EXCEEDED,
    FOUND,
    NOT_FOUND
};

struct searchVal
{
    Outcome state;
    int fVal;
};

template <typename T>
class IDAStar
{
private:
    searchVal search(int node_idx, int g_val, int threshold, int goal_idx,
                     const vector<int> &h, const vector<vector<pair<int, int>>> &adj,
                     vector<int> &parents, vector<char> &visited)
    {

        int f = g_val + h[node_idx];
        if (f > threshold)
            return {EXCEEDED, f};
        if (node_idx == goal_idx)
            return {FOUND, threshold};

        visited[node_idx] = 1;
        int min_exceeded = INT_MAX;

        for (const auto &neigh : adj[node_idx])
        {
            int v = neigh.first;
            int weight = neigh.second;

            if (visited[v])
                continue;

            parents[v] = node_idx;
            searchVal result = search(v, g_val + weight, threshold, goal_idx, h, adj, parents, visited);

            if (result.state == FOUND)
                return result;
            if (result.state == EXCEEDED)
            {
                if (result.fVal < min_exceeded)
                    min_exceeded = result.fVal;
            }
        }

        visited[node_idx] = 0;

        if (min_exceeded != INT_MAX)
            return {EXCEEDED, min_exceeded};

        return {NOT_FOUND, min_exceeded};
    }

public:
    vector<T> findShortestPath(const unordered_map<T, vector<pair<T, int>>> &graph,
                               const unordered_map<T, int> &heuristic,
                               T root, T goal)
    {

        unordered_map<T, int> t_to_idx;
        vector<T> idx_to_t;
        int counter = 0;

        auto get_idx = [&](const T &node)
        {
            if (t_to_idx.find(node) == t_to_idx.end())
            {
                t_to_idx[node] = counter++;
                idx_to_t.push_back(node);
            }
            return t_to_idx[node];
        };

        // 1. Mappatura completa dei nodi
        for (auto const &[node, neighbours] : graph)
        {
            get_idx(node);
            for (auto const &neigh : neighbours)
                get_idx(neigh.first);
        }

        if (t_to_idx.find(root) == t_to_idx.end() || t_to_idx.find(goal) == t_to_idx.end())
            return {};

        int n = counter;
        int root_idx = t_to_idx[root];
        int goal_idx = t_to_idx[goal];

        // 2. Costruzione strutture dati indicizzate
        vector<vector<pair<int, int>>> adj(n);
        vector<int> h(n, 0);

        for (auto const &[node, neighbours] : graph)
        {
            int u = t_to_idx[node];
            for (auto const &neigh : neighbours)
            {
                adj[u].push_back({t_to_idx[neigh.first], neigh.second});
            }
        }

        for (auto const &[node, val] : heuristic)
        {
            if (t_to_idx.count(node))
            {
                h[t_to_idx[node]] = val;
            }
        }

        // 3. Ciclo IDA*
        vector<int> parents(n, -1);
        vector<char> visited(n, 0);
        int threshold = h[root_idx];
        Outcome status = EXCEEDED;

        while (status == EXCEEDED)
        {
            auto res = search(root_idx, 0, threshold, goal_idx, h, adj, parents, visited);
            status = res.state;
            threshold = res.fVal;
            if (threshold == INT_MAX)
                break;
        }

        // 4. Ricostruzione Path
        vector<T> path;
        if (status == FOUND)
        {
            int curr = goal_idx;
            while (curr != -1)
            {
                path.push_back(idx_to_t[curr]);
                curr = parents[curr];
            }
            reverse(path.begin(), path.end());
        }
        return path;
    }
};

int main()
{
    unordered_map<string, vector<pair<string, int>>> city_graph = {
        {"Arad", {{"Zerind", 75}, {"Sibiu", 140}, {"Timisoara", 118}}},
        {"Zerind", {{"Arad", 75}, {"Oradea", 71}}},
        {"Oradea", {{"Zerind", 71}, {"Sibiu", 151}}},
        {"Sibiu", {{"Arad", 140}, {"Oradea", 151}, {"Fagaras", 99}, {"Rimnicu", 80}}},
        {"Timisoara", {{"Arad", 118}, {"Lugoj", 111}}},
        {"Lugoj", {{"Timisoara", 118}, {"Mehadia", 70}}},
        {"Mehadia", {{"Lugoj", 70}, {"Dobreta", 75}}},
        {"Dobreta", {{"Mehadia", 75}, {"Craiova", 120}}},
        {"Craiova", {{"Dobreta", 120}, {"Rimnicu", 146}, {"Pitesti", 138}}},
        {"Rimnicu", {{"Sibiu", 80}, {"Craiova", 146}, {"Pitesti", 97}}},
        {"Fagaras", {{"Sibiu", 99}, {"Bucharest", 211}}},
        {"Pitesti", {{"Rimnicu", 97}, {"Craiova", 138}, {"Bucharest", 101}}},
        {"Bucharest", {{"Fagaras", 211}, {"Pitesti", 101}}}};

    unordered_map<string, int> h_values = {
        {"Arad", 366}, {"Bucharest", 0}, {"Craiova", 160}, {"Dobreta", 242}, {"Fagaras", 176}, {"Lugoj", 244}, {"Mehadia", 241}, {"Oradea", 380}, {"Pitesti", 100}, {"Rimnicu", 193}, {"Sibiu", 253}, {"Timisoara", 329}, {"Zerind", 374}};

    IDAStar<string> solver;
    string start = "Arad", goal = "Bucharest";

    cout << "Searching path from " << start << " to " << goal << "..." << endl;
    vector<string> result = solver.findShortestPath(city_graph, h_values, start, goal);

    if (!result.empty())
    {
        cout << "Found path!" << endl;
        for (size_t i = 0; i < result.size(); ++i)
        {
            cout << result[i] << (i == result.size() - 1 ? "" : " -> ");
        }
        cout << endl;
    }
    else
    {
        cout << "No path found!" << endl;
    }

    return 0;
}