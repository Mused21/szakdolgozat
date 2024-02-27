#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include <random>
#include <numeric>
#include <optional>
#include <algorithm>
#include "dynamic.h"
#include "static.h"
#include "italiano.h"

class DirectedGraphGenerator
{
public:
    static std::vector<std::pair<int, int>> generateGraph(int N, int M, int Gver)
    {
        std::vector<std::pair<int, int>> edges;

        if (Gver == 1)
        {
            std::unordered_set<std::string> existingEdges;
            srand(time(nullptr));

            std::vector<int> array(N);
            std::iota(array.begin(), array.end(), 0);
            std::mt19937 rng(std::random_device{}());
            std::uniform_int_distribution<> dist(0, N - 1);
            shuffle(array.begin(), array.end(), rng);

            for (int i = 0; i < M; ++i)
            {
                int u, v;
                std::string key;
                do
                {
                    u = dist(rng);
                    v = dist(rng);
                    if (u > v)
                    {
                        std::swap(u, v);
                    }
                    key = edgeKey(array[u], array[v]);
                } while (u == v || existingEdges.find(key) != existingEdges.end());

                edges.emplace_back(array[u], array[v]);
                existingEdges.insert(key);
            }

            for (int i = 0; i < M / 100; ++i)
            {
                int u, v;
                std::string key;
                do
                {
                    u = dist(rng);
                    v = dist(rng);
                    if (u < v)
                    {
                        std::swap(u, v);
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
    static std::string edgeKey(int u, int v)
    {
        return std::to_string(u) + "->" + std::to_string(v);
    }
};

template <typename Algorithm>
std::pair<std::optional<std::pair<int, int>>, long long> runAlgorithm(int N, std::vector<std::pair<int, int>> &edges)
{
    Algorithm algorithm(N);
    std::optional<std::pair<int, int>> retEdge = {};
    auto start = std::chrono::high_resolution_clock::now();
    for (auto &edge : edges)
    {
        if (!algorithm.insertEdge(edge))
        {
            retEdge = edge;
            break;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    return {retEdge, std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()};
}

int main()
{
    int N, M, C, Gver;
    std::cout << "Enter the number of nodes (N): ";
    std::cin >> N;
    std::cout << "Enter the total number of edges (M): ";
    std::cin >> M;
    std::cout << "Enter the total number of runs (C): ";
    std::cin >> C;
    std::cout << "Graph generator version (G) [1 - random or 2 - uniform]: ";
    std::cin >> Gver;

    int i = 0;
    long staticSum = 0;
    long dynamicSum = 0;
    long italianoSum = 0;

    while (i < C)
    {
        bool dynamicFoundCycle = false;
        std::pair<int, int> dynamicCycle;
        bool staticFoundCycle = false;
        std::pair<int, int> staticCycle;
        bool italianoFoundCycle = false;
        std::pair<int, int> italianoCycle;

        auto start_generation = std::chrono::high_resolution_clock::now();
        auto edges = DirectedGraphGenerator::generateGraph(N, M, Gver);
        auto stop_generation = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop_generation - start_generation);

        std::cout << "Generation lasted " << duration.count() << " ms." << std::endl;

        auto [italianoOptEdge, italianoTime] = runAlgorithm<ItalianoGraph>(N, edges);
        auto [dynamicOptEdge, dynamicTime] = runAlgorithm<DynamicGraph>(N, edges);
        auto [staticOptEdge, staticTime] = runAlgorithm<StaticGraph>(N, edges);

        dynamicSum += dynamicTime;
        staticSum += staticTime;
        italianoSum += italianoTime;

        if (dynamicOptEdge)
        {
            dynamicFoundCycle = true;
            dynamicCycle = dynamicOptEdge.value();
        }
        if (staticOptEdge)
        {
            staticFoundCycle = true;
            staticCycle = staticOptEdge.value();
        }
        if (italianoOptEdge)
        {
            italianoFoundCycle = true;
            italianoCycle = italianoOptEdge.value();
        }

        if (dynamicFoundCycle && staticFoundCycle && italianoFoundCycle)
        {
            i++;
        }
        if (dynamicOptEdge != staticOptEdge || dynamicOptEdge != italianoOptEdge)
        {
            std::cerr << "Error! These algorithms found cycles in different places."
                      << " Dynamic: " << dynamicOptEdge.value().first << "," << dynamicOptEdge.value().second
                      << " Static: " << staticOptEdge.value().first << "," << staticOptEdge.value().second
                      << " Italiano: " << italianoOptEdge.value().first << "," << italianoOptEdge.value().second
                      << std::endl;
        }

        if (!dynamicFoundCycle && !staticFoundCycle && italianoFoundCycle)
        {
            std::cout << "No cycle found, regenerating graph." << std::endl;
            continue;
        }
    }

    std::cout << "Results: " << std::endl;
    std::cout << "Number of nodes: " << N << std::endl;
    std::cout << "Number of edges: " << M << std::endl;
    std::cout << "Number of runs: " << C << std::endl;
    std::cout << "Graph generation: " << ((Gver == 1) ? "random" : "uniform") << std::endl;
    std::cout << "Italiano graph speed average: " << italianoSum / C << " ms." << std::endl;
    std::cout << "Dynamic graph speed average: " << dynamicSum / C << " ms." << std::endl;
    std::cout << "Static graph speed average: " << staticSum / C << " ms." << std::endl;

    return 0;
}