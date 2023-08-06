/*
 * NeighborTable.h
 *
 *  Created on: Jul 24, 2023
 *      Author: chandler
 */

#ifndef NEIGHBORENTRY_H_
#define NEIGHBORENTRY_H_

#include <string>

class NeighborEntry {
private:


public:
    // Constructors
    NeighborEntry();
    NeighborEntry(int carsInRange, double speed, double velocity, double xVelocity, double yVelocity,
                  double acceleration, double deceleration, double xPosition, double yPosition,
                  double xDirection, double yDirection, double timestamp);
    explicit NeighborEntry(const std::string& str);

    // Getter methods
    int getCarsInRange() const;
    double getSpeed() const;
    double getVelocity() const;
    // Add other getters for the rest of the data members if needed.

    // Setter methods
    void setCarsInRange(int cars);
    void setSpeed(double spd);
    void setVelocity(double vel);
    // Add other setters for the rest of the data members if needed.

    // ToString method: converts object to a string representation
    std::string toString() const;

    // FromString method: converts string representation to an object
    static NeighborEntry fromString(const std::string& str);

    double timestamp;
    int carsInRange;
    double speed;
    double velocity;
    double xVelocity;
    double yVelocity;
    double acceleration;
    double deceleration;
    double xPosition;
    double yPosition;
    double xDirection;
    double yDirection;

};

#endif /* NEIGHBORENTRY_H_ */
