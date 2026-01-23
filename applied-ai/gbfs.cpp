#include <vector>
#include <queue>
#include <algorithm>
#include <iostream>
#include <utility>

using namespace std;

class GBFS
{
public:
    vector<int> findShortestPath(vector<vector<int>> &graph, vector<int> &heuristic,
                                 int root, int goal, vector<int> &expansion_order)
    {
        // La priority queue deve contenere {valore_euristico, indice_nodo}
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;

        int n = graph.size();
        vector<int> parents(n, -1);
        vector<bool> visited(n, false);

        // Inseriamo la radice con la sua euristica
        pq.push({heuristic[root], root});
        visited[root] = true;

        bool found = false;
        expansion_order.clear();

        while (!pq.empty())
        {
            auto [current_h, u] = pq.top();
            pq.pop();

            expansion_order.push_back(u);

            if (u == goal)
            {
                found = true;
                break;
            }

            for (int v : graph[u])
            {
                if (!visited[v])
                {
                    visited[v] = true;
                    parents[v] = u;
                    // Inseriamo il vicino usando la SUA euristica
                    pq.push({heuristic[v], v});
                }
            }
        }

        vector<int> path;
        if (found)
        {
            int curr = goal;
            while (curr != -1)
            {
                path.push_back(curr);
                curr = parents[curr];
            }
            reverse(path.begin(), path.end());
        }
        return path;
    }
};

int main()
{
    auto c_to_i = [](char x)
    { return std::tolower(x) - 'a'; };
    auto i_to_c = [](int i)
    { return (char)(i + 'A'); };

    // Grafo di test (basato sul tuo input)
    vector<vector<char>> ch_graph = {
        {'B', 'C', 'F', 'G'},      // A
        {'A', 'D'},                // B
        {'A', 'D', 'E'},           // C
        {'B', 'C', 'E', 'K'},      // D
        {'C', 'F', 'I', 'K', 'D'}, // E
        {'A', 'G', 'H', 'I', 'E'}, // F
        {'A', 'F', 'H'},           // G
        {'G', 'F', 'I', 'J', 'K'}, // H
        {'E', 'F', 'H', 'K'},      // I
        {'H', 'K'},                // J
        {'D', 'E', 'I', 'J', 'K'}  // K
    };

    vector<vector<int>> graph(ch_graph.size());
    for (size_t i = 0; i < ch_graph.size(); ++i)
    {
        for (char x : ch_graph[i])
        {
            graph[i].push_back(c_to_i(x));
        }
    }

    // Euristiche verso 'K'
    vector<int> h_vals = {150, 100, 110, 30, 50, 110, 140, 40, 35, 20, 0};
    vector<int> expansion_log;
    GBFS solver;
    vector<int> path = solver.findShortestPath(graph, h_vals, c_to_i('A'), c_to_i('K'),
                                               expansion_log);

    // Stampa dell'ordine di espansione
    cout << "Ordine di espansione (nodi visitati): " << endl;
    for (size_t i = 0; i < expansion_log.size(); ++i)
    {
        cout << i_to_c(expansion_log[i]) << (i == expansion_log.size() - 1 ? "" : " -> ");
    }
    cout << "\n"
         << endl;

    if (!path.empty())
    {
        cout << "Percorso Greedy trovato: ";
        for (size_t i = 0; i < path.size(); ++i)
        {
            cout << i_to_c(path[i]) << (i == path.size() - 1 ? "" : " -> ");
        }
        cout << endl;
    }
    else
    {
        cout << "Nessun percorso trovato." << endl;
    }

    return 0;
}