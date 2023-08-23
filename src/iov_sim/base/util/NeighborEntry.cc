/*
 * NeighborList.cc
 *
 *  Created on: Aug 3, 2023
 *      Author: chandler
 */

#include "NeighborEntry.h"
#include <sstream>

// Constructors
NeighborEntry::NeighborEntry() : carsInRange(0), speed(0), xVelocity(0), yVelocity(0),
                                 acceleration(0), deceleration(0), xPosition(0), yPosition(0),
                                 xDirection(0), yDirection(0), timestamp(0) {}

NeighborEntry::NeighborEntry(int carsInRange, double speed, double xVelocity, double yVelocity,
                             double acceleration, double deceleration, double xPosition, double yPosition,
                             double xDirection, double yDirection, double timestamp)
    : carsInRange(carsInRange), speed(speed), xVelocity(xVelocity),
      yVelocity(yVelocity), acceleration(acceleration), deceleration(deceleration),
      xPosition(xPosition), yPosition(yPosition), xDirection(xDirection), yDirection(yDirection),
      timestamp(timestamp), signalStrength(signalStrength) {}

NeighborEntry::NeighborEntry(const std::string& str) {
    *this = fromString(str);
}


// Getter methods
int NeighborEntry::getCarsInRange() const { return carsInRange; }
double NeighborEntry::getSpeed() const { return speed; }
// Add other getters for the rest of the data members if needed.

// Setter methods
void NeighborEntry::setCarsInRange(int cars) { carsInRange = cars; }
void NeighborEntry::setSpeed(double spd) { speed = spd; }
void NeighborEntry::setSignalStrength(double signalStr) { signalStrength = signalStr; }

// Add other setters for the rest of the data members if needed.

// ToString method: converts object to a string representation
std::string NeighborEntry::toString() const {
    std::ostringstream oss;
    oss << carsInRange << "," << speed << "," << signalStrength << ","
        << xVelocity << "," << yVelocity << "," << acceleration << ","
        << deceleration << "," << xPosition << "," << yPosition << ","
        << xDirection << "," << yDirection << "," << timestamp;
    return oss.str();
}

// FromString method: converts string representation to an object
NeighborEntry NeighborEntry::fromString(const std::string& str) {
    NeighborEntry entry;
    std::istringstream iss(str);
    char comma; // To consume commas from the string

    iss >> entry.carsInRange >> comma >> entry.speed
        >> comma >> entry.signalStrength >> comma >> entry.xVelocity
        >> comma >> entry.yVelocity >> comma >> entry.acceleration
        >> comma >> entry.deceleration >> comma >> entry.xPosition
        >> comma >> entry.yPosition >> comma >> entry.xDirection
        >> comma >> entry.yDirection >> comma >> entry.timestamp;

    return entry;
}
