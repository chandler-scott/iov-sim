/*
 * NeighborTable.h
 *
 *  Created on: Jul 24, 2023
 *      Author: chandler
 */

#ifndef IOV_SIM_BASE_UTIL_NEIGHBORTABLE_H_
#define IOV_SIM_BASE_UTIL_NEIGHBORTABLE_H_

#include <iostream>
#include <unordered_map>
#include <string>
#include "iov_sim/base/util/NeighborEntry.h"

using namespace std;


class NeighborTable {
private:
    std::unordered_map<std::string, NeighborEntry> table;
    std::unordered_map<std::string, double> score;
    double timeout;


public:
    // Constructor
    explicit NeighborTable(double defaultTimeout = 30.0);

    // Add a row to the table
    void addRow(const NeighborEntry& entry, const char* nodeId);

    // Prune rows from the table based on the given currentTime
    void pruneTable(double currentTime);

    // calculate averages
    void calculateMetadata();

    void scoreNeighbors();

    // Reset the table to empty
    void resetTable();

    // Print the scores of each neighbor
    void printScores();

    // Print the calculated averages
    void printAverages();

    // Print the table
    void printTable();

private:
    double avgXVelocity;
    double avgYVelocity;
    double avgVelocity;
    double avgSpeed;
    double avgAcceleration;
    double avgDeceleration;
    double avgXPosition;
    double avgYPosition;

    // Degree of centricity
    double centricity;
};

#endif /* IOV_SIM_BASE_UTIL_NEIGHBORTABLE_H_ */
