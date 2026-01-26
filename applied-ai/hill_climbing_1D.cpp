/*
Simple implementation of the Hill Climbing algorithm in 1D
*/

#include <vector>
#include <cmath>
#include <random>
#include <iostream>

using namespace std;

struct Solution
{
    int s;
    int f_val;
    bool converged;
};

class HC1D
{
public:
    template <typename F>
    Solution solve(int start, F f, int radius, int eps, int maxIter)
    {
        // start from a random position
        int sol = start;
        int iter = 0;
        int best_val = f(sol);
        bool found = true;

        while (found && iter < maxIter)
        {
            found = false;
            int local_best_val = best_val;
            int cand_sol = sol;

            for (int n = sol - radius; n <= sol + radius; ++n)
            {
                if ((int)f(n) - local_best_val >= eps)
                {
                    local_best_val = (int)f(n);
                    cand_sol = n;
                    found = true;
                }
            }
            if (found)
            {
                sol = cand_sol;
                best_val = local_best_val;
            }
            iter++;
        }

        return Solution{sol, best_val, found};
    }
};

int main()
{
    auto F = [](int x)
    {
        return -(x * x) + 10 * cos(2 * x);
    };

    HC1D solver;
    int num_tests = 5;

    random_device dev;
    mt19937 rng(dev());
    uniform_int_distribution<mt19937::result_type> dist(-100, 100);
    for (size_t i = 0; i < num_tests; ++i)
    {
        int s0 = dist(rng);
        cout << "Starting from: " << s0 << endl;
        Solution sol = solver.solve(s0, F, 2, 0, 100);
        cout << "Found solution: " << endl;
        cout << "x = " << sol.s << ", f(x) = " << sol.f_val << (sol.converged ? ", converged" : ", not-converged") << endl;
    }
}