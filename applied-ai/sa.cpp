/*
Simple implementation of Simulated Annealing
*/

#include <vector>
#include <cmath>
#include <random>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <utility>

using namespace std;

struct Point
{
    double X;
    double Y;
};

void saveHistory(const string &filename, const vector<pair<double, double>> &history)
{
    ofstream outFile(filename);
    if (!outFile.is_open())
        return;

    for (auto &[temp, dist] : history)
    {
        outFile << temp << "," << dist << "\n";
    }

    outFile.close();
}

void savePath(const string &filename, vector<int> &path, vector<Point> &coords)
{
    ofstream outFile(filename);
    if (!outFile.is_open())
        return;

    for (int idx : path)
    {
        outFile << coords[idx].X << "," << coords[idx].Y << "\n";
    }

    if (!path.empty())
    {
        outFile << coords[path[0]].X << "," << coords[path[0]].Y << "\n";
    }

    outFile.close();
}

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

struct Solution
{
    vector<int> path;
    vector<pair<double, double>> history;
    double distance = 0.0;
};

class SA
{
private:
    double calcAcceptProb(double deltaE, double temp)
    {
        return deltaE < 0 ? 1.0 : exp(-deltaE) / temp;
    }

public:
    template <typename F>
    Solution solve(vector<int> start, F f, double startT, double endT, double coolFactor)
    {
        int numCities = start.size();

        vector<pair<double, double>> history;
        vector<int> sol = start;
        double bestDist = f(sol);
        vector<int> bestPath = sol;
        double temp = startT;

        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dist(0, numCities - 1);
        uniform_real_distribution<double> distProb(0.0, 1.0);

        while (temp > endT)
        {
            double currE = f(sol);

            // // swap two cities randomly
            // int idx1 = dis(gen);
            // int idx2 = (idx1 + 1) % numCities;

            // swap(sol[idx1], sol[idx2]);

            int idx1 = dist(gen);
            int idx2;
            do
            {
                idx2 = dist(gen);
            } while (idx1 == idx2);

            if (idx1 > idx2)
                swap(idx1, idx2);

            vector<int> test_sol = sol;
            reverse(test_sol.begin() + idx1, test_sol.begin() + idx2 + 1);

            double neighE = f(test_sol);
            double deltaE = neighE - currE;

            // if pass accept and check if it's a best solution
            if (deltaE < 0 || distProb(gen) < exp(-deltaE / temp))
            {
                double currDist = neighE;
                sol = test_sol;
                if (currDist < bestDist)
                {
                    bestDist = currDist;
                    bestPath = test_sol;
                }
            }
            // else
            // {
            //     // swap it back
            //     swap(sol[idx1], sol[idx2]);
            // }

            history.push_back({temp, bestDist});

            temp *= coolFactor;
        }
        return Solution{bestPath, history, bestDist};
    }
};

void printPath(const vector<int> &path)
{
    int n = path.size();
    for (int i = 0; i < n; ++i)
    {
        cout << path[i] + 1 << "->";
    }
    cout << path[0] + 1 << endl;
}

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

    auto F = [&](const vector<int> &path)
    {
        double totDist = 0;
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

    double tmax = 10.0;
    double tmin = 0.0005;
    double coolingRatio = 0.995;
    SA solver;
    Solution sol = solver.solve(cities, F, tmax, tmin, coolingRatio);

    cout << "Found solution: " << endl;
    for (auto const &x : sol.path)
        cout << x << "->";
    cout << sol.path[0] << endl;
    cout << "Final cost = " << sol.distance << endl;

    saveHistory("results/sa_history.csv", sol.history);
    savePath("results/sa_final_path.csv", sol.path, coordsCities);

    return 0;
}