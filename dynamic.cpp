#include "dynamic.h"
#include <cmath>
#include <random>
#include <algorithm>
#include <queue>

DynamicGraph::DynamicGraph(size_t N) : N(N) {
    SAMPLE_PROBABILITY_THRESHOLD = 11 * log2(N) / sqrt(N);
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_real_distribution<double> distribution(0.0, 1.0);

    A.resize(N);
    D.resize(N);
    As.resize(N);
    Ds.resize(N);
    AA.resize(N);
    edges.resize(N);

    for (int i = 0; i < N; ++i)
    {
        if (distribution(generator) <= SAMPLE_PROBABILITY_THRESHOLD)
        {
            S.insert(i);
            As[i].insert(i);
            Ds[i].insert(i);
        }
        A[i].insert(i);
        D[i].insert(i);
        AA[i].insert(i);
    }
}

void DynamicGraph::update(const std::pair<int, int> &edge) {
    int u = edge.first;
    int v = edge.second;

    edges[u].insert(v);

    for (auto desc : D[v])
    {
        A[desc].insert(A[u].begin(), A[u].end());
        if (S.count(desc))
        {
            for (auto anc : A[u])
            {
                Ds[anc].insert(desc);
            }
        }
    }

    for (auto anc : A[u])
    {
        D[anc].insert(D[v].begin(), D[v].end());
        if (S.count(anc))
        {
            for (auto desc : D[v])
            {
                As[desc].insert(anc);
            }
        }
    }
}

bool DynamicGraph::isSEquivalent(int u, int v) {
    return (As[u].size() == As[v].size()) && (Ds[u].size() == Ds[v].size()) && (A[u].count(v) || A[v].count(u));
}

bool DynamicGraph::hasCycle(const std::pair<int, int> &edge) {
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

bool DynamicGraph::insertEdge(const std::pair<int, int> &edge) {
    update(edge);
    return !hasCycle(edge);
}
