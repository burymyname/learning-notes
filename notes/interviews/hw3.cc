#include<iostream>
#include <bits/stdc++.h>

using namespace std;


int main(int argc, char** argv) {
    int D, N;
    while (cin >> D >> N) {
        vector<int> dis(N+1, 0);
        vector<int> wait(N+1, 0);

        for (int i = 1; i <= N; ++i) {
            cin >> dis[i] >> wait[i];
            if (dis[i] - dis[i-1] > 1000) {
                cout << -1 << endl;
                return 0;
            }
        }

        if (D - dis[N] > 1000) {
            cout << -1 << endl;
            return 0;
        }

        // dp[i][j]: 从i开始
    }
    return 0;
}