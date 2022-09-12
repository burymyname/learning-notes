#include<iostream>
#include <bits/stdc++.h>

using namespace std;

int Cost = INT_MAX;
int n, m;
int endX, endY;
const int d[] = {0, 1, 0, -1, 0};

void dfs(vector<vector<int>>& map, int x, int y, int cost, vector<vector<bool>>& visited);

int main(int argc, char** argv) {
    
    while (cin >> n >> m) {
        vector<vector<int>> map(n, vector<int>(m, 0));
        int x, y;
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                cin >> map[i][j];

                if (map[i][j] == 2) {
                    x = i;
                    y = j;
                }
            }
        }

        vector<vector<bool>> visited(n, vector<bool>(m, false));
        dfs(map, x, y, 0, visited);
        cout << Cost << endl;
        return 0;
    }

    return 0;
}

void dfs(vector<vector<int>>& map, int x, int y, int cost, 
         vector<vector<bool>>& visited) {
    
    // pruning
    if (map[x][y] == 3) {
        if (cost < Cost) {
            Cost = cost;
        }
        return;
    }

    if (cost >= Cost) {
        return;
    }

    int curCost = cost + abs(endX - x) + abs(endY - y);
    if (curCost >= Cost) {
        return;
    }

    for (int i = 0; i < 4; ++i) {
        int nextX = x + d[i];
        int nextY = y + d[i+1];

        if (nextX < 0  || nextY < 0  ||
            nextX >= n || nextY >= m ||
            visited[nextX][nextY]    ||
            map[nextX][nextY] == 1   ||
            map[nextX][nextY] == 2) {
            
            continue;
        }

        // path or terminat
        if (map[nextX][nextY] == 0 || map[nextX][nextY] == 3) {
            ++cost;
            visited[nextX][nextY] = true;
            dfs(map, nextX, nextY, cost, visited);
            --cost;
            visited[nextX][nextY] = false;
        
        // wall
        } else if (map[nextX][nextY] == 1) {
            continue;
        
        // trap
        } else if (map[nextX][nextY] == 4) {
            cost += 3;
            visited[nextX][nextY] = true;
            dfs(map, nextX, nextY, cost, visited);
            cost -= 3;
            visited[nextX][nextY] = false;

        // bomb
        } else if (map[nextX][nextY] == 6) {
            ++cost;
            visited[nextX][nextY] = true;
            // clear wall
            vector<vector<bool>> mask(n, vector<bool>(m, false));
            for (int k = 0; k < 4; ++k) {
                int nnx = nextX + d[k];
                int nny = nextY + d[k+1];

                if (nnx < 0  || nny < 0  ||
                    nnx >= n || nny >= m ||
                    visited[nnx][nny]    ||
                    map[nnx][nny] != 1) {
                    
                    continue;
                }

                map[nnx][nny] = 0;
                mask[nnx][nny] = true;
            }

            dfs(map, nextX, nextY, cost, visited);

            // fix wall
            for (int t1 = 0; t1 < n; ++t1) {
                for (int t2 = 0; t2 < m; ++t2) {
                    if (mask[t1][t2]) {
                        map[t1][t2] = 1;
                    }
                }
            }

            --cost;
            visited[nextX][nextY] = false;
        }
    }
}