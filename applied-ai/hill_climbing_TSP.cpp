/*
Simple implementation of the Hill Climbing algorithm in 2D
with the Travelling Salesman example from unit 3.2.2
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

        int local_best_val = INT_MAX; // initialise the curr cost to infinite

        while (iter < maxIter)
        {
            // swap two random cities

            vector<int> sol_idx(n);
            iota(sol_idx.begin(), sol_idx.end(), 0);

            random_device dev;
            mt19937 rng(dev());
            shuffle(begin(sol_idx), end(sol_idx), rng);

            vector<int> test_sol = sol;
            test_sol[sol_idx[0]] = sol[sol_idx[1]];
            test_sol[sol_idx[1]] = sol[sol_idx[0]];

            int local_val = (int)f(test_sol);

            if (local_val - best_val <= eps)
            {
                sol = test_sol;
                best_val = local_val; // update the best cost
            }

            iter++;
        }

        return Solution{
            sol,
            best_val};
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
        for (size_t i = 0; i < path.size() - 1; ++i)
        {
            int j = i + 1;
            if (path[i] > path[j])
            {
                int k = i;
                i = j;
                j = k;
            }
            totDist += distances[path[i]][path[j] - path[i] - 1];
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

    Solution sol = solver.solve(cities, F, 0, 100);
    cout << "Found solution after 100 iters: " << endl;
    for (auto const &x : sol.s)
        cout << x << "->";
    cout << sol.s[0] << endl;
    cout << "Final cost = " << sol.f_val << endl;
}