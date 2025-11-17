#include "DataStructure.h"
#include "FileGenerator.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>
#include <vector>
#include <map>
#include <iomanip>
#include <algorithm>
#include <stdexcept>

using namespace std;

constexpr int THREAD_CASES[] = { 1, 2, 3 };
constexpr int MAX_THREADS = 3;

vector<string> load_file(const string& filename)
{
    vector<string> res;
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("File could not be opened: " + filename);
    }
    string s;
    while (getline(file, s)) res.push_back(s);
    return res;
}

void execute(MultithreadedDataStructure& ds, const vector<string>& actions)
{
    for (const auto& line : actions) {
        stringstream ss(line);
        string cmd;
        ss >> cmd;

        if (cmd == "read") {
            int idx;
            if (!(ss >> idx)) continue;
            try {
                ds.getValue(idx);
            }
            catch (const out_of_range&) {}
        }
        else if (cmd == "write") {
            int idx, val;
            if (!(ss >> idx >> val)) continue;
            try {
                ds.setValue(idx, val);
            }
            catch (const out_of_range&) {}
        }
        else if (cmd == "string") {
            volatile string s = (string)ds;
        }
    }
}

long long benchmark(int threads, const vector<vector<string>>& actions_per_thread)
{
    MultithreadedDataStructure ds;

    auto start = chrono::high_resolution_clock::now();
    vector<thread> th;

    for (int i = 0; i < threads; i++) {
        const vector<string>& current_actions = actions_per_thread[i];
        th.emplace_back(execute, ref(ds), cref(current_actions));
    }

    for (auto& x : th) x.join();

    auto end = chrono::high_resolution_clock::now();
    return chrono::duration_cast<chrono::milliseconds>(end - start).count();
}

int main() {
    try {
        FileGenerator g;
        g.generate_files(OP_COUNT, MAX_THREADS);

        map<string, vector<vector<string>>> actions_map;
        for (const string& prefix : FILE_PREFIXES) {
            for (int t = 0; t < MAX_THREADS; t++) {
                actions_map[prefix].push_back(load_file(prefix + "_" + to_string(t) + ".txt"));
            }
        }

        cout << "\n==================================================================" << endl;
        cout << "                PERFORMANCE RESULTS (ms)" << endl;
        cout << "==================================================================" << endl;
        cout << setw(8) << "Threads" << " | "
            << setw(16) << "File A" << " | "
            << setw(16) << "File B" << " | "
            << setw(16) << "FIle C" << endl;
        cout << "---------|------------------|------------------|------------------" << endl;

        for (int t : THREAD_CASES) {
            vector<vector<string>> Afiles(actions_map["A"].begin(), actions_map["A"].begin() + t);
            vector<vector<string>> Bfiles(actions_map["B"].begin(), actions_map["B"].begin() + t);
            vector<vector<string>> Cfiles(actions_map["C"].begin(), actions_map["C"].begin() + t);

            long long timeA = benchmark(t, Afiles);
            long long timeB = benchmark(t, Bfiles);
            long long timeC = benchmark(t, Cfiles);

            cout << setw(8) << t << " | "
                << setw(13) << timeA << " ms" << " | "
                << setw(13) << timeB << " ms" << " | "
                << setw(13) << timeC << " ms" << endl;
        }

        cout << "==================================================================" << endl;

    }
    catch (const exception& e) {
        cerr << "Critical runtime error: " << e.what() << endl;
        return 1;
    }

    return 0;
}