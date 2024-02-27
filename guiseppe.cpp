#include "guiseppe.h"
#include <vector>

class TreeNode {
  public:
    std::vector<TreeNode*> children;
    int value;
    TreeNode(int value = 0): value(value) {}
};

class GuiseppeDescendantImpl {
  public:
    size_t N;
    std::vector<std::vector<TreeNode*>> index;
    std::vector<TreeNode> treeNodePool;
    size_t nextTreeNode;
  GuiseppeDescendantImpl(size_t N): N(N), nextTreeNode(0) {
    index.resize(N);
    treeNodePool.resize(N * N); // avoiding small heap mallocs
    for (int i = 0; i < N; ++i) {
      index[i] = std::vector<TreeNode*>(N, nullptr);
      index[i][i] = &(treeNodePool[nextTreeNode++]);
      index[i][i]->value = i; // fix that shit of guiseppe et al
    }
  }
  bool is_descendant(int u, int v) {
    return index[u][v] != nullptr;
  }
  void meld(int x, int j, int u, int v) {
    auto pv = &(treeNodePool[nextTreeNode++]);
    pv->value = v;
    index[x][v] = pv;
    index[x][u]->children.push_back(pv);
    for (auto pw: index[j][v]->children) {
      if (index[x][pw->value] == nullptr) {
        meld(x, j, v, pw->value);
      }
    }
  }
  void insert(int i, int j) {
    if (index[i][j] != nullptr) return;
    for (int x = 0; x < N; ++x) {
      if (index[x][i] == nullptr) continue;
      if (index[x][j] != nullptr) continue;
      meld(x, j, i, j);
    }
  }
};

GuiseppeDescendant::GuiseppeDescendant(size_t N): N(N) {
  impl = new GuiseppeDescendantImpl(N);
}

void GuiseppeDescendant::insert(int u, int v) {
  impl->insert(u, v);
}

bool GuiseppeDescendant::is_descendant(int u, int v) {
  return impl->is_descendant(u,v);
}

bool GuiseppeDescendant::insertEdge(std::pair<int,int>& edge) {
  int u = edge.first;
  int v = edge.second;
  if (impl->index[v][u] != nullptr) return false;
  insert(u,v);
  return true;
}
