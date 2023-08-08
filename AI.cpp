#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <unordered_set>
#include <unordered_map> 
#include <cmath>
#include <algorithm>
#include <functional>
#include <ctime>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>

using namespace std;

const int n = 3;
const int maxDepth = 150;
const int beamWidth = 4; 

struct State {
    vector<vector<int>> table;
    int cost;
    int depth = 0;
    State() {
        table = vector<vector<int>>(n, vector<int>(n, 0));
        cost = 0;
        int depth = 0;
    }

    bool operator==(const State& other) const {         // định nghĩa toán tử == giữa 2 state
        return table == other.table;
    }
    bool operator!=(const State& other) const {         // định nghĩa toán tử != giữa 2 state
        return !(*this == other);
    }
    int heuristic(const State& target) const {          // hàm đánh giá cho bước hiện tại 
        int h = 0;
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                int num = table[i][j];
                if (num != -1) {
                    int targetRow = (num - 1) / n;
                    int targetCol = (num - 1) % n;
                    h += std::abs(i - targetRow) + std::abs(j - targetCol);
                }
            }
        }
        return h;
    }

    int totalCost(const State& target) const {
        return cost + heuristic(target);
    }
};

namespace std {                     // định nghĩa cấu trúc cho bảng hash, sử dụng cho unordered_set/map
    template <>
    struct hash<State> {
        size_t operator()(const State& s) const {
            hash<int> int_hash;
            size_t hash_value = 0;
            for (const auto& row : s.table) {
                for (const auto& num : row) {
                    hash_value = hash_value * 31 + int_hash(num);
                }
            }
            return hash_value;

        }
    };
}
// hàm kiểm tra bước đi hợp lệ
bool isValidState(const State& s) {
    unordered_set<int> seen;
    for (const auto& row : s.table) {
        for (const auto& num : row) {
            if (num == -1) continue; // 
            if (num < 1 || num > n * n) return false; // 
            if (seen.find(num) != seen.end()) return false; // 
            seen.insert(num);
        }
    }
    return true;
}

// Hàm tìm kiếm danh sách bước đi hợp lệ tiếp theo từ trạng thái hiện tại
vector<State> getPossibleMoves(const State& s) {
    vector<State> moves;
    int emptyRow, emptyCol;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (s.table[i][j] == -1) {
                emptyRow = i;
                emptyCol = j;
                break;
            }
        }
    }

    int dx[] = {1, 0, -1, 0};
    int dy[] = {0, 1, 0, -1};

    for (int i = 0; i < 4; ++i) {
        int newRow = emptyRow + dx[i];
        int newCol = emptyCol + dy[i];

        if (newRow >= 0 && newRow < n && newCol >= 0 && newCol < n) {
            State newMove = s;
            swap(newMove.table[emptyRow][emptyCol], newMove.table[newRow][newCol]);
            if (isValidState(newMove)) {
                moves.push_back(newMove);
            }
        }
    }

    return moves;
}

// ------------------------------------------------- BFS ----------------------------------------------------

int BFS(const State& start, const State& target) {
    queue<State> q;
    unordered_set<State> visited;
    unordered_map<State, State> parent;
    int count = 0;

    q.push(start);
    visited.insert(start);

    while (!q.empty()) {
        count ++;
        State current = q.front();
        q.pop();

        if (current == target) {
            vector<State> path;
            while (current != start) {
                path.push_back(current);
                current = parent[current];
            }
            path.push_back(start);
            reverse(path.begin(), path.end());
            std::cout << "Successful Search !" << endl;
            std::cout << "Steps: " << count << endl;
            return count;
        }

        vector<State> moves = getPossibleMoves(current);
        for (const auto& next : moves) {
            if (visited.find(next) == visited.end()) {
                q.push(next);
                visited.insert(next);
                parent[next] = current;
            }
        }
    }

    return 0; 
}

//-------------------------------------------------- DFS -----------------------------------------------------

int DFS(const State& start, const State& target) {
    stack<State> open;
    unordered_set<State> visited;
    unordered_map<State, State> parent;
    int count = 0;

    open.push(start);
    visited.insert(start);

    while (!open.empty()) {
        count ++;
        State current = open.top();
        open.pop();

        if (current == target) {
            vector<State> path;
            while (current != start) {
                path.push_back(current);
                current = parent[current];
            }
            path.push_back(start);
            reverse(path.begin(), path.end());
            std::cout << "Successful Search !" << endl;
            std::cout << "Steps: " << count << endl;
            //return path;
            return count;
        }

        vector<State> moves = getPossibleMoves(current);
        for (const auto& next : moves) {
            if (visited.find(next) == visited.end()) {
                open.push(next);
                visited.insert(next);
                parent[next] = current;
            }
        }
    }

    return 0; 
}
// ------------------------------------------------------- DLS --------------------------------------------------------------

