/*
Simple example of Uniform Cost Search using Dijkstra's Algorithm
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
    int cost;
};

class UCS
{
public:
    Node *findPath(vector<vector<pair<int, int>>> &adj, int S, int T)
    {
        if (S == T)
        {
            Node *head = new Node{S, nullptr, 0};
            return head;
        }

        int n = adj.size();
        vector<bool> visited(n, 0);
        vector<int> parents(n, -1);

        vector<int> dist(n, INT_MAX);
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq; // by default it's ordered by first

        pq.push(make_pair(0, S));
        dist[S] = 0;

        while (!pq.empty())
        {
            pair<int, int> curr = pq.top();
            pq.pop();
            if (visited[curr.second])
                continue; // we skip those rubbish nodes with higher costs

            visited[curr.second] = true;

            if (curr.second == T)
            {
                dist[T] = curr.first;
                break;
            }

            for (size_t i = 0; i < adj[curr.second].size(); ++i)
            {
                pair<int, int> neigh = adj[curr.second][i];
                if (!visited[neigh.second])
                {
                    if (dist[neigh.second] > dist[curr.second] + neigh.first)
                    {
                        // update with the lower cost path
                        parents[neigh.second] = curr.second;
                        dist[neigh.second] = dist[curr.second] + neigh.first;
                        neigh.first = dist[neigh.second];
                        pq.push(neigh);
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
            Node *parentNode = new Node{curr, head, dist[curr]};
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
            cout << head->id << ", " << head->cost << "->";
            head = head->next;
        }
        cout << head->id << ", " << head->cost << endl;
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