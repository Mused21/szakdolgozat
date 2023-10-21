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
    pair<int, int> edge;
public:
    CycleFormedException(){
    }

    CycleFormedException(pair<int, int> edge) : edge(edge) {
    }

    const pair<int, int>& getEdge() const {
        return edge;
    }
};

class StaticGraph {
    int N;
    vector<vector<int>> adj;
    vector<int> inDegree;
 
public:
    StaticGraph(int N) : N(N), adj(N), inDegree(N){
    }
 
    void insertEdge(const pair<int, int>& edge) {
        adj[edge.first].push_back(edge.second);
        inDegree[edge.second]++;
    }
 
    void topologicalSort() { 
        queue<int> q;
        for (int i = 0; i < inDegree.size(); i++) {
            if (inDegree[i] == 0) {
                q.push(i);
            }
        }

        int count = 0;
 
        while (!q.empty()) {
            int u = q.front();
            q.pop();

            for(int i = 0; i < adj[u].size(); i++) {
                if (--inDegree[adj[u][i]] == 0) {
                    q.push(adj[u][i]);
                }
            }
            count++;

        }

        if (count != N) {
            throw CycleFormedException();
        }
    }
};

class DynamicGraph{

private:
    int N;
    double SAMPLE_PROBABILITY_THRESHOLD;
    unordered_set<int> S;
    vector<unordered_set<int>> A;
    vector<unordered_set<int>> D;
    vector<unordered_set<int>> As;
    vector<unordered_set<int>> Ds;
    vector<unordered_set<int>> AA;
    unordered_map<int, unordered_set<int>> edges;

    void update(const pair<int,int>& edge) {
        int u = edge.first;
        int v = edge.second;

        edges[u].insert(v);

        A[u].insert(u);
        D[v].insert(v);

        // ez n^2 itt van elrontva az egesz szerintem, de azt mondjak O(M) idoben megoldhato egy 1986-os cikk alapjan :)

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

    bool isSEquivalent(int u, int v) {
        return (As[u].size() == As[v].size()) && (Ds[u].size() == Ds[v].size());
    }

    bool checkCycle(const pair<int, int>& edge) {
        int u = edge.first;
        int v = edge.second;
        unordered_set<int> to_explore;
        to_explore.insert(v);

        while (!to_explore.empty()) {
            int w  = *to_explore.begin();
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
    DynamicGraph(int N) : N(N){
        this->SAMPLE_PROBABILITY_THRESHOLD = 11 * log2(N) / sqrt(N);
        random_device rd;
        default_random_engine generator(rd());
        uniform_real_distribution<double> distribution(0.0, 1.0);
        A.resize(N);
        D.resize(N);
        As.resize(N);
        Ds.resize(N);
        AA.resize(N);

        for (int i = 0; i < N; ++i) {
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

    void insertEdge(const pair<int, int>& edge) {
        update(edge);
        if (checkCycle(edge)) {
            throw CycleFormedException(edge);
        }
    }
};

int main()
{
    int N;
    cout << "How many nodes?" << endl;
    cin >> N;

    int M;
    cout << "How many edges per node?" << endl;
    cin >> M;
    vector<pair<int, int>> totalEdgesToInsert;

    for (size_t i = 0; i < N - M; i++) {
        for (size_t j = 0; j < M; j++)
        {
            totalEdgesToInsert.push_back(make_pair(i, i + j + 1));
        }
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

    // ok en lehet felreertem ezt az egeszet, de en a statikusnal ugy megyek, 
    // hogy letrehozom az elso ellel a grafot, utana az elso kettovel, utana elso harommal... es igy tovabb

    StaticGraph sg = StaticGraph(1);
    vector<pair<int, int>> edgesToInsertStatic;
    unordered_set<int> nodesStatic;
    auto start_static = chrono::high_resolution_clock::now();
    for (auto edge : totalEdgesToInsert) {
        nodesStatic.insert(edge.first);
        nodesStatic.insert(edge.second);
        edgesToInsertStatic.push_back(edge);
        sg = StaticGraph(nodesStatic.size());
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
        } catch (const exception& exc) {
            cout << exc.what();
        }
    }
    cout << "Done" << endl;
    return 0;
}

