/*
 * NeighborList.h
 *
 *  Created on: Aug 1, 2023
 *      Author: chandler
 */

#ifndef IOV_SIM_BASE_UTIL_NEIGHBORLIST_H_
#define IOV_SIM_BASE_UTIL_NEIGHBORLIST_H_

#include <string>
#include <unordered_map>
#include <iostream>
#include <iomanip>


using namespace std;

class NeighborList {
public:
    explicit NeighborList(double defaultTimeout = 30.0);
    virtual ~NeighborList();

private:
    std::unordered_map<std::string, double> neighborMap;
    double timeout;
public:
    void addNeighbor(std::string id, double timestamp);
    void pruneList(double currentTime);
    void printList();
};

#endif /* IOV_SIM_BASE_UTIL_NEIGHBORLIST_H_ */
