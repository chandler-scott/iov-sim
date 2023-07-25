/*
 * NeighborTable.cpp
 *
 *  Created on: Jul 24, 2023
 *      Author: chandler
 */

#include "NeighborTable.h"

// Constructor
NeighborTable::NeighborTable(double defaultTimeout) : timeout(defaultTimeout) {}

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
    double totalXVelocity = 0.0;
    double totalYVelocity = 0.0;
    double totalVelocity = 0.0;
    double totalSpeed = 0.0;
    double totalAcceleration = 0.0;
    double totalDeceleration = 0.0;
    double totalXPosition = 0.0;
    double totalYPosition = 0.0;

    for (const auto& entry : table) {
        totalXVelocity += entry.second.xVelocity;
        totalYVelocity += entry.second.yVelocity;
        totalVelocity += entry.second.velocity;
        totalSpeed += entry.second.speed;
        totalAcceleration += entry.second.acceleration;
        totalDeceleration += entry.second.deceleration;
        totalXPosition += entry.second.xPosition;
        totalYPosition += entry.second.yPosition;
    }
    auto size = table.size();

    avgXVelocity = totalXVelocity / size;
    avgYVelocity = totalXVelocity / size;
    avgVelocity = totalVelocity / size;
    avgSpeed = totalSpeed / size;
    avgAcceleration = totalAcceleration / size;
    avgDeceleration = totalDeceleration / size;
    avgXPosition = totalXPosition / size;
    avgYPosition = totalYPosition / size;
    centricity = 0;
}


// Reset the table to empty
void NeighborTable::scoreNeighbors() {
    calculateMetadata();
    score.clear();

    for (const auto& entry : table) {
        score[entry.first] = 1;
    }
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
