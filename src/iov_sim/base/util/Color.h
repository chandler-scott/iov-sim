

/*
 * Color.h
 *
 *  Created on: Aug 3, 2023
 *      Author: chandler
 */

#ifndef IOV_SIM_BASE_UTIL_COLOR_H_
#define IOV_SIM_BASE_UTIL_COLOR_H_

#include <string>

using namespace std;

class Color {
public:
    enum Value
    {
        MODEL_RCV, ELECTION_SRC, ELECTION_RETRY_SRC,
        ELECTION_PARTICIPANT, CLUSTER_HEAD, CLUSTER_MEMBER
    };

    Color(Value val) : value(val) {}

    std::string toString() const {
        switch (value) {
            case MODEL_RCV:   return "#FFC107";
            case ELECTION_SRC: return "#25e407";
            case ELECTION_RETRY_SRC:  return "#1c4c15";
            case ELECTION_PARTICIPANT:  return "#2196F3";
            case CLUSTER_HEAD:  return "#9C27B0";
            case CLUSTER_MEMBER:  return "#FF5722";
            default:    return "";
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const Color& color) {
        os << color.toString();
        return os;
    }

    Value getValue() const {
        return value;
    }

private:
    Value value;
};
#endif /* IOV_SIM_BASE_UTIL_COLOR_H_ */
