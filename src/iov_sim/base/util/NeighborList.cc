/*
 * NeighborList.cc
 *
 *  Created on: Aug 1, 2023
 *      Author: chandler
 */

#include "NeighborList.h"

NeighborList::NeighborList(double defaultTimeout) : timeout(defaultTimeout)
{}

NeighborList::~NeighborList() {
    // TODO Auto-generated destructor stub
}

// Method to add an entry if it is unique
void NeighborList::addNeighbor(std::string id, double timestamp) {
    neighborMap[id] = timestamp;
}

// Method to prune entries from the list that have a time < simTime() - timeout
void NeighborList::pruneList(double currentTime) {
    auto it = neighborMap.begin();
    while (it != neighborMap.end()) {
        if (it->second < currentTime - timeout) {
            it = neighborMap.erase(it);
        } else {
            ++it;
        }
    }
}

// Method to print the list with a nice format
void NeighborList::printList() {
    std::cout << "Neighbor List:" << std::endl;
    std::cout << std::left << std::setw(20) << "ID" << "Timestamp" << std::endl;
    for (const auto& entry : neighborMap) {
        std::cout << std::left << std::setw(20) << entry.first << entry.second << std::endl;
    }
}
