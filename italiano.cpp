#include "italiano.h"
#include <vector>

ItalianoGraph::ItalianoGraph(size_t N) : N(N), nextTreeNode(0)
{
    index.resize(N);
    treeNodePool.resize(N * N);
    for (int i = 0; i < N; ++i)
    {
        index[i] = std::vector<TreeNode *>(N, nullptr);
        index[i][i] = &(treeNodePool[nextTreeNode++]);
        index[i][i]->value = i;
    }
}

void ItalianoGraph::update(int u, int v)
{
    if (isDescendant(u, v))
        return;
    for (int x = 0; x < N; ++x)
    {
        if (!isDescendant(x, u))
            continue;
        if (isDescendant(x, v))
            continue;
        meld(x, v, u, v);
    }
}

bool ItalianoGraph::isDescendant(int u, int v)
{
    return index[u][v] != nullptr;
}

void ItalianoGraph::meld(int x, int j, int u, int v)
{
    auto pv = &(treeNodePool[nextTreeNode++]);
    pv->value = v;
    index[x][v] = pv;
    index[x][u]->children.push_back(pv);
    for (auto pw : index[j][v]->children)
    {
        if (!isDescendant(x, pw->value))
        {
            meld(x, j, v, pw->value);
        }
    }
}

bool ItalianoGraph::insertEdge(std::pair<int, int> &edge)
{
    int u = edge.first;
    int v = edge.second;
    if (isDescendant(v, u))
        return false;
    update(u, v);
    return true;
}
