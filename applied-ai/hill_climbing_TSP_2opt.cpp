/*
Simple implementation of the Hill Climbing algorithm in 2D
with the Travelling Salesman and application of 2-opt

2-opt: 2 pairs of cities are disconnected and re-connected so that
they don't cross anymore.
*/

#include <vector>
#include <cmath>
#include <random>
#include <algorithm>
#include <iostream>
#include <limits.h>

using namespace std;

struct Solution
{
    vector<int> s;
    int f_val;
    bool converged;
};

class HC
{
public:
    template <typename F>
    Solution solve(const vector<int> start, F f, int eps, int maxIter)
    {
        int n = start.size();
        vector<int> sol = start;
        int iter = 0;
        int best_val = f(sol);
        int noUpdate = 0;

        int local_best_val = INT_MAX; // initialise the curr cost to infinite

        random_device dev;
        mt19937 rng(dev());
        uniform_int_distribution<int> dist(0, start.size() - 1);

        while (iter < maxIter && noUpdate <= (int)maxIter / 3)
        {
            // 2-opt
            int idx1 = dist(rng);
            int idx2 = dist(rng);
            if (idx1 == idx2)
                continue;

            if (idx1 > idx2)
                swap(idx1, idx2);

            vector<int> test_sol = sol;
            reverse(test_sol.begin() + idx1, test_sol.begin() + idx2 + 1);

            int local_val = (int)f(test_sol);

            if (local_val - best_val <= eps)
            {
                sol = test_sol;
                best_val = local_val; // update the best cost
                noUpdate = 0;
            }
            else
            {
                noUpdate++;
            }

            iter++;
        }

        return Solution{
            sol,
            best_val, noUpdate < (int)maxIter / 3};
    }
};

int main()
{
    int numCities = 8;
    vector<vector<int>> distances;
    distances.push_back({1, 3, 2, 2, 3, 5, 2});
    distances.push_back({1, 4, 4, 4, 4, 6});
    distances.push_back({1, 3, 2, 1, 3});
    distances.push_back({1, 4, 3, 5});
    distances.push_back({1, 4, 4});
    distances.push_back({1, 2});
    distances.push_back({1});
    distances.push_back({});

    auto F = [&](const vector<int> &path)
    {
        int totDist = 0;
        int n = path.size();

        for (size_t i = 0; i < n; ++i)
        {
            int u = path[i];
            int v = path[(i + 1) % n];
            if (u == v)
                continue;
            int startNode = min(u, v);
            int endNode = max(u, v);

            totDist += distances[startNode][endNode - startNode - 1];
        }

        return totDist;
    };

    HC solver;
    int num_tests = 5;

    vector<int> cities = {0, 1, 2, 3, 4, 5, 6, 7};
    random_device dev;
    mt19937 rng(dev());

    // start from random path
    shuffle(begin(cities), end(cities), rng);
    cout << "Starting path: " << endl;
    for (auto const &x : cities)
        cout << x << "->";
    cout << cities[0] << endl;
    cout << "Initial cost: " << F(cities) << endl;

    Solution sol = solver.solve(cities, F, 0, 1000);
    cout << "Found solution after 1000 iters: " << endl;
    for (auto const &x : sol.s)
        cout << x << "->";
    cout << sol.s[0] << endl;
    cout << "Final cost = " << sol.f_val << endl;
    cout << (sol.converged ? "Converged" : "Not-converged") << endl;
}