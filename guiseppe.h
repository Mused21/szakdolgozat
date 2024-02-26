#ifndef __DESCENDANT_H__
#define __DESCENDANT_H__
#include <cstddef>
#include <algorithm>

class GuiseppeDescendantImpl;

class GuiseppeDescendant {
  private:
    size_t N;
    GuiseppeDescendantImpl *impl;
  public:
    GuiseppeDescendant(size_t N);
    void insert(int u, int v);
    bool is_descendant(int u, int v);
    bool insertEdge(std::pair<int,int>&);
};

#endif