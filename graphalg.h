#ifndef GRAPHALG_H
#define GRAPHALG_H

#include <unordered_set>
#include <vector>
#include <unordered_map>

using namespace std;

class StaticGraph
{
private:
    int N;
    vector<vector<int>> adj;
    vector<int> inDegree;
    unordered_set<int> zeroDegree;
    vector<bool> everSeen;
    int nodeCounter;

    void update(const pair<int, int> &edge);
    bool hasCycle();

public:
    StaticGraph(int N);
    bool insertEdge(const pair<int, int> &edge);
};

class DynamicGraph
{
private:
    int N;
    double SAMPLE_PROBABILITY_THRESHOLD;
    unordered_set<int> S;
    vector<unordered_set<int>> A, D, As, Ds, AA, edges;

    void update(const pair<int, int> &edge);
    bool isSEquivalent(int u, int v);
    bool hasCycle(const pair<int, int> &edge);

public:
    DynamicGraph(int N);
    bool insertEdge(const pair<int, int> &edge);
};

#endif
