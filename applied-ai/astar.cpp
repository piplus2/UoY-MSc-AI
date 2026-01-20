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
private:
    int heuristic(int neigh_idx, int T)
    {
    }

public:
    // In this implementation, we define the edge as (neigh_idx, g, h)
    template <typename Heuristic>
    Node *findPath(vector<vector<pair<int, int>>> &adj, int S, int T, Heuristic h)
    {
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
        f[S] = g[S] + h(S, T);

        while (!pq.empty())
        {
            pair<int, int> curr = pq.top();

            int node_cost = curr.first;
            int node_idx = curr.second;

            pq.pop();
            if (visited[node_idx])
                continue; // we skip those rubbish nodes with higher costs

            visited[node_idx] = true;

            if (node_idx == T)
            {
                f[T] = node_cost;
                break;
            }

            for (size_t i = 0; i < adj[node_idx].size(); ++i)
            {
                vector<int> neigh = adj[node_idx][i];
                int neigh_idx = neigh[0];
                int neigh_g = neigh[1];

                if (!visited[neigh_idx])
                {
                    if (g[neigh_idx] > g[node_idx] + neigh_g)
                    {
                        // update with the lower cost path
                        parents[neigh_idx] = node_idx;
                        g[neigh_idx] = g[node_idx] + neigh_g;
                        f[neigh_idx] = g[neigh_idx] + h(neigh_idx, T);
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

/*
the method findPath can be used with the lambda function that
returns the Euclidean distance between the current node and the
target node.
Assuming that we have a structure like:

struct Point {
    int x;
    int y;
};

and a vector coords<Point> which contains the coords of all nodes:

[&coords](int i, int j) { double dx = coords[i].x - coords[j].x;
    double dy = coords[i].y - coords[j].y; return sqrt(dx * dx + dy * dy); }
*/

struct Point
{
    int x;
    int y;
};

int main()
{
    // define 4 points in a grid
    // 0(0,0) -- 1(1,0)
    //   |         |
    // 2(0,1) -- 3(1,1)
    vector<Point> coords = {{0, 0}, {1, 0}, {0, 1}, {1, 1}};

    // adjacency list (neighbour, cost)
    vector<vector<pair<int, int>>> adj(4);
    adj[0] = {{1, 1}, {2, 1}};
    adj[1] = {{0, 1}, {3, 1}};
    adj[2] = {{0, 1}, {3, 1}};
    adj[3] = {{1, 1}, {2, 1}};

    AStar solver;

    // Definiamo l'euristica di Manhattan: |x1 - x2| + |y1 - y2|
    auto manhattan = [&coords](int i, int j)
    {
        return abs(coords[i].x - coords[j].x) + abs(coords[i].y - coords[j].y);
    };

    Node *path = solver.findPath(adj, 0, 3, manhattan);

    // Stampa del percorso
    Node *curr = path;
    cout << "Found path " << endl;
    while (curr)
    {
        cout << "Node " << curr->id << " (Cost g: " << curr->g << ")" << endl;
        curr = curr->next;
    }

    solver.deletePath(path);

    return 0;
}