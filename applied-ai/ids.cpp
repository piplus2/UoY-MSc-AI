/*
Simple implementation of Iterative Deepening Search
*/

#include <vector>
#include <memory>
#include <utility>
#include <iostream>
#include <unordered_map>

using namespace std;

struct Node
{
    int id;
    unique_ptr<Node> pNext;
};

class IDS
{
public:
    unique_ptr<Node> FindShortestPath(vector<vector<int>> &adj, const int src, const int target, const int maxDepth)
    {
        const int n = adj.size();
        vector<int> parents(n, -1);
        // use unordered_map to be faithful with O(d) space complexity
        // however vector<int> is fastest, but occupies more memory
        unordered_map<int, int> visitedAtLimit;
        bool found = false;

        for (size_t limit = 0; limit <= maxDepth; ++limit)
        {
            // reset visitedAtLimit
            visitedAtLimit.clear();
            visitedAtLimit[src] = limit;

            if (DLS(adj, parents, visitedAtLimit, src, target, limit))
            {
                // reconstruct the path using parents
                auto head = make_unique<Node>();
                head->id = target;
                head->pNext = nullptr;

                int nextId = parents[target];
                while (nextId != -1)
                {
                    auto currNode = make_unique<Node>();
                    currNode->id = nextId;
                    // transfer ownership to pNext of currNode
                    currNode->pNext = move(head);
                    head = move(currNode);
                    nextId = parents[nextId];
                }
                return head;
            }
        }

        // if we are here, there is no path
        return nullptr;
    }

    void printPath(const Node *head)
    {
        if (!head)
        {
            cout << "No path found." << endl;
            return;
        }
        const Node *curr = head;
        while (curr->pNext)
        {
            cout << curr->id << "->";
            head = curr->pNext.get(); // get to take the raw pointer
        }
        cout << curr->id << endl;
    }

private:
    bool DLS(const vector<vector<int>> &adj, vector<int> &parents, unordered_map<int, int> &visitedAtLimit, const int src, const int target, const int limit)
    {
        if (src == target)
            return true;
        if (limit <= 0)
            return false;

        for (const int neigh : adj[src])
        {
            auto it = visitedAtLimit.find(neigh);
            bool isBetterBudget = (it == visitedAtLimit.end() || limit > it->second);

            if (isBetterBudget)
            {
                visitedAtLimit[neigh] = limit;

                if (DLS(adj, parents, visitedAtLimit, neigh, target, limit - 1))
                {
                    parents[neigh] = src;
                    return true;
                }
            }
        }
        return false;
    }
};

int main()
{
    vector<vector<int>> adj(6);

    adj[0] = {1, 2};
    adj[1] = {3};
    adj[2] = {1, 4};
    adj[3] = {5};
    adj[4] = {3, 5};
    adj[5] = {};

    IDS solver;
    unique_ptr<Node> path = solver.FindShortestPath(adj, 0, 5, 10);
    solver.printPath(path.get());

    return 0;
}