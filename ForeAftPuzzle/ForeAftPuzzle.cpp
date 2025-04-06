#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

struct State {
    string board;
    int g, h;
    vector<string> path;

    State(string b, int g_cost, int h_cost, vector<string> p)
        : board(std::move(b)), g(g_cost), h(h_cost), path(std::move(p)) {}

    int f() const {
        return g + h;
    }

    bool operator>(const State &other) const {
        return f() > other.f();
    }
};

string board_to_string(const vector<string>& board) {
    stringstream ss;
    for (const auto& row : board) {
        ss << row;
    }
    return ss.str();
}

vector<string> string_to_board(const string& board_str, int width) {
    vector<string> board;
    for (int i = 0; i < board_str.size(); i += width) {
        board.push_back(board_str.substr(i, width));
    }
    return board;
}

unordered_map<string, int> precompute_pattern_database(const string& goal, const vector<int>& group, int width, int height) {
    unordered_map<string, int> database;
    queue<pair<string, int>> q;

    string initial_pattern(width * height, ' ');
    for (int idx : group) {
        initial_pattern[idx] = goal[idx];
    }
    q.push({initial_pattern, 0});
    database[initial_pattern] = 0;

    while (!q.empty()) {
        auto [current, cost] = q.front(); q.pop();

        for (size_t i = 0; i < current.size(); ++i) {
            if (current[i] == ' ') continue;

            for (size_t j = 0; j < current.size(); ++j) {
                if (i == j || current[j] != ' ') continue;

                string new_pattern = current;
                swap(new_pattern[i], new_pattern[j]);

                if (!database.count(new_pattern)) {
                    database[new_pattern] = cost + 1;
                    q.push({new_pattern, cost + 1});
                }
            }
        }
    }

    return database;
}

int dpdb_heuristic(const string& board, const vector<unordered_map<string, int>>& databases, const vector<vector<int>>& groups) {
    int h = 0;
    for (size_t i = 0; i < groups.size(); ++i) {
        string pattern(board.size(), ' ');
        for (int idx : groups[i]) {
            pattern[idx] = board[idx];
        }
        if (databases[i].count(pattern)) {
            h += databases[i].at(pattern);
        } else {
            h += 100000; // Large penalty to avoid unrecognized patterns
        }
    }
    return h;
}

vector<string> get_neighbors(const string& board, int width, int height) {
    vector<string> neighbors;
    vector<pair<int, int>> directions = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };

    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            int idx = r * width + c;
            if (board[idx] == 'R' || board[idx] == 'B') {
                for (auto [dr, dc] : directions) {
                    int nr = r + dr, nc = c + dc;
                    int nidx = nr * width + nc;
                    if (nr >= 0 && nr < height && nc >= 0 && nc < width && board[nidx] == ' ') {
                        string new_board = board;
                        swap(new_board[idx], new_board[nidx]);
                        neighbors.push_back(new_board);
                    }
                }
            }
        }
    }
    return neighbors;
}

void solve_puzzle(const vector<string>& initial_board, const vector<string>& goal_board, const string& output_file) {
    ofstream fout(output_file);
    if (!fout) {
        cerr << "Error opening file: " << output_file << endl;
        return;
    }

    int height = initial_board.size();
    int width = initial_board[0].size();
    string initial = board_to_string(initial_board);
    string goal = board_to_string(goal_board);

    vector<vector<int>> groups = {
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 15}, // Red tiles
        {11, 12, 13, 14, 16, 17, 18, 19, 20, 21, 22, 23, 24} // Blue tiles
    };

    vector<unordered_map<string, int>> databases;
    for (const auto& group : groups) {
        databases.push_back(precompute_pattern_database(goal, group, width, height));
    }

    priority_queue<State, vector<State>, greater<State>> pq;
    unordered_set<string> visited;
    pq.emplace(initial, 0, dpdb_heuristic(initial, databases, groups), vector<string>{initial});

    int iteration = 0;

    while (!pq.empty()) {
        State current = pq.top(); pq.pop();

        if (visited.count(current.board)) continue;
        visited.insert(current.board);

        // Print iteration to console
        cout << "Iteration: " << iteration << endl;

        // Log iteration to file
        fout << iteration << ":\n";
        vector<string> board = string_to_board(current.board, width);
        for (const auto& row : board) {
            fout << row << "\n";
        }
        fout << "\n";
        iteration++;

        if (current.board == goal) {
            fout << "Solution completed in " << current.g << " steps.\n";
            for (size_t i = 0; i < current.path.size(); ++i) {
                fout << "Step " << i + 1 << ":\n";
                vector<string> board = string_to_board(current.path[i], width);
                for (const auto& row : board) {
                    fout << row << "\n";
                }
                fout << "\n";
            }
            fout.close();
            cout << "Solution written to " << output_file << endl;
            return;
        }

        auto neighbors = get_neighbors(current.board, width, height);
        for (const auto& neighbor : neighbors) {
            if (!visited.count(neighbor)) {
                auto new_path = current.path;
                new_path.push_back(neighbor);
                pq.emplace(neighbor, current.g + 1, dpdb_heuristic(neighbor, databases, groups), new_path);
            }
        }
    }
    fout << "No solution found for the puzzle." << endl;
    fout.close();

    cout << "No solution found for the puzzle." << endl;
}

int main() {
    vector<string> board5 = { 
        "RRRRR", 
        "RRRRR", 
        "RR BB", 
        "BBBBB", 
        "BBBBB" 
    };
    vector<string> goal5 = { 
        "BBBBB", 
        "BBBBB", 
        "BB RR", 
        "RRRRR", 
        "RRRRR" 
    };
    solve_puzzle(board5, goal5, "output.out");

    return 0;
}