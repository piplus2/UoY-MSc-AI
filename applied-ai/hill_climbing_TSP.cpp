/*
Simple implementation of the Hill Climbing algorithm in 2D
with the Travelling Salesman example from unit 3.2.2
*/

#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>
#include <random>
#include <algorithm>
#include <iostream>
#include <limits.h>

using namespace std;

vector<vector<int>> loadMatrix(string filename, int n)
{
    vector<vector<int>> matrix(n, vector<int>(n));
    ifstream file(filename);
    string line;
    int row = 0;
    while (getline(file, line) && row < n)
    {
        stringstream ss(line);
        string val;
        int col = 0;
        while (getline(ss, val, ',') && col < n)
        {
            matrix[row][col] = stoi(val);
            col++;
        }
        row++;
    }
    return matrix;
}

struct Solution
{
    int cost;
    int iter;
    vector<int> history;
};

class HC
{
public:
    template <typename F>
    Solution solve(vector<int> sol, F f, int maxIter, int patience)
    {
        int n = sol.size();
        int iter = 0;
        int best_val = f(sol);
        int noUpdate = 0;
        vector<int> history;

        random_device dev;
        mt19937 rng(dev());
        uniform_int_distribution<int> dist(0, n - 1);

        while (iter < maxIter && noUpdate <= patience)
        {
            history.push_back(best_val);

            // swap two random cities
            int idx1 = dist(rng);
            int idx2 = (idx1 + 1) % n;

            swap(sol[idx1], sol[idx2]);
            int local_val = (int)f(sol);

            if (local_val < best_val)
            {
                best_val = local_val; // update the best cost
                noUpdate = 0;
            }
            else
            {
                // backtrack
                swap(sol[idx1], sol[idx2]);
                noUpdate++;
            }

            iter++;
        }

        return Solution{best_val, iter, history};
    }
};

int main()
{
    int n = 50;
    string filename = "TSP Matrix.txt";
    auto distances = loadMatrix(filename, n);

    auto F = [&](const vector<int> &path)
    {
        int totDist = 0;
        for (int i = 0; i < n; ++i)
        {
            totDist += distances[path[i]][path[(i + 1) % n]];
        }
        return totDist;
    };

    random_device dev;
    mt19937 rng(dev());

    // Start 10 repeats

    for (int t = 1; t <= 10; ++t)
    {
        vector<int> cities(n);
        iota(cities.begin(), cities.end(), 0);
        // Random initialisation
        shuffle(begin(cities), end(cities), rng);

        int initCost = F(cities);
        HC solver;
        Solution sol = solver.solve(cities, F, 1000, 25);

        ofstream outFile("run_" + to_string(t) + ".csv");
        for (int i = 0; i < sol.history.size(); ++i)
            outFile << i << "," << sol.history[i] << "\n";
        outFile.close();

        cout << "Test " << t << ": Initial cost = " << initCost << " -> Final cost = " << sol.cost << " reached after " << sol.iter << " iters" << endl;
    }

    return 0;
}