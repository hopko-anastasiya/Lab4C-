#pragma once
#include <random>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <stdexcept>
#include "DataStructure.h"

constexpr int OP_COUNT = 100000;
const std::string FILE_PREFIXES[] = { "A", "B", "C" };


class FileGenerator {
private:
    std::mt19937 gen;

    std::string generate_action(const std::map<std::string, double>& frequencies) {
        std::vector<std::string> ops;
        std::vector<double> probs;

        for (const auto& pair : frequencies) {
            ops.push_back(pair.first);
            probs.push_back(pair.second);
        }

        std::discrete_distribution<> dist(probs.begin(), probs.end());
        return ops[static_cast<int>(dist(gen))];
    }

public:
    FileGenerator() : gen(std::random_device{}()) {}

    void generate_files(int operations, int threads)
    {
        std::cout << "Generation of action sequence files (" << operations << " operations per file)..." << std::endl;

        std::map<std::string, double> freq_A = {
            {"read 0", 10}, {"write 0 1", 10},
            {"read 1", 50}, {"write 1 1", 10},
            {"string", 20}
        };


        std::map<std::string, double> freq_B = {
            {"read 0", 20}, {"write 0 1", 20},
            {"read 1", 20}, {"write 1 1", 20},
            {"string", 20}
        };

        std::map<std::string, double> freq_C = {
            {"read 0", 5}, {"write 0 1", 20},
            {"read 1", 5}, {"write 1 1", 20},
            {"string", 50}
        };

        std::vector<std::map<std::string, double>> frequencies = { freq_A, freq_B, freq_C };

        for (int i = 0; i < frequencies.size(); ++i) {
            const std::string& prefix = FILE_PREFIXES[i];

            for (int t = 0; t < threads; t++) {
                std::ofstream out(prefix + "_" + std::to_string(t) + ".txt");
                if (!out.is_open()) {
                    throw std::runtime_error("Could not open file for writing: " + prefix + "_" + std::to_string(t) + ".txt");
                }

                for (int op = 0; op < operations; op++) {
                    out << generate_action(frequencies[i]) << "\n";
                }
            }
            std::cout << "Generated " << threads << " files for profile " << prefix << "." << std::endl;
        }
    }
};