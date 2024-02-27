#ifndef __TREENODE_H__
#define __TREENODE_H__
#include <vector>

class TreeNode
{
public:
    std::vector<TreeNode *> children;
    int value;
    TreeNode(int value = 0) : value(value) {}
};

#endif