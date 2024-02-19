#include <iostream>
#include <vector>
#include <chrono>
#include "graphalg.h"

using namespace std;


int main()
{
    int N;
    cout << "How many nodes?" << endl;
    cin >> N;

    int M;
    cout << "How many edges per node?" << endl;
    cin >> M;

    // ok ezt tudom h nem pontosan annyi mert az utolso M-bol nem fog kiindulni csucs, majd ezen javitok, de elsore jo kozelites

    vector<pair<int, int>> totalEdgesToInsert;

    for (size_t i = 0; i < N - M; i++) {
        for (size_t j = 0; j < M; j++)
        {
            totalEdgesToInsert.push_back(make_pair(i, i + j + 1));
        }
    }
    totalEdgesToInsert.push_back(make_pair(N - 1, N - M - 1));

    auto start_dynamic = chrono::high_resolution_clock::now();

    DynamicGraph dg = DynamicGraph(N);

    for (auto edge : totalEdgesToInsert) {
        if (!dg.insertEdge(edge)) {
            auto stop_dynamic = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::milliseconds>(stop_dynamic - start_dynamic);
            cout << "Dynamic: Cycle was formed at inserting edge: (" << edge.first << ", " << edge.second << "). Time taken: " << duration.count() << " ms." << endl;
            break;
        }
    }

    auto start_static = chrono::high_resolution_clock::now();
    StaticGraph sg = StaticGraph(N);

    for (auto edge : totalEdgesToInsert) {
        if (!sg.insertEdge(edge)) {
            auto stop_static = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::milliseconds>(stop_static - start_static);
            cout << "Static: Cycle was formed at inserting edge: (" << edge.first << ", " << edge.second << "). Time taken: " << duration.count() << " ms." << endl;
            break;
        }
    }

    cout << "Done" << endl;
    return 0;
}