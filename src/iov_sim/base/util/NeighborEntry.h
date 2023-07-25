/*
 * NeighborTable.h
 *
 *  Created on: Jul 24, 2023
 *      Author: chandler
 */

#ifndef IOV_SIM_BASE_UTIL_NEIGHBORENTRY_H_
#define IOV_SIM_BASE_UTIL_NEIGHBORENTRY_H_


using namespace std;

struct NeighborEntry {
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
    double timestamp;
};

#endif /* IOV_SIM_BASE_UTIL_NEIGHBORENTRY_H_ */
