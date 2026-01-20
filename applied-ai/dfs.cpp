/*
Simple example of Deep First Search
*/

#include <vector>
#include <stack>
#include <iostream>

using namespace std;

struct Node
{
    int id;
    Node *next;
};

class DFS
{
public:
    Node *findPath(vector<vector<int>> &adj, int S, int T)
    {
        if (S == T)
        {
            Node *head = new Node{S, nullptr};
            return head;
        }

        stack<int> s;
        int n = adj.size();
        vector<bool> visited(n, 0);

        bool found = false;
        visited[S] = true;
        s.push(S);
        vector<int> parents(n, -1);
        while (!s.empty() && !found)
        {
            int U = s.top();
            s.pop();

            for (int i = (int)adj[U].size() - 1; i >= 0; --i)
            {
                int N = adj[U][i];
                if (!visited[N])
                {
                    visited[N] = true;
                    parents[N] = U;
                    if (N == T)
                    {
                        found = true;
                    }
                    s.push(N);
                }
                if (found)
                    break;
            }
        }

        // now that we have the reversed path from parents
        // we can gen the list of Nodes
        if (parents[T] == -1)
            return nullptr;

        int curr = T;
        Node *head = new Node{curr, nullptr};
        while (curr != S)
        {
            curr = parents[curr];
            Node *parentNode = new Node{curr, head};
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
            cout << head->id << "->";
            head = head->next;
        }
        cout << head->id << endl;
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

int main()
{
    vector<vector<int>> adj = {
        {1, 2},
        {3},
        {3},
        {}};

    DFS dfs;
    Node *head = dfs.findPath(adj, 0, 3);
    dfs.printPath(head);
    dfs.deletePath(head);
}