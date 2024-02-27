#include "static.h"
#include <queue>

StaticGraph::StaticGraph(size_t N) : N(N), adj(N), inDegree(N), everSeen(N, false), nodeCounter(0) {}

void StaticGraph::update(const std::pair<int, int> &edge)
{
    adj[edge.first].push_back(edge.second);
    inDegree[edge.second]++;
    if (inDegree[edge.first] == 0)
    {
        zeroDegree.insert(edge.first);
    }
    if (inDegree[edge.second] == 1)
    {
        zeroDegree.erase(edge.second);
    }
    if (!everSeen[edge.first])
    {
        everSeen[edge.first] = true;
        nodeCounter++;
    }
    if (!everSeen[edge.second])
    {
        everSeen[edge.second] = true;
        nodeCounter++;
    }
}

bool StaticGraph::hasCycle()
{
    std::queue<int> q;
    for (int element : zeroDegree)
    {
        q.push(element);
    }
    int count = 0;
    std::vector<int> inDegreeCopy = inDegree;

    while (!q.empty())
    {
        int u = q.front();
        q.pop();

        for (int i = 0; i < adj[u].size(); i++)
        {
            if (--inDegreeCopy[adj[u][i]] == 0)
            {
                q.push(adj[u][i]);
            }
        }
        count++;
    }

    return count != nodeCounter;
}

bool StaticGraph::insertEdge(const std::pair<int, int> &edge)
{
    update(edge);
    return !hasCycle();
}
