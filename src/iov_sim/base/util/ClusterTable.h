/*
 * COLOR.h
 *
 *  Created on: Aug 3, 2023
 *      Author: chandler
 */

#ifndef IOV_SIM_BASE_UTIL_CLUSTERTABLE_H_
#define IOV_SIM_BASE_UTIL_CLUSTERTABLE_H_

#include <iostream>
#include <unordered_map>
#include <string>
#include <limits>
#include <vector>
#include <sstream>
#include <algorithm>



#include "iov_sim/base/util/NeighborEntry.h"

using namespace std;


class ClusterTable {
private:
    std::unordered_map<std::string, NeighborEntry> table;
    std::unordered_map<std::string, double> score;
    double timeout;


public:
    // Constructor
    explicit ClusterTable(double defaultTimeout = 30.0);

    explicit ClusterTable(std::string initString, double defaultTimeout = 30.0);


    // Set weights for scoring neighbors
    void setWeights(double xVelWeight = 1, double yVelWeight = 1, double velWeight = 1,
                       double speedWeight = 1, double accelWeight = 1, double decelWeight = 1,
                       double xPosWeight = 1, double yPosWeight = 1, double timeWeight = 1,
                       double inRangeWeight = 1, double xDirectionWeight = 1, double yDirectionWeight = 1);

    // Add a row to the table
    void addRow(const NeighborEntry& entry, const char* nodeId);

    // Prune rows from the table based on the given currentTime
    void pruneTable(double currentTime);
    void pruneTable(const std::vector<std::string>& clusterNodes);

    // calculate averages
    void calculateMetadata();

    // score neighbors
    void scoreNeighbors();

    // get best scoring neighbor (closest to 0)
    std::string getBestScoringNeighbor();

    std::vector<std::string> getAllNeighbors() const;

    std::string toString() const;
    void fromString(const std::string& str);

    std::vector<double> toList();


    int getSize();

    // Reset the table to empty
    void resetTable();

    // Print the scores of each neighbor
    void printScores();

    // Print the calculated averages
    void printAverages();

    // Print the table
    void printTable();

    double avgCarsInRange;
    double avgXVelocity;
    double avgYVelocity;
    double avgVelocity;
    double avgSpeed;
    double avgAcceleration;
    double avgDeceleration;
    double avgXPosition;
    double avgYPosition;
    double avgTimestamp;
    double avgXDirection;
    double avgYDirection;
    // Degree of centricity
    double centricity;

private:
    double carsInRangeWeight;
    double xVelocityWeight;
    double yVelocityWeight;
    double velocityWeight;
    double speedWeight;
    double accelerationWeight;
    double decelerationWeight;
    double xPositionWeight;
    double yPositionWeight;
    double timeWeight;
    double xDirectionWeight;
    double yDirectionWeight;
    double centricityWeight;
};

#endif /* IOV_SIM_BASE_UTIL_CLSUTERTABLE_H_ */
