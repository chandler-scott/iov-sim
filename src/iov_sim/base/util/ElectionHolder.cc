/*
 * ElectionHolder.cc
 *
 *  Created on: Jul 24, 2023
 *      Author: chandler
 */

#include "ElectionHolder.h"

// Constructor with initialization parameters
ElectionHolder::ElectionHolder(int carsInRange, double speed,
    double xVelocity, double yVelocity, double acceleration,
    double deceleration, double xPosition, double yPosition,
    double xDirection, double yDirection)
    : carsInRange(carsInRange), speed(speed), xVelocity(xVelocity), yVelocity(yVelocity),
      acceleration(acceleration), deceleration(deceleration), xPosition(xPosition),
      yPosition(yPosition), xDirection(xDirection), yDirection(yDirection) {}

// Constructor to create an instance from a string representation
ElectionHolder::ElectionHolder(std::string initString) {
    fromString(initString);
}

// Destructor
ElectionHolder::~ElectionHolder() {}

// Parse the initialization string and set object properties
void ElectionHolder::fromString(std::string initString) {
    std::stringstream ss(initString);
    std::string token;

    // Split the string using the delimiter ';'
    while (std::getline(ss, token, ';')) {
        // Split each key-value pair using the delimiter '='
        size_t pos = token.find('=');
        if (pos != std::string::npos) {
            std::string key = token.substr(0, pos);
            std::string value = token.substr(pos + 1);

            // Parse and set the object properties based on the keys
            if (key == "carsInRange") {
                carsInRange = std::stoi(value);
            } else if (key == "speed") {
                speed = std::stod(value);
            } else if (key == "xVelocity") {
                xVelocity = std::stod(value);
            } else if (key == "yVelocity") {
                yVelocity = std::stod(value);
            } else if (key == "acceleration") {
                acceleration = std::stod(value);
            } else if (key == "deceleration") {
                deceleration = std::stod(value);
            } else if (key == "xPosition") {
                xPosition = std::stod(value);
            } else if (key == "yPosition") {
                yPosition = std::stod(value);
            } else if (key == "xDirection") {
                xDirection = std::stod(value);
            } else if (key == "yDirection") {
                yDirection = std::stod(value);
            }
        }
    }
}

// Generate a string representation of the object
std::string ElectionHolder::toString() {
    std::stringstream ss;
    // Concatenate object properties with delimiters to form a string
    ss << "carsInRange=" << carsInRange << ";";
    ss << "speed=" << speed << ";";
    ss << "xVelocity=" << xVelocity << ";";
    ss << "yVelocity=" << yVelocity << ";";
    ss << "acceleration=" << acceleration << ";";
    ss << "deceleration=" << deceleration << ";";
    ss << "xPosition=" << xPosition << ";";
    ss << "yPosition=" << yPosition << ";";
    ss << "xDirection=" << xDirection << ";";
    ss << "yDirection=" << yDirection << ";";

    return ss.str();
}
