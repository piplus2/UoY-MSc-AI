/*
Simple example of A* search

A* is similar to a uniform cost search with the only difference
that we have a cost function g and a heurisitc function h which
define the total cost function f = g + h
*/

#include <vector>
#include <utility>
#include <queue>
#include <iostream>
#include <limits.h>

using namespace std;

struct Node
{
    int id;
    Node *next;
    int f;
    int g;
};

class AStar
{

public:
    // In this implementation, we define the edge as (neigh_idx, g, h)
    Node *findPath(vector<vector<pair<int, int>>> &adj, int S, int T, vector<int> h,
                   vector<int> &expansion_log)
    {
        expansion_log.clear(); // Puliamo il log all'inizio
        if (S == T)
        {
            Node *head = new Node{S, nullptr, 0};
            return head;
        }

        int n = adj.size();
        vector<bool> visited(n, 0);
        vector<int> parents(n, -1);

        vector<int> f(n, INT_MAX);
        vector<int> g(n, INT_MAX);
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq; // by default it's ordered by first

        pq.push(make_pair(0, S));
        g[S] = 0;
        f[S] = g[S] + h[S];

        while (!pq.empty())
        {
            pair<int, int> curr = pq.top();

            int node_cost = curr.first;
            int node_idx = curr.second;

            pq.pop();
            if (visited[node_idx])
                continue; // we skip those rubbish nodes with higher costs

            expansion_log.push_back(node_idx);

            visited[node_idx] = true;

            if (node_idx == T)
            {
                f[T] = node_cost;
                break;
            }

            for (size_t i = 0; i < adj[node_idx].size(); ++i)
            {
                int neigh_idx = adj[node_idx][i].first;
                int neigh_g = adj[node_idx][i].second;

                if (!visited[neigh_idx])
                {
                    if (g[neigh_idx] > g[node_idx] + neigh_g)
                    {
                        // update with the lower cost path
                        parents[neigh_idx] = node_idx;
                        g[neigh_idx] = g[node_idx] + neigh_g;
                        f[neigh_idx] = g[neigh_idx] + h[neigh_idx];
                        pair<int, int> newNeigh{f[neigh_idx], neigh_idx};
                        pq.push(newNeigh);
                    }
                }
            }
        }

        if (parents[T] == -1)
            return nullptr;

        int curr = T;
        Node *head = new Node{curr, nullptr};
        while (curr != S)
        {
            curr = parents[curr];
            Node *parentNode = new Node{curr, head, f[curr], g[curr]};
            head = parentNode;
        }
        return head;
    }

    void printPath(Node *head)
    {
        if (head == nullptr)
            return;

        while (head->next != nullptr)
        {
            cout << head->id << ", " << head->g << "->";
            head = head->next;
        }
        cout << head->id << ", " << head->g << endl;
    }

    void deletePath(Node *head)
    {
        if (head == nullptr)
            return;

        Node *curr = head;
        while (curr->next != nullptr)
        {
            Node *nextNode = curr->next;
            delete curr;
            curr = nextNode;
        }
        delete curr;
    }
};

// Per la stampa nel main, puoi rendere l'output più leggibile
void printPathConLettere(Node *head)
{
    auto i_to_c = [](int i)
    { return (char)(i + 'A'); };
    if (!head)
    {
        cout << "Nessun percorso trovato!" << endl;
        return;
    }
    while (head)
    {
        cout << i_to_c(head->id) << " (g=" << head->g << ")";
        if (head->next)
            cout << " -> ";
        head = head->next;
    }
    cout << endl;
}

int main()
{
    auto c_to_i = [](char x)
    { return std::tolower(x) - 'a'; };
    auto i_to_c = [](int i)
    { return (char)(i + 'A'); };

    // Grafo di test (basato sul tuo input)
    vector<vector<pair<char, int>>> ch_graph = {
        {{'B', 70}, {'C', 35}, {'F', 25}, {'G', 50}},            // A
        {{'A', 70}, {'D', 30}},                                  // B
        {{'A', 35}, {'D', 40}, {'E', 35}},                       // C
        {{'B', 30}, {'C', 40}, {'E', 35}, {'K', 30}},            // D
        {{'C', 15}, {'F', 25}, {'I', 15}, {'K', 50}, {'D', 35}}, // E
        {{'A', 25}, {'G', 20}, {'H', 20}, {'I', 35}, {'E', 25}}, // F
        {{'A', 50}, {'F', 20}, {'H', 35}},                       // G
        {{'G', 35}, {'F', 20}, {'I', 10}, {'J', 35}, {'K', 40}}, // H
        {{'E', 15}, {'F', 35}, {'H', 10}, {'K', 35}},            // I
        {{'H', 35}, {'K', 20}},                                  // J
        {{'D', 30}, {'E', 50}, {'I', 35}, {'J', 20}}             // K
    };

    vector<vector<pair<int, int>>> graph(ch_graph.size());
    for (size_t i = 0; i < ch_graph.size(); ++i)
    {
        for (auto const &[c, w] : ch_graph[i])
        {
            graph[i].push_back({c_to_i(c), w});
        }
    }

    // Euristiche verso 'K'
    vector<int> h_vals = {150, 100, 110, 30, 50, 110, 140, 40, 35, 20, 0};
    vector<int> expansion_log;
    AStar solver;

    Node *path = solver.findPath(graph, c_to_i('A'), c_to_i('K'), h_vals, expansion_log);

    cout << "Sequenza di nodi esplorati (Expansion Order):" << endl;
    for (size_t i = 0; i < expansion_log.size(); ++i)
    {
        cout << i_to_c(expansion_log[i]) << (i == expansion_log.size() - 1 ? "" : " -> ");
    }
    cout << "\n"
         << endl;

    // Stampa del percorso
    printPathConLettere(path);

    solver.deletePath(path);

    return 0;
}