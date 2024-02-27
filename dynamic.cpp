#include "dynamic.h"
#include <cmath>
#include <random>
#include <algorithm>
#include <queue>
#include <iostream>

DynamicGraph::DynamicGraph(size_t N) : N(N), nextTreeNode(0)
{
    SAMPLE_PROBABILITY_THRESHOLD = 11 * log2(N) / sqrt(N);
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_real_distribution<double> distribution(0.0, 1.0);

    As.resize(N);
    Ds.resize(N);
    AA.resize(N);
    edges.resize(N);

    index.resize(N);
    treeNodePool.resize(N * N);
    for (int i = 0; i < N; ++i)
    {
        index[i] = std::vector<TreeNode *>(N, nullptr);
        index[i][i] = &(treeNodePool[nextTreeNode++]);
        index[i][i]->value = i;
    }

    for (int i = 0; i < N; ++i)
    {
        if (distribution(generator) <= SAMPLE_PROBABILITY_THRESHOLD)
        {
            S.insert(i);
            As[i].insert(i);
            Ds[i].insert(i);
        }
        AA[i].insert(i);
    }
}

bool DynamicGraph::isDescendant(int u, int v)
{
    return index[u][v] != nullptr;
}

void DynamicGraph::meld(int x, int j, int u, int v)
{
    auto pv = &(treeNodePool[nextTreeNode++]);
    pv->value = v;
    index[x][v] = pv;
    index[x][u]->children.push_back(pv);

    for (auto child : index[j][v]->children)
    {
        if (!isDescendant(x, child->value))
        {
            meld(x, j, v, child->value);
        }
    }

    if (S.count(x))
    {
        Ds[x].insert(v);
    }

    if (S.count(v))
    {
        As[v].insert(x);
    }
}

void DynamicGraph::update(const std::pair<int, int> &edge)
{
    int u = edge.first;
    int v = edge.second;

    edges[u].insert(v);

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

bool DynamicGraph::isSEquivalent(int u, int v)
{
    return (As[u].size() == As[v].size()) && (Ds[u].size() == Ds[v].size()) && (isDescendant(u, v) || isDescendant(v, u));
}

bool DynamicGraph::hasCycle(const std::pair<int, int> &edge)
{
    int u = edge.first;
    int v = edge.second;
    std::queue<int> to_explore;
    to_explore.push(v);
    while (!to_explore.empty())
    {
        int w = to_explore.front();
        to_explore.pop();
        if ((w == u) || (AA[u].count(w)))
        {
            return true;
        }
        else if (AA[w].count(u) || !isSEquivalent(u, w))
        {
            continue;
        }
        else
        {
            AA[w].insert(u);
            for (const auto &e : edges[w])
            {
                to_explore.push(e);
            }
        }
    }
    return false;
}

bool DynamicGraph::insertEdge(const std::pair<int, int> &edge)
{
    update(edge);
    return !hasCycle(edge);
}
