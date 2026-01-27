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
#include <sstream>
#include <fstream>
#include <limits.h>

using namespace std;

struct Point
{
    double X;
    double Y;
};

template <typename T>
struct Solution
{
    vector<int> s;
    T f_val;
    bool converged;
};

vector<Point> loadCoords(string filename)
{
    vector<Point> matrix;
    ifstream file(filename);
    string line;
    vector<double> coords;

    while (getline(file, line))
    {
        coords.clear();
        string word;
        istringstream iss(line);
        int col = 0;
        while (col < 2)
        {
            getline(iss, word, ',');
            coords.push_back(stod(word));
            col++;
        }
        if (coords.size() == 2)
        {
            matrix.push_back({coords[0], coords[1]});
        }
    }

    return matrix;
}

template <typename Distance>
vector<vector<double>> calcDist(const vector<Point> &coords, Distance d)
{
    int n = coords.size();
    vector<vector<double>> distMat;
    for (size_t i = 0; i < n; ++i)
    {
        vector<double> citydist;
        for (size_t j = i + 1; j < n; ++j)
        {
            citydist.push_back(d(coords[i], coords[j]));
        }
        distMat.push_back(citydist);
    }

    return distMat;
}

void savePath(const string &filename, const vector<int> &path, const vector<Point> &coords)
{
    ofstream outFile(filename);
    if (!outFile.is_open())
        return;

    // Salviamo le coordinate nell'ordine del percorso
    for (int idx : path)
    {
        outFile << coords[idx].X << "," << coords[idx].Y << "\n";
    }

    // Per chiudere il loop nel plot, torniamo alla prima città
    if (!path.empty())
    {
        outFile << coords[path[0]].X << "," << coords[path[0]].Y << "\n";
    }

    outFile.close();
}

class HC
{
public:
    template <typename D, typename T>
    Solution<double> solve(const vector<int> start, D f, T eps, int maxIter)
    {
        int n = start.size();
        vector<int> sol = start;
        int iter = 0;
        T best_val = f(sol);
        bool improved = true;

        while (iter < maxIter && improved)
        {
            improved = false;
            T best_local_val = best_val;
            vector<int> best_test_sol = sol;

            for (int i = 0; i < n; ++i)
            {
                for (int j = i + 1; j < n; ++j)
                {
                    swap(sol[i], sol[j]);

                    T curr_val = f(sol);

                    if (curr_val < best_local_val - eps)
                    {
                        best_local_val = curr_val;
                        best_test_sol = sol;
                        improved = true;
                    }

                    // restore old solution
                    swap(sol[i], sol[j]);
                }
            }

            if (improved)
            {
                sol = best_test_sol;
                best_val = best_local_val;
            }

            iter++;
        }

        return Solution<double>{
            sol,
            best_val, !improved || iter >= maxIter};
    }
};

int main()
{
    string filename = "data/TSP Matrix.csv";
    vector<Point> coordsCities = loadCoords(filename);
    int nCities = coordsCities.size();

    auto euclideanDistance = [](Point x, Point y)
    {
        return sqrt(pow((x.X - y.X), 2) + pow((x.Y - y.Y), 2));
    };

    vector<vector<double>>
        distMat = calcDist(coordsCities, euclideanDistance);

    auto F = [&](const vector<int> &path) -> double
    {
        double totDist = 0.0;
        int n = path.size();

        for (size_t i = 0; i < n; ++i)
        {
            int u = path[i];
            int v = path[(i + 1) % n];
            if (u == v)
                continue;
            int startNode = min(u, v);
            int endNode = max(u, v);

            totDist += distMat[startNode][endNode - startNode - 1];
        }

        return totDist;
    };

    HC solver;
    int num_tests = 5;

    double bestCost = 99999999999.9;

    for (size_t t = 0; t < num_tests; ++t)
    {
        // Initialise the vector of cities IDs
        vector<int> cities(nCities);
        iota(cities.begin(), cities.end(), 0);

        random_device dev;
        mt19937 rng(dev());

        // start from random path
        shuffle(begin(cities), end(cities), rng);
        cout << "Starting path: " << endl;
        for (auto const &x : cities)
            cout << x << "->";
        cout << cities[0] << endl;
        cout << "Initial cost: " << F(cities) << endl;

        Solution sol = solver.solve(cities, F, 0.0, 5000);
        cout << "Found solution after 1000 iters: " << endl;
        for (auto const &x : sol.s)
            cout << x << "->";
        cout << sol.s[0] << endl;
        cout << "Final cost = " << sol.f_val << endl;
        cout << (sol.converged ? "Converged" : "Not-converged") << endl;

        if (sol.f_val < bestCost)
        {
            bestCost = sol.f_val;
            // write the initial solution
            savePath("results/path_initial.csv", cities, coordsCities);
            // write the found solution
            savePath("results/path_final.csv", sol.s, coordsCities);
        }
    }
}