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

        
        vector<vector<int>> dp(N+1, vector<int>(1001, 0));
        for (int j = 0; j <= 1000; ++j) {
            int d = D - dis[N];
            if (j >= d) dp[N][j] = d / 100;
            else dp[N][j] = d/100 + wait[N] + 1;
        }

    
        for (int i = N-1; i >= 0; --i) {
            for (int j = 0; j <= 1000; ++j) {
                int d = dis[i+1] - dis[i];
                if (j >= d) {
                    dp[i][j] = INT_MAX;
                    if (dp[i+1][j-d] != -1) dp[i][j] = min(dp[i][j], dp[i+1][j-d] + d/100);
                    if (dp[i+1][1000-d] != -1) dp[i][j] = min(dp[i][j], dp[i+1][1000-d] + d/100 + wait[i] + 1);
                    if (dp[i][j] == INT_MAX) dp[i][j] = -1;
                } else {
                    if (dp[i+1][1000-d] == -1) dp[i][j] = -1;
                    else {
                        dp[i][j] = dp[i+1][1000-d] + d/100 + wait[i] + 1;
                    } 
                }
            }
        }

        cout << dp[0][1000] << endl;        
    }
    return 0;
}