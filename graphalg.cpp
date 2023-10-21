#include <iostream>
#include <unordered_set>
#include <cmath>
#include <random>
#include <vector>
#include <unordered_map>
#include <queue>
#include <chrono>

using namespace std;

class CycleFormedException : public exception {
private:
    pair<unsigned, unsigned> edge;
public:
    CycleFormedException(){
    }

    CycleFormedException(pair<unsigned, unsigned> edge) : edge(edge) {
    }

    const pair<unsigned, unsigned>& getEdge() const {
        return edge;
    }
};

class StaticGraph {
    unsigned N;
    vector<unsigned>* adj;
 
public:
    StaticGraph(unsigned N) : N(N), adj(new vector<unsigned>[N]){
    }
 
    void insertEdge(const pair<unsigned, unsigned>& edge) {
        adj[edge.first].push_back(edge.second);
    }
 
    void topologicalSort() {
        vector<unsigned> in_degree(N, 0);
 
        for (int u = 0; u < N; u++) {
            vector<unsigned>::iterator itr;
            for (itr = adj[u].begin(); itr != adj[u].end(); itr++) {
                in_degree[*itr]++;
            }
        }   
 
        queue<unsigned> q;
        for (unsigned i = 0; i < N; i++) {
            if (in_degree[i] == 0) {
                q.push(i);
            }
        }

        unsigned cnt = 0;
 
        vector<unsigned> topo;
 
        while (!q.empty()) {
            unsigned u = q.front();
            q.pop();
            topo.push_back(u);
            vector<unsigned>::iterator itr;
            for (itr = adj[u].begin(); itr != adj[u].end(); itr++) {
                if (--in_degree[*itr] == 0) {
                    q.push(*itr);
                }
            }
            cnt++;

        }

        if (cnt != N) {
            throw CycleFormedException();
        }
    }
};

class DynamicGraph{

private:
    unsigned N;
    double SAMPLE_PROBABILITY_THRESHOLD;
    unordered_set<unsigned> S;
    vector<unordered_set<unsigned>> A;
    vector<unordered_set<unsigned>> D;
    vector<unordered_set<unsigned>> As;
    vector<unordered_set<unsigned>> Ds;
    vector<unordered_set<unsigned>> AA;
    unordered_map<unsigned, unordered_set<unsigned>> edges;

    void update(const pair<unsigned,unsigned>& edge) {
        edges[edge.first].insert(edge.second);
        unsigned u = edge.first;
        unsigned v = edge.second;
        A[u].insert(u);
        D[v].insert(v);

        for (auto desc : D[v]) {
            for (auto anc : A[u]) {
                    A[desc].insert(anc);
                    if (S.count(desc)) {
                        Ds[anc].insert(desc);
                    }
            }
            
        }

        for (auto anc : A[u]) {
            for (auto desc : D[v]) {
                D[anc].insert(desc);
                if (S.count(anc)) {
                    As[desc].insert(anc);
                }
            }
        }
    }

    bool isSEquivalent(unsigned u, unsigned v) {
        return (As[u].size() == As[v].size()) && (Ds[u].size() == Ds[v].size());
    }

    bool checkCycle(const pair<unsigned, unsigned>& edge) {
        unsigned u = edge.first;
        unsigned v = edge.second;
        unordered_set<unsigned> to_explore;
        to_explore.insert(v);

        while (!to_explore.empty()) {
            unsigned w  = *to_explore.begin();
            to_explore.erase(to_explore.begin());

            if ((w == u) || (AA[u].count(w))){
                return true;
            }
            else if (AA[w].count(u) || !isSEquivalent(u, w)) {
                continue;
            }
            else {
                AA[w].insert(u);
                for (const auto& e : edges[w]) {
                    to_explore.insert(e);
                }
            }
        }
        return false; 
    }

public:
    DynamicGraph(unsigned N) : N(N){
        this->SAMPLE_PROBABILITY_THRESHOLD = 11 * log2(N) / sqrt(N);
        random_device rd;
        default_random_engine generator(rd());
        uniform_real_distribution<double> distribution(0.0, 1.0);
        A.resize(N);
        D.resize(N);
        As.resize(N);
        Ds.resize(N);
        AA.resize(N);

        for (unsigned i = 0; i < N; ++i) {
            if (distribution(generator) <= SAMPLE_PROBABILITY_THRESHOLD) {
                S.insert(i);
                As[i].insert(i);
                Ds[i].insert(i);
            }
            A[i].insert(i);
            D[i].insert(i);
            AA[i].insert(i);
        }
    }

    void insertEdge(const pair<unsigned, unsigned>& edge) {
        update(edge);
        if (checkCycle(edge)) {
            throw CycleFormedException(edge);
        }
    }
};

int main()
{
    unsigned N = 10000;
    vector<pair<unsigned, unsigned>> totalEdgesToInsert;

    for (size_t i = 0; i < N - 3; i++)
    {
        totalEdgesToInsert.push_back(make_pair(i, i + 1));
        totalEdgesToInsert.push_back(make_pair(i, i + 2));
        totalEdgesToInsert.push_back(make_pair(i, i + 3));
    }
    totalEdgesToInsert.push_back(make_pair(N - 1, 0));

    auto start_dynamic = chrono::high_resolution_clock::now();

    DynamicGraph dg = DynamicGraph(N);

    for (auto edge : totalEdgesToInsert) {
        try {
            dg.insertEdge(edge);
        } catch (const CycleFormedException& exception) {
            auto stop_dynamic = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::milliseconds>(stop_dynamic - start_dynamic);
            cout << "Dynamic: Cycle was formed at inserting edge: (" << exception.getEdge().first << ", " << exception.getEdge().second << "). Time taken: " << duration.count() << " ms." << endl;
            break;
        }
    }

    StaticGraph sg = StaticGraph(1);
    vector<pair<unsigned, unsigned>> edgesToInsertStatic;
    auto start_static = chrono::high_resolution_clock::now();
    for (auto edge : totalEdgesToInsert) {
        edgesToInsertStatic.push_back(edge);
        sg = StaticGraph(edgesToInsertStatic.size());
        for (auto edgeToInsert : edgesToInsertStatic) {
            sg.insertEdge(edgeToInsert);
        }
        try {
            sg.topologicalSort();
        } catch (const CycleFormedException& exception) {
            auto stop_static = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::milliseconds>(stop_static - start_static);
            cout << "Static: Cycle was formed at inserting edge: (" << edge.first << ", " << edge.second << "). Time taken: " << duration.count() << " ms." << endl;
            break;
        }
    }
    return 0;
}