bool depthLimitedSearch(stack<State>& open, unordered_set<State>& visited,unordered_set<State>& Wait, const State& target, int depthLimit, int& count) {
    //unordered_map<State, State> parent;
    while (!open.empty()) {
        count ++;
        State current = open.top();
        open.pop();

        if (current == target) {
            std::cout << "Successful Search !" << endl;
            return true;
        }

        if (current.depth <= depthLimit) {                      
            vector<State> moves = getPossibleMoves(current);
            for (auto next : moves) {
                if (visited.find(next) == visited.end() && Wait.find(next) == Wait.end()) {
                    next.depth = current.depth+1;
                    open.push(next);
                    visited.insert(next);
                    //parent[next] = current;
                }
            }
        }else{
            Wait.insert(current);
        }
    }
    return 0; 
}

// ---------------------------------------------------------- DRS -----------------------------------------------------------------------

int DepthRepeatSearch(const State& start, const State& target) {
    unordered_set<State> visited;           // ClOSE
    stack<State> open;                       // OPEN
    unordered_set<State> Wait;              // NEXT           
    int count = 0;

    Wait.insert(start);

    for (int d = 1; d <= maxDepth; ++d) {
        for(auto st : Wait){
            open.push(st);
        }
        Wait.clear();
        if (depthLimitedSearch(open,visited,Wait,target,d,count)) {
            std::cout << "Find at depth: " << d << endl;
            std::cout << "Steps: " << count << endl;
            //return true;
            return count;
        }
    }

    std::cout << "Can't find Solution" << endl;
    return 0;
}

// --------------------------------------------------------HS-----------------------------------------------------

bool compareStates(const State& a, const State& b, const State& target) {
    return a.heuristic(target) > b.heuristic(target);
}

// hàm sắp xếp các state theo stack 
void sortStackByHeuristic(std::stack<State>& stack, const State& target) {
    std::vector<State> tempVector;

    while (!stack.empty()) {
        tempVector.push_back(stack.top());
        stack.pop();
    }
    std::sort(tempVector.begin(), tempVector.end(), [&](const State& a, const State& b) {
        return compareStates(a, b, target);
    });

    for (const auto& state : tempVector) {
        stack.push(state);
    }
}
int HillClimbingSearch(const State& start, const State& target) {
    stack<State> open;
    unordered_set<State> visited;
    unordered_map<State, State> parent;

    int count = 0;
    open.push(start);
    visited.insert(start);

    while (!open.empty()) {
        count ++;
        sortStackByHeuristic(open,target);                       // sắp xếp lại stack
        State current = open.top();
        open.pop();

         if (current == target) {
            vector<State> path;
            while (current != start) {
                current = parent[current];
            }
            std::cout << "Successful Search !" << endl;
            std::cout << "Steps: " << count << endl;
            //return path;
            return count;
        }
        vector<State> moves = getPossibleMoves(current);
        for (const auto& next : moves) {
            if (visited.find(next) == visited.end()) {
                
                open.push(next);
                visited.insert(next);
                parent[next] = current;
            }
        }
    }

    return 0;
}

// ------------------------------------------------------------ Best-first search ----------------------------------------------------------

int bestFirstSearch(const State& start, const State& target) {
    priority_queue<State, vector<State>, function<bool(const State&,const State&)>> open(
        [&target](const State& a, const State& b) {
            return a.heuristic(target) > b.heuristic(target);
        });

    int count = 0;
    unordered_set<State> visited;
    unordered_map<State, State> parent;

    open.push(start);
    visited.insert(start);

    while (!open.empty()) {
        count++;
        State current = open.top();
        open.pop();

        if (current == target) {
            vector<State> path;
            while (current != start) {
                path.push_back(current);
                current = parent[current];
            }
            path.push_back(start);
            reverse(path.begin(), path.end());
            std::cout << "Successful Search !" << endl;
            std::cout << "Steps: " << count << endl;
            //return path;
            return count;
        }

        vector<State> moves = getPossibleMoves(current);
        for (const auto& next : moves) {
            if (visited.find(next) == visited.end()) {
                open.push(next);
                visited.insert(next);
                parent[next] = current;
            }
        }
    }

    return 0; 
}


// --------------------------------------------------------- Beam Search------------------------------------------------------------

