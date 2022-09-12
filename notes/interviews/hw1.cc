#include<iostream>
#include <bits/stdc++.h>

using namespace std;

void toLower(string& s) {
    int len = s.length();
    for (int i = 0; i < len; ++i) {
        if (isalpha(s[i]))
            s[i] = tolower(s[i]);
    }
}

int main(int argc, char** argv) {
    string line;
    while (getline(cin, line)) {
        unordered_map<string, int> word2idx;
        string word;
        int idx = 0;

        // transfor all word to lower case
        while (cin >> word) {
            toLower(word);
            word2idx[word] = idx++;
        }

        int startIdx = 0, endIdx = 0;
        bool isWord = false, ignore = false;
        int i = 0;
        while (i < line.length()) {
            if (line[i] == '"') {
                ignore = !ignore;
            } else if (!ignore && !isWord && isalpha(line[i])) {
                isWord = true;
                startIdx = i;
            } else if (!ignore && isWord && !isalpha(line[i])) {
                isWord = false;
                endIdx = i;

                string sub = line.substr(startIdx, endIdx-startIdx);
                toLower(sub);
                cout << sub << endl;
                if (word2idx.find(sub) != word2idx.end()) {
                    string idx = to_string(word2idx[sub]);
                    line.replace(startIdx, endIdx-startIdx, idx);
                    i = startIdx + idx.length() - 1;
                    // cout << i << endl;
                }

            }

            ++i;
        }
        cout << line << endl;
    }
    return 0;
}