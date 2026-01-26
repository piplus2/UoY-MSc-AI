/*
Simple implementation of the Hill Climbing algorithm in 2D
with the Travelling Salesman example from unit 3.2.2 with
deterministic approach
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
        bool improved = true;

        while (iter < maxIter && improved)
        {
            improved = false;
            int best_local_val = best_val;
            vector<int> best_test_sol = sol;

            for (int i = 0; i < n; ++i)
            {
                for (int j = i + 1; j < n; ++j)
                {
                    vector<int> test_sol = sol;
                    swap(test_sol[i], test_sol[j]);

                    int curr_val = f(test_sol);

                    if (curr_val - best_local_val < eps)
                    {
                        best_local_val = curr_val;
                        best_test_sol = test_sol;
                        improved = true;
                    }
                }
            }

            if (improved)
            {
                sol = best_test_sol;
                best_val = best_local_val;
            }

            iter++;
        }

        return Solution{
            sol,
            best_val, improved};
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