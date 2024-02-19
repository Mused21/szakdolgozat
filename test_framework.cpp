#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include "graphalg.h"

using namespace std;

class DirectedGraphGenerator {
public:
    static vector<pair<int, int>> generateGraph(int N, int M) {
        vector<pair<int, int>> edges;
        unordered_set<string> existingEdges;
        srand(time(nullptr));

        for (int i = 0; i < M - 1; ++i) {
            int u, v;
            do {
                u = rand() % N;
                v = rand() % N; 
            } while (u >= v || existingEdges.find(edgeKey(u, v)) != existingEdges.end());

            edges.emplace_back(u, v);
            existingEdges.insert(edgeKey(u, v));
        }
        // ez igy sokszor eredmenyez aciklikus grafot tbh
        /*if (M > N - 1) {
            int u, v;
            do {
                u = rand() % (N - 1) + 1;
                v = rand() % u;
            } while (existingEdges.find(edgeKey(u, v)) != existingEdges.end());

            edges.emplace_back(u, v);
            existingEdges.insert(edgeKey(u, v));
        }
        */
        if (M > N - 1 && !edges.empty()) {
            int randomIndex = rand() % edges.size();
            auto& selectedEdge = edges[randomIndex];
        
            int u = selectedEdge.second;
            int v = selectedEdge.first;

            edges.emplace_back(u, v);
            existingEdges.insert(edgeKey(u, v));
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