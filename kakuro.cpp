// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <chrono>


int get_sum(short a) {
    int sum = 0;
    int cnt = 0;
    while (a > 0) {
        cnt++;
        if (a & 1) sum += cnt;
        a /= 2;
    }
    return sum;
}
typedef std::vector<std::vector<int>> vector2d;
typedef std::vector <std::vector<bool>> vector2dbool;

using namespace std;
int gcnt = 0;

vector<vector<short>> possible_bitmap(100, vector<short>(10, 0));
vector<vector<vector< short >>> possible_sets(100, vector<vector< short>>(10, vector<short>(2000)));

std::chrono::time_point<std::chrono::high_resolution_clock> begin1; 

bool solve(vector<vector<short>> &current, const vector2d& horizontal, const vector2d& vertical, const vector<pair<int, int> >& questions, const vector<vector<pair<int, int>>>& block_size, vector2d& sc1, vector2d& sc2, vector2d& sc3,const vector<vector<bool>> &blocked) {
    gcnt++;
    int n, m;
    n = current.size();
    m = current[0].size();

    for (pair<int, int> p : questions) {
        int i = p.first, j = p.second;
        sc3[i][j] = 0;
        int cursecond = block_size[i][j].second;
        for (int k = 1; k <= block_size[i][j].first; k++) {
            int ci = i + k;
            sc1[ci][j] = current[ci][j];
            if (__popcnt16(current[ci][j]) == 1) {
                sc3[i][j] |= current[ci][j];

            }
        }
        for (int k = 1; k <= block_size[i][j].second; k++) {
            int cj = j + k;
            sc2[i][cj] = current[i][cj];
            if (__popcnt(current[i][cj]) == 1) {
                sc3[i][j] |= (((int)current[i][cj]) << 16);
            }
        }
    }
    for (pair<int, int> p : questions) {
        int i = p.first, j = p.second;
        for (int k = 1; k <= block_size[i][j].first; k++) {
            int ci = i + k;
            if (__popcnt16(current[ci][j]) != 1) {
                short t2 = sc3[i][j] & ((1 << 16) - 1);
                short t = possible_sets[vertical[i][j]][block_size[i][j].first][t2];
                sc1[ci][j] &= t;
            }
        }
        for (int k = 1; k <= block_size[i][j].second; k++) {
            int cj = j + k;
            if (__popcnt(current[i][cj]) > 1) {

                short t2 = (sc3[i][j] >> 16);
                short t = possible_sets[horizontal[i][j]][block_size[i][j].second][t2];
                sc2[i][cj] &= t;
            }
        }
    }
    vector< pair<pair<int, int>, pair<pair<int, int>,int > > > notset;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            
            if (__popcnt(current[i][j]) > 1) {
                short pos = current[i][j] & sc1[i][j] & sc2[i][j];
                // early return if a solution is impossible
                if ((!blocked[i][j]) && pos == 0) 
                    return false;

                notset.push_back({ {__popcnt16(pos),pos }, {{i,j},current[i][j]}});
                current[i][j] = pos;
            }
        }
    }

    if (notset.size() == 0) {
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - begin1).count() << "micro seconds seconds" << std::endl;
        cout << "FOUND and answer" << endl;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                cout << get_sum(current[i][j]) << ' ';
            }
            cout << endl;
        }
        return true;
    }
    
    sort(notset.begin(), notset.end());
    int cnt = 0;
    for (int k = 0; k < 1; k++) {
        int i = notset[k].second.first.first, j = notset[k].second.first.second;
        short possible = notset[k].first.second;
        short tmp = current[i][j];
        for (int m2 = 0; m2 < 9; m2++) {
            if (possible & (1 << m2)) {
               
                current[i][j] = (1 << m2);
                cnt++;
                if (solve(current, horizontal, vertical, questions, block_size, sc1, sc2, sc3,blocked)) {
                    return true;
                }
                else {
                    current[i][j] = tmp;
                }
            }
        }
    }
    
    //roll back changes
    for (int k = 0; k < notset.size(); k++) {
        int i = notset[k].second.first.first, j = notset[k].second.first.second;
        short t = notset[k].second.second;
        current[i][j] = t;
    }
       
   
    return false;
}


bool solve_dumb(vector2d& table, const vector2d& horizontal, const vector2d& vertical, const vector<pair<int, int> >& questions, const vector<vector<pair<int, int>>>& block_size,int d  = 0) {
    

    if (d > 35)
        cout << d << endl;

    // check for inconsisitentyc
    for (pair<int, int> p : questions) {
        int i = p.first, j = p.second;
        int sum = 0;
        int mask = 0;
        for (int k = 1; k <= block_size[i][j].first; k++) {
            sum += table[i + k][j];
            if (table[i + k][j] == 0) continue;
            if (mask & ((1 << (table[i + k][j] - 1)))) return false;
            mask |= (1 << (table[i + k][j] - 1));
        }
        if (sum > vertical[i][j]) return false;
        sum = 0;
        mask = 0;
        for (int k = 1; k <= block_size[i][j].second; k++) {
            sum += table[i][j + k];
            if (table[i][j + k] == 0) continue;
            if (mask & ((1 << (table[i][j+ k] - 1)))) return false;
            mask |= (1 << (table[i][j + k] - 1));
        }
        if (sum > horizontal[i][j]) return false;
    }
    bool found_empty = false;

    // expand tree
    for (pair<int, int> p : questions) {
        int i = p.first, j = p.second;
        for (int k = 1; k <= block_size[i][j].first; k++) {
            if (table[i + k][j] == 0) {
                found_empty = true;
                for (int m = 1; m <= 9; m++) {
                    table[i + k][j] = m;
                    if (solve_dumb(table, horizontal, vertical, questions, block_size,d+1)) {
                        return true;
                    }
                }
                table[i + k][j] = 0;
                return false;
            }
            if (found_empty)
                return false;
        }
        for (int k = 1; k <= block_size[i][j].second; k++) {
            if (table[i][j + k] == 0) {
                found_empty = true;
                for (int m = 1; m <= 9; m++) {
                    table[i][j + k] = m;
                    if (solve_dumb(table, horizontal, vertical, questions, block_size,d+1)) {
                        return true;
                    }
                }
                table[i][j+ k] = 0;
                return false;

            }
            if (found_empty)
                return false;
        }
    }
    cout << "at a leaf" << endl;
    // if at a leaf check the sum
    if (!found_empty) {
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - begin1).count() << "micro seconds" << std::endl;
        for (int i = 0; i < table.size(); i++) {
            for (int j = 0; j < table[i].size(); j++) {
                cout << table[i][j] << ' ';
            }
            cout << endl;
        }
        return true;
    }
    return false;
}

int main()
{   
    using namespace std;
    
    
    for (int i = 0; i < (1 << 9); i++) {
        int bit_cnt = __popcnt16(i);
        int sum = get_sum(i);
        possible_bitmap[sum][bit_cnt] |= i;
        for (int j = 0; j <= i; j++) {
            if ((i & j) != j) continue;       
            possible_sets[sum][bit_cnt][j] |= i - j;
        }
    }
   
    
    istringstream inp("10 1 3 4 17,1 7 8 10,2 1 4 20,2 7 9 15,3 1 2 5,3 4 9 25,4 2 3 8,4 5 6 3,4 8 9 15,5 2 3 3,5 7 8 17,6 1 2 9,6 4 5 3,6 7 8 6,7 1 6 38,7 8 9 17,8 1 3 7,8 6 9 12,9 2 3 4,9 6 7 3;1 2 3 3,1 6 8 13,2 2 9 37,3 1 2 17,3 4 5 3,3 7 9 10,4 1 3 19,4 6 7 7,5 3 4 6,5 6 7 5,6 3 4 3,6 7 9 16,7 1 3 7,7 5 6 10,7 8 9 3,8 1 8 44,9 2 4 23,9 7 8 11.");
    int n, m;
    inp >> n;
    m = n;
    cout << n << ' ' << m << endl;
    vector<vector<bool>> blocked(n, vector<bool>(m,true));
  
    vector<vector<int>> vertical(n, vector<int>(m, 0));
    vector<vector<int>> horizontal(n, vector<int>(m, 0));
    vector<pair<int, int> > questions;
    // first is vertical second is horizontal
    vector<vector<pair<int, int>>> block_size(n, vector<pair<int, int>>(m, { 0,0 }));
    bool tvertical = false;
    char tmp;
    int a, b , size, sum;
    while (true) {
        inp >> a >> b >> size >> sum;
        a--;
        b--;
        
        size -= b;
        inp >> tmp; 
        cerr << a <<  ' ' << b << ' ' << size << ' ' << tmp << endl;
        if (tvertical) {
            swap(a, b);
            b++;
            vertical[a][b] = sum;
            
            block_size[a][b].first = size;
            for (int j = a + 1; j <= a + size; j++) {
                blocked[j][b] = false;
            }
        }
        else {
            a++;
            horizontal[a][b] = sum;
            block_size[a][b].second = size;
            for (int j = b + 1; j <= b + size; j++) {
                blocked[a][j] = false;
            }
        }
        if (tmp == '.') break;

        if (tmp == ';') tvertical = true;
        

    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (blocked[i][j] && (vertical[i][j] != 0 || horizontal[i][j] != 0)){
                questions.push_back({ i,j });
              
            }
        }
    }

    vector<vector<short>> currently_possible(n, vector<short>(m, 1023));
    for (pair<int, int> p : questions) {
        int i = p.first; int j = p.second;
        for (int k = 1; k <= block_size[i][j].first;k++) {
            if (i + k == 1 && j == 6) {
                cout << possible_bitmap[vertical[i][j]][block_size[i][j].first] << ' '  << vertical[i][j] << endl;
            }
            currently_possible[i + k][j] &= possible_bitmap[vertical[i][j]][block_size[i][j].first];
        
        }
        for (int k = 1; k <= block_size[i][j].second; k++) {
            if (i  == 1 && j + k == 6) {
                cout << possible_bitmap[horizontal[i][j]][block_size[i][j].first] << endl;
            }
            currently_possible[i][j + k] &= possible_bitmap[horizontal[i][j]][block_size[i][j].second];
        }
    }
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (blocked[i][j]) currently_possible[i][j] = 0;
            cout <<setw(4) << ' ' <<  currently_possible[i][j] << ' ';
        }
        cout << endl;
    }
    // scratch pads
    vector2d sc1(n, vector<int>(m, 0));
    vector2d sc2(n, vector<int>(m, 0));
    vector2d sc3(n, vector<int>(m, 0));
   

    // code to benchmark
    vector2d table(n, vector<int>(m, 0));
    begin1 = std::chrono::high_resolution_clock::now();
    //solve_dumb(table, horizontal, vertical, questions, block_size);
    begin1 = std::chrono::high_resolution_clock::now();
    if (!solve(currently_possible, horizontal, vertical, questions, block_size, sc1, sc2, sc3,blocked)) {
        cout << "Couldn't Find a solution" << endl;
    }
    cout << gcnt << endl;
}

