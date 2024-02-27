#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include <random>
#include <numeric>
#include <optional>
#include <algorithm>
#include "graphalg.h"
#include "guiseppe.h"

using namespace std;

class DirectedGraphGenerator
{
public:
    static vector<pair<int, int>> generateGraph(int N, int M, int Gver)
    {
        vector<pair<int, int>> edges;

        if (Gver == 1)
        {
            unordered_set<string> existingEdges;
            srand(time(nullptr));

            vector<int> array(N);
            iota(array.begin(), array.end(), 0);
            mt19937 rng(random_device{}());
            uniform_int_distribution<> dist(0, N - 1);
            shuffle(array.begin(), array.end(), rng);

            for (int i = 0; i < M; ++i)
            {
                int u, v;
                string key;
                do
                {
                    u = dist(rng);
                    v = dist(rng);
                    if (u > v)
                    {
                        swap(u, v);
                    }
                    key = edgeKey(array[u], array[v]);
                } while (u == v || existingEdges.find(key) != existingEdges.end());

                edges.emplace_back(array[u], array[v]);
                existingEdges.insert(key);
            }

            for (int i = 0; i < M / 100; ++i)
            {
                int u, v;
                string key;
                do
                {
                    u = dist(rng);
                    v = dist(rng);
                    if (u < v)
                    {
                        swap(u, v);
                    }
                    key = edgeKey(array[u], array[v]);
                } while (u == v || existingEdges.find(key) != existingEdges.end());

                edges.emplace_back(array[u], array[v]);
                existingEdges.insert(key);
            }
        }
        else
        {
            for (size_t i = 0; i < N; i++)
            {
                for (size_t j = 1; j <= M / N; j++)
                {
                    if (i + j < N)
                    {
                        edges.emplace_back(i, i + j);
                    }
                }
            }
            edges.emplace_back(N - 1, 0);
        }
        return edges;
    }

private:
    static string edgeKey(int u, int v)
    {
        return to_string(u) + "->" + to_string(v);
    }
};

template <typename Algo> std::pair<std::optional<std::pair<int, int>>, long long> runAlgorithm(int N, std::vector<std::pair<int,int>>& edges) {
  Algo algo(N);
  std::optional<std::pair<int,int>> retEdge = {};
  auto start = chrono::high_resolution_clock::now();
  for (auto& edge: edges) {
    if (!algo.insertEdge(edge)) {
      retEdge = edge;
      break;
    }
  }
  auto end = chrono::high_resolution_clock::now();
  return {retEdge, chrono::duration_cast<chrono::milliseconds>(end - start).count()};
}

int main()
{
    int N, M, C, Gver;
    cout << "Enter the number of nodes (N): ";
    cin >> N;
    cout << "Enter the total number of edges (M): ";
    cin >> M;
    cout << "Enter the total number of runs (C): ";
    cin >> C;
    cout << "Graph generator version (G) [1 - random or 2 - uniform]: ";
    cin >> Gver;

    int i = 0;
    long staticSum = 0;
    long dynamicSum = 0;
    long guiseppeSum = 0;

    while (i < C)
    {
        bool dynamicFoundCycle = false;
        pair<int, int> dynamicCycle;
        bool staticFoundCycle = false;
        pair<int, int> staticCycle;
        bool guiseppeFoundCycle = false;
        std::pair<int, int> guiseppeCycle;

        auto start_generation = chrono::high_resolution_clock::now();
        auto edges = DirectedGraphGenerator::generateGraph(N, M, Gver);
        auto stop_generation = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(stop_generation - start_generation);

        cout << "Generation lasted " << duration.count() << " ms." << endl;

        auto [dynamicOptEdge, dynamicTime] = runAlgorithm<DynamicGraph>(N, edges);
        auto [staticOptEdge, staticTime] = runAlgorithm<StaticGraph>(N, edges);
        auto [guiseppeOptEdge, guiseppeTime] = runAlgorithm<GuiseppeDescendant>(N, edges);
        dynamicSum += dynamicTime;
        staticSum += staticTime;
        guiseppeSum += guiseppeTime;
        if (dynamicOptEdge) { dynamicFoundCycle = true; dynamicCycle = dynamicOptEdge.value(); }
        if (staticOptEdge) { staticFoundCycle = true; staticCycle = staticOptEdge.value(); }
        if (guiseppeOptEdge) { guiseppeFoundCycle = true; guiseppeCycle = guiseppeOptEdge.value(); }

        if (dynamicFoundCycle && staticFoundCycle && guiseppeFoundCycle)
        {
            i++;
        } else {
          std::cout << "error! static: " << staticFoundCycle << " dynamic: " << dynamicFoundCycle << " guiseppe: " << guiseppeFoundCycle << std::endl;
        }
        if (dynamicOptEdge != staticOptEdge || dynamicOptEdge != guiseppeOptEdge) {
          std::cerr << "error! these guys found cycles in different places."
                    << " dynamic: " << dynamicOptEdge.value().first << "," << dynamicOptEdge.value().second
                    << " static: " << staticOptEdge.value().first << "," << staticOptEdge.value().second
                    << " guiseppe: " << guiseppeOptEdge.value().first << "," << guiseppeOptEdge.value().second
                    << std::endl;
        }

        if (!dynamicFoundCycle && !staticFoundCycle)
        {
            cout << "No cycle found, regenerating graph." << endl;
            continue;
        }

        if (!dynamicFoundCycle && staticFoundCycle)
        {
            cout << "ERROR: Dynamic graph method did not find the cycle formed at: " << staticCycle.first << "->" << staticCycle.second;
            break;
        }

        if (!dynamicFoundCycle && staticFoundCycle)
        {
            cout << "ERROR: Static graph method did not find the cycle formed at: " << dynamicCycle.first << "->" << dynamicCycle.second;
            break;
        }
    }

    cout << "Results: " << endl;
    cout << "Number of nodes: " << N << endl;
    cout << "Number of edges: " << M << endl;
    cout << "Number of runs: " << C << endl;
    cout << "Graph generation: " << ((Gver == 1) ? "random" : "uniform") << endl;
    cout << "Dynamic graph speed average: " << dynamicSum / C << " ms." << endl;
    cout << "Static graph speed average: " << staticSum / C << " ms." << endl;
    std::cout << "Guiseppe graph speed average: " << guiseppeSum / C << " ms." << std::endl;

    return 0;
}