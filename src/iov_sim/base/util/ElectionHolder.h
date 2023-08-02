/*
 * ElectionHolder.h
 *
 *  Created on: Jul 24, 2023
 *      Author: chandler
 */

#ifndef IOV_SIM_BASE_UTIL_ELECTIONHOLDER_H_
#define IOV_SIM_BASE_UTIL_ELECTIONHOLDER_H_

#include <string>
#include <sstream>

using namespace std;

class  ElectionHolder {
public:
    ElectionHolder(int carsInRange, double speed,
    double xVelocity, double yVelocity, double acceleration,
    double deceleration, double xPosition, double yPosition,
    double xDirection,double yDirection);
    ElectionHolder(std::string initString);
    ~ElectionHolder();

    void fromString(std::string initString);
    std::string toString();

private:
    int carsInRange;
    double speed;
    double xVelocity;
    double yVelocity;
    double acceleration;
    double deceleration;
    double xPosition;
    double yPosition;
    double xDirection;
    double yDirection;
};

#endif /* IOV_SIM_BASE_UTIL_NEIGHBORENTRY_H_ */
