#ifndef __DYNAMIC_H__
#define __DYNAMIC_H__

#include <unordered_set>
#include <vector>
#include <algorithm>

class DynamicGraph {
private:
    size_t N;
    double SAMPLE_PROBABILITY_THRESHOLD;
    std::unordered_set<int> S;
    std::vector<std::unordered_set<int>> A, D, As, Ds, AA, edges;

    void update(const std::pair<int, int> &edge);
    bool isSEquivalent(int u, int v);
    bool hasCycle(const std::pair<int, int> &edge);

public:
    DynamicGraph(size_t N);
    bool insertEdge(const std::pair<int, int> &edge);
};

#endif
