#pragma once

#include <vector>
#include <string>
#include <mutex>
#include <shared_mutex>
#include <sstream>
#include <algorithm>
#include <stdexcept>

constexpr int NUM_FIELDS = 2;

class MultithreadedDataStructure {
private:
    std::vector<int> fields = std::vector<int>(NUM_FIELDS, 0);

    mutable std::mutex mutex0;
    mutable std::shared_mutex shared_mutex1;

public:
    MultithreadedDataStructure() = default;

    int getValue(int idx) const {
        if (idx == 0) {
            std::lock_guard<std::mutex> lock(mutex0);
            return fields[0];
        }
        else if (idx == 1) {
            std::shared_lock<std::shared_mutex> lock(shared_mutex1);
            return fields[1];
        }
        throw std::out_of_range("Incorrect field index.");
    }

    void setValue(int idx, int value) {
        if (idx == 0) {
            std::lock_guard<std::mutex> lock(mutex0);
            fields[0] = value;
        }
        else if (idx == 1) {
            std::unique_lock<std::shared_mutex> lock(shared_mutex1);
            fields[1] = value;
        }
        else {
            throw std::out_of_range("Incorrect field index.");
        }
    }

    operator std::string() {
        std::unique_lock<std::mutex> lock0{ mutex0, std::defer_lock };
        std::unique_lock<std::shared_mutex> lock1{ shared_mutex1, std::defer_lock };

        std::lock(lock0, lock1);

        std::ostringstream oss;
        oss << "Structure: Field 0 = " << fields[0]
            << ", Field 1 = " << fields[1];
        return oss.str();
    }
};