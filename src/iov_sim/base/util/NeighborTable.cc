/*
 * NeighborTable.cpp
 *
 *  Created on: Jul 24, 2023
 *      Author: chandler
 */

#include "NeighborTable.h"

// Constructor
NeighborTable::NeighborTable(double defaultTimeout) : timeout(defaultTimeout)
{
    setWeights();
}

// Set weights for scoring neighbors
void NeighborTable::setWeights(double inRangeWeight, double xVelWeight,
        double yVelWeight, double velWeight, double speedWeight, double accelWeight,
        double decelWeight, double xPosWeight, double yPosWeight,double timeWeight)
{
    carsInRangeWeight = inRangeWeight;
    xVelocityWeight = xVelWeight;
    yVelocityWeight = yVelWeight;
    velocityWeight = velWeight;
    speedWeight = speedWeight;
    accelerationWeight = accelWeight;
    decelerationWeight = decelWeight;
    xPositionWeight = xPosWeight;
    yPositionWeight = yPosWeight;
    timeWeight = timeWeight;
}

// Add a row to the table
void NeighborTable::addRow(const NeighborEntry& entry, const char* nodeId) {
    table[nodeId] = entry;
}

// Prune rows from the table based on the given currentTime
void NeighborTable::pruneTable(double currentTime) {
    auto it = table.begin();
    while (it != table.end()) {
        if (currentTime - it->second.timestamp > timeout) {
            it = table.erase(it);
        } else {
            ++it;
        }
    }
}

// Calculate metadata for the neighbor table
void NeighborTable::calculateMetadata() {
    double totalCarsInRange = 0.0;
    double totalXVelocity = 0.0;
    double totalYVelocity = 0.0;
    double totalVelocity = 0.0;
    double totalSpeed = 0.0;
    double totalAcceleration = 0.0;
    double totalDeceleration = 0.0;
    double totalXPosition = 0.0;
    double totalYPosition = 0.0;
    double totalTimestamp = 0.0;

    for (const auto& entry : table) {
        totalCarsInRange += entry.second.carsInRange;
        totalXVelocity += entry.second.xVelocity;
        totalYVelocity += entry.second.yVelocity;
        totalVelocity += entry.second.velocity;
        totalSpeed += entry.second.speed;
        totalAcceleration += entry.second.acceleration;
        totalDeceleration += entry.second.deceleration;
        totalXPosition += entry.second.xPosition;
        totalYPosition += entry.second.yPosition;
        totalTimestamp += entry.second.timestamp;

    }
    auto size = table.size();

    avgCarsInRange = totalCarsInRange / size;
    avgXVelocity = totalXVelocity / size;
    avgYVelocity = totalXVelocity / size;
    avgVelocity = totalVelocity / size;
    avgSpeed = totalSpeed / size;
    avgAcceleration = totalAcceleration / size;
    avgDeceleration = totalDeceleration / size;
    avgXPosition = totalXPosition / size;
    avgYPosition = totalYPosition / size;
    avgTimestamp = totalTimestamp / size;

    centricity = 0;
}

// Reset the table to empty
void NeighborTable::scoreNeighbors() {
    score.clear();
    for (const auto& entry : table) {
        score[entry.first] =
                carsInRangeWeight * (avgCarsInRange - entry.second.carsInRange) +
                xVelocityWeight * (avgXVelocity - entry.second.xVelocity) +
                yVelocityWeight * (avgYVelocity - entry.second.yVelocity) +
                velocityWeight * (avgVelocity - entry.second.velocity) +
                speedWeight * (avgSpeed - entry.second.speed) +
                accelerationWeight * (avgAcceleration - entry.second.acceleration) +
                decelerationWeight * (avgDeceleration - entry.second.deceleration) +
                xPositionWeight * (avgXPosition - entry.second.xPosition) +
                yPositionWeight * (avgYPosition - entry.second.yPosition) +
                timeWeight * (avgTimestamp - entry.second.timestamp);
    }
}

std::vector<double> NeighborTable::toList() {
    std::vector<double> values;
    values.push_back(avgCarsInRange);
    values.push_back(avgXVelocity);
    values.push_back(avgYVelocity);
    values.push_back(avgVelocity);
    values.push_back(avgSpeed);
    values.push_back(avgAcceleration);
    values.push_back(avgDeceleration);
    values.push_back(avgXPosition);
    values.push_back(avgYPosition);
    values.push_back(avgTimestamp);

    // Return the vector
    return values;
}

int NeighborTable::getSize() {
    return table.size();
}

std::string NeighborTable::getBestScoringNeighbor() {
    // Make sure the scores are up-to-date
    scoreNeighbors();

    // return closest neighbor to 0
    double closestScore = std::numeric_limits<double>::max();
    std::string closestEntry;

    for (const auto& entry : score) {
        double currentScore = std::abs(entry.second);
        if (currentScore < closestScore) {
            closestScore = currentScore;
            closestEntry = entry.first;
        }
    }

    return closestEntry;
}

std::vector<std::string> NeighborTable::getAllNeighbors() {
    std::vector<std::string> keys;
    keys.reserve(table.size());

    for (const auto& entry : table) {
        keys.push_back(entry.first);
    }

    return keys;
}

// Reset the table to empty
void NeighborTable::resetTable() {
    table.clear();
}

// Print the scores of each neighbor
void NeighborTable::printScores() {
    cout << "Neighbor Scores:" << endl;
    for (const auto& entry : score) {
        cout << "Name: " << entry.first << ", Score: " << entry.second << endl;
    }
}

// Print the calculated averages
void NeighborTable::printAverages() {
    cout << "Calculated Averages:" << endl;
    cout << "Average Cars in Range: " << avgCarsInRange << endl;
    cout << "Average X Velocity: " << avgXVelocity << endl;
    cout << "Average Y Velocity: " << avgYVelocity << endl;
    cout << "Average Velocity: " << avgVelocity << endl;
    cout << "Average Speed: " << avgSpeed << endl;
    cout << "Average Acceleration: " << avgAcceleration << endl;
    cout << "Average Deceleration: " << avgDeceleration << endl;
    cout << "Average X Position: " << avgXPosition << endl;
    cout << "Average Y Position: " << avgYPosition << endl;
    cout << "Centricity: " << centricity << endl;
}

// Print the table
void NeighborTable::printTable() {
    for (const auto& entry : table) {
        cout << "Name: " << entry.first << ", Speed: " << entry.second.speed
             << ", Cars in Range: " << entry.second.carsInRange
             << ", Velocity: " << entry.second.velocity
             << ", xVelocity: " << entry.second.xVelocity
             << ", yVelocity: " << entry.second.yVelocity
             << ", Acceleration: " << entry.second.acceleration
             << ", Deceleration: " << entry.second.deceleration
             << ", xPosition: " << entry.second.xPosition
             << ", yPosition: " << entry.second.yPosition
             << ", xDirection: " << entry.second.xDirection
             << ", Timestamp: " << entry.second.timestamp << endl;
    }
}
