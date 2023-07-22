/*
 * AggregatorWrapper.h
 *
 *  Created on: Jul 16, 2023
 *      Author: chandler
 */

#ifndef IOV_SIM_UTIL_AGGREGATORWRAPPER_H_
#define IOV_SIM_UTIL_AGGREGATORWRAPPER_H_

#include "iov_sim/util/PythonWrapper.h"
#include <unordered_map>
#include <string>

using namespace std;

class AggregatorWrapper {
public:
    AggregatorWrapper();
    ~AggregatorWrapper();
    std::unordered_map<std::string, PyObject*> getStateDicts();
    void saveStateDict(const std::string& policySave, const std::string& valueSave);
    void loadStateDict(const std::string& policyLoad, const std::string& valueLoad);



private:
    PyObject* pModule;
    PyObject* pAggregator;
    PyObject* pClass;
    PythonWrapper& wrapper;

    // neural network parameters
    int obs_size = 4;
    int act_size = 3;
    double lower_bound = -1.0;
    double upper_bound = 1.0;
};

#endif /* IOV_SIM_UTIL_AGGREGATORWRAPPER_H_ */
