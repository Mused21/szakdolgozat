#ifndef __DYNAMIC_H__
#define __DYNAMIC_H__

#include <unordered_set>
#include <vector>
#include <algorithm>
#include "treenode.h"

class DynamicGraph
{
private:
    size_t N;
    double SAMPLE_PROBABILITY_THRESHOLD;
    std::unordered_set<int> S;
    std::vector<std::unordered_set<int>> As, Ds, AA, edges;

    std::vector<std::vector<TreeNode *>> index;
    std::vector<TreeNode> treeNodePool;
    size_t nextTreeNode;

    bool isDescendant(int u, int v);
    void meld(int x, int j, int u, int v);
    void update(const std::pair<int, int> &edge);
    bool isSEquivalent(int u, int v);
    bool hasCycle(const std::pair<int, int> &edge);
    void updateAs(int v, int s);

public:
    DynamicGraph(size_t N);
    bool insertEdge(const std::pair<int, int> &edge);
};

#endif