int beamSearch(const State& start, const State& target) {
    priority_queue<State, vector<State>, function<bool(const State&, const State&)>> open(
        [&target](const State& a, const State& b) {
            return a.heuristic(target) > b.heuristic(target);
        });
    int count = 0;
    unordered_set<State> visited;
    unordered_map<State, State> parent;
    open.push(start);
    visited.insert(start);

    while (!open.empty()) {
        count++;
        vector<State> candidates;
        int statesToExpand = min(static_cast<int>(open.size()), beamWidth); 
        for (int i = 0; i < statesToExpand; ++i) {
            candidates.push_back(open.top());
            open.pop();
        }

        for ( auto& current : candidates) {
            if (current == target) {
                vector<State> path;
                while (current != start) {
                    path.push_back(current);
                    current = parent[current];
                }
                path.push_back(start);
                reverse(path.begin(), path.end());
                std::cout << "Successful Search !" << endl;
                std::cout << "Steps: " << count << endl;
                //return path;
                return count;
            }

            vector<State> moves = getPossibleMoves(current);
            for (const auto& next : moves) {
                if (visited.find(next) == visited.end()) {
                    visited.insert(next);
                    parent[next] = current;
                    open.push(next);
                }
            }
        }
    }

    return 0;
}




int main() {

    State start, target;

    start.table = {
        {4, 7, 6},
        {3, -1, 5},
        {8, 1, 2}
    };

    target.table = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, -1}
    };

    start.cost = 0;
    int numTime = 10;
    vector<vector<int>> SearchTime;
    for(int i=0; i<6;i++){
        SearchTime.push_back(vector<int>());
    }

    vector<int> Steps;
    string names[6] = {"BS","DRS","B-FS","HS","BFS","DFS"};
    unordered_map<int,string> Name ;
    for(int i=1; i<=6;i++){
        Name[i] = names[i-1];
    }
    
    for(int i=0;i<numTime;i++){
        //stime.clear();
        auto starting = chrono::high_resolution_clock::now();

        int step = beamSearch(start, target);                                               //   9ms
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end - starting);
        std::cout << "Beam Search time : " << duration.count() << " milliseconds" << endl;
        SearchTime[0].push_back(duration.count());
        Steps.push_back(step);
        starting = chrono::high_resolution_clock::now();

        step = DepthRepeatSearch(start, target)  ;                                     //    4s
        end = chrono::high_resolution_clock::now();
        duration = chrono::duration_cast<chrono::milliseconds>(end - starting);
        std::cout << "DRS time : " << duration.count() << " milliseconds" << endl;
        SearchTime[1].push_back(duration.count());
        Steps.push_back(step);
        starting = chrono::high_resolution_clock::now();

        step = bestFirstSearch(start, target);                //    14ms
        end = chrono::high_resolution_clock::now();
        duration = chrono::duration_cast<chrono::milliseconds>(end - starting);
        std::cout << "Best-first Search time : " << duration.count() << " milliseconds" << endl;
        SearchTime[2].push_back(duration.count());
        Steps.push_back(step);
        starting = chrono::high_resolution_clock::now();

        step = HillClimbingSearch(start, target);   //      16ms
        end = chrono::high_resolution_clock::now();
        duration = chrono::duration_cast<chrono::milliseconds>(end - starting);
        std::cout << "Hill-Climbing Search time : " << duration.count() << " milliseconds" << endl;
        SearchTime[3].push_back(duration.count());
        Steps.push_back(step);
        starting = chrono::high_resolution_clock::now();

        step = BFS(start, target);                         // OK
        end = chrono::high_resolution_clock::now();
        duration = chrono::duration_cast<chrono::milliseconds>(end - starting);
        std::cout << "BFS time : " << duration.count() << " milliseconds" << endl;
        SearchTime[4].push_back(duration.count());
        Steps.push_back(step);
        starting = chrono::high_resolution_clock::now();

        step = DFS(start, target);                        // OK
        end = chrono::high_resolution_clock::now();
        duration = chrono::duration_cast<chrono::milliseconds>(end - starting);
        std::cout << "DFS time : " << duration.count() << " milliseconds" << endl;
        SearchTime[5].push_back(duration.count());
        Steps.push_back(step);

    }
    std::cout << "Complete !" << endl;              // thông báo hoàn thành

    // lưu thông số tìm kiếm vào 1 file 
    ofstream file("SearchTime_AI_Algorithm.txt");       
    if(file.is_open()){
        file << "There are search time of 6 AI_Algorithm with Numbering problem :  " << endl;
        for(auto name : Name){
            int TotalTime = 0;
            file << name.second << " - " ;
            for(int i=0; i<numTime;i++){
                file << SearchTime[name.first-1][i] << " " ;
                TotalTime += SearchTime[name.first-1][i];
            }
            file <<" -> " << Steps[name.first-1] << " steps | " << "AVG Time: " << TotalTime/numTime << " ms.";
            file << endl;
        }
        cout << "Done make Search time file" << endl;
        file.close();
    }else{
        std::cout << "Unable to open file !" << endl;
    }

    return 0;
    
}
