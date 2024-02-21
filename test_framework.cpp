#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include <random>
#include <numeric>
#include <algorithm>
#include "graphalg.h"

using namespace std;

class DirectedGraphGenerator {
public:
    static vector<pair<int, int>> generateGraph(int N, int M) {
        vector<pair<int, int>> edges;
        unordered_set<string> existingEdges;
        srand(time(nullptr));

        vector<int> array(N);
        iota(array.begin(), array.end(), 0);
        mt19937 rng(random_device{}());
        uniform_int_distribution<> dist(0, N - 1);
        std::shuffle(array.begin(), array.end(), rng);

        for (int i = 0; i < M; ++i) {
            int u, v;
            string key;
            do {
                u = dist(rng);
                v = dist(rng);
                if (u > v) {
                    swap(u, v);
                }
                key = edgeKey(array[u], array[v]);
            } while (u == v || existingEdges.find(key) != existingEdges.end());

            edges.emplace_back(array[u], array[v]);
            existingEdges.insert(key);
        }

        for (int i = 0; i < M/100; ++i) {
            int u, v;
            string key;
            do {
                u = dist(rng);
                v = dist(rng);
                if (u < v) {
                    swap(u, v);
                }
                key = edgeKey(array[u], array[v]);
            } while (u == v || existingEdges.find(key) != existingEdges.end());

            edges.emplace_back(array[u], array[v]);
            existingEdges.insert(key);
        }
        return edges;
    }

private:
    static string edgeKey(int u, int v) {
        return to_string(u) + "->" + to_string(v);
    }
};


int main() {   
    int N, M, C;
    cout << "Enter the number of nodes (N): ";
    cin >> N;
    cout << "Enter the total number of edges (M): ";
    cin >> M;
    cout << "Enter the total number of runs (C): ";
    cin >> C;

    int i = 0;
    long staticSum = 0;
    long dynamicSum = 0;

    while (i < C) {
        bool dynamicFoundCycle = false;
        pair<int, int> dynamicCycle;
        bool staticFoundCycle = false;
        pair<int, int> staticCycle;

        auto start_generation = chrono::high_resolution_clock::now();
        auto edges = DirectedGraphGenerator::generateGraph(N, M);
        auto stop_generation = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(stop_generation - start_generation);

        cout << "Generation lasted " << duration.count() << " ms." << endl;

        auto start_dynamic = chrono::high_resolution_clock::now();
        DynamicGraph dg = DynamicGraph(N);
        for (auto edge : edges) {
            if (!dg.insertEdge(edge)) {
                auto stop_dynamic = chrono::high_resolution_clock::now();
                auto duration = chrono::duration_cast<chrono::milliseconds>(stop_dynamic - start_dynamic);
                dynamicFoundCycle = true;
                dynamicCycle = edge;
                dynamicSum += duration.count();
                break;
            }
        }

        auto start_static = chrono::high_resolution_clock::now();
        StaticGraph sg = StaticGraph(N);
        for (auto edge : edges) {
            if (!sg.insertEdge(edge)) {
                auto stop_static = chrono::high_resolution_clock::now();
                auto duration = chrono::duration_cast<chrono::milliseconds>(stop_static - start_static);
                staticFoundCycle = true;
                staticCycle = edge;
                staticSum += duration.count();
                break;
            }
        }

        if (dynamicFoundCycle && staticFoundCycle) {
            i++;
        }

        if (!dynamicFoundCycle && !staticFoundCycle) {
            cout << "No cycle found, regenerating graph." << endl;
            continue;
        }

        if (!dynamicFoundCycle && staticFoundCycle) {
            cout << "ERROR: Dynamic graph method did not find the cycle formed at: " << staticCycle.first << "->" << staticCycle.second;
            break;
        }

        if (!dynamicFoundCycle && staticFoundCycle) {
            cout << "ERROR: Static graph method did not find the cycle formed at: " << dynamicCycle.first << "->" << dynamicCycle.second;
            break;
        }
    }

    cout << "Results: " << endl;
    cout << "Number of nodes: " << N << endl;
    cout << "Number of edges: " << M << endl;
    cout << "Number of runs: " << C << endl;
    cout << "Dynamic graph speed average: " << dynamicSum / C << endl;
    cout << "Static graph speed average: " << staticSum / C << endl;

    return 0;
}