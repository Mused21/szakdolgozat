#ifndef __STATIC_H__
#define __STATIC_H__

#include <unordered_set>
#include <vector>
#include <algorithm>

class StaticGraph {
private:
    size_t N;
    std::vector<std::vector<int>> adj;
    std::vector<int> inDegree;
    std::unordered_set<int> zeroDegree;
    std::vector<bool> everSeen;
    int nodeCounter;

    void update(const std::pair<int, int> &edge);
    bool hasCycle();

public:
    StaticGraph(size_t N);
    bool insertEdge(const std::pair<int, int> &edge);
};

#endif