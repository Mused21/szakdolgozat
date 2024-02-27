#ifndef __ITALIANO_H__
#define __ITALIANO_H__
#include <cstddef>
#include <algorithm>
#include <vector>

class TreeNode {
  public:
    std::vector<TreeNode*> children;
    int value;
    TreeNode(int value = 0): value(value) {}
};

class ItalianoGraph {
  private:
    size_t N;
    std::vector<std::vector<TreeNode*>> index;
    std::vector<TreeNode> treeNodePool;
    size_t nextTreeNode;
    void update(int u, int v);
    bool isDescendant(int u, int v);
    void meld(int x, int j, int u, int v);
  public:
    ItalianoGraph(size_t N);
    bool insertEdge(std::pair<int,int>&);
};

#endif