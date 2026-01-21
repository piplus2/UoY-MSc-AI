/*
Simple implementation of Iterative Deepening Search
*/

#include <vector>
#include <memory>
#include <utility>
#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;

template <typename T>
struct Node
{
    T id;
    unique_ptr<Node<T>> pNext;
};

template <typename T>
class IDS
{
public:
    unique_ptr<Node<T>> FindShortestPath(unordered_map<T, vector<T>> &adj,
                                         const T src, const T target, const int maxDepth)
    {
        const int n = adj.size();
        unordered_map<T, T> parents;
        // use unordered_map to be faithful with O(d) space complexity
        // however vector<int> is fastest, but occupies more memory
        unordered_map<T, int> visitedAtLimit;
        bool found = false;

        for (size_t limit = 0; limit <= maxDepth; ++limit)
        {
            // reset visitedAtLimit
            visitedAtLimit.clear();
            visitedAtLimit[src] = limit;

            if (DLS(adj, parents, visitedAtLimit, src, target, limit))
            {
                // reconstruct the path using parents
                auto head = make_unique<Node<T>>();
                head->id = target;
                head->pNext = nullptr;

                T currId = target;
                while (parents.find(currId) != parents.end())
                {
                    T parentId = parents[currId];
                    auto currNode = make_unique<Node<T>>();
                    currNode->id = parentId;
                    // transfer ownership to pNext of currNode
                    currNode->pNext = move(head);
                    head = move(currNode);
                    currId = parentId;

                    if (currId == src)
                        break;
                }
                return head;
            }
        }

        // if we are here, there is no path
        return nullptr;
    }

    void printPath(const Node<T> *head)
    {
        if (!head)
        {
            cout << "No path found." << endl;
            return;
        }
        const Node<T> *curr = head;
        while (curr->pNext)
        {
            cout << curr->id << "->";
            curr = curr->pNext.get(); // get to take the raw pointer
        }
        cout << curr->id << endl;
    }

private:
    bool DLS(const unordered_map<T, vector<T>> &adj, unordered_map<T, T> &parents,
             unordered_map<T, int> &visitedAtLimit, const T curr, const T target, const int limit)
    {
        if (curr == target)
            return true;
        if (limit <= 0)
            return false;

        auto it_adj = adj.find(curr);
        if (it_adj == adj.end())
            return false;

        for (const T &neigh : it_adj->second)
        {
            auto it_vis = visitedAtLimit.find(neigh);
            bool isBetterBudget = (it_vis == visitedAtLimit.end() || limit > it_vis->second);

            if (isBetterBudget)
            {
                visitedAtLimit[neigh] = limit;

                if (DLS(adj, parents, visitedAtLimit, neigh, target, limit - 1))
                {
                    parents[neigh] = curr;
                    return true;
                }
            }
        }
        return false;
    }
};

int main()
{
    unordered_map<string, vector<string>> flights;
    flights["Roma"] = {"Londra", "Parigi"};
    flights["Londra"] = {"New York", "Tokyo"};
    flights["Parigi"] = {"Tokyo"};
    flights["Tokyo"] = {"New York"};

    IDS<string> solver;

    auto path = solver.FindShortestPath(flights, "Roma", "New York", 5);
    solver.printPath(path.get());

    return 0;
}