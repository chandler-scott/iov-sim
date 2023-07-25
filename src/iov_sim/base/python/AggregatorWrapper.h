/*
 * AggregatorWrapper.h
 *
 *  Created on: Jul 16, 2023
 *      Author: chandler
 */

#ifndef IOV_SIM_UTIL_AGGREGATORWRAPPER_H_
#define IOV_SIM_UTIL_AGGREGATORWRAPPER_H_

#include "iov_sim/base/python/PythonWrapper.h"
#include "iov_sim/base/python/BaseWrapper.h"

#include <unordered_map>
#include <string>

using namespace std;

class AggregatorWrapper : public BaseWrapper{
public:
    AggregatorWrapper();
    ~AggregatorWrapper();
    
    std::pair<PyObject*, PyObject*> getStateDictsAsJson();
    std::pair<PyObject *, PyObject *> getStateDictsFromJson(const char* pJson, const char* vJson);

    void saveStateDict(const std::string& policySave, const std::string& valueSave);
    void loadStateDict(const std::string& policyLoad, const std::string& valueLoad);

private:
    PyObject* pAggregator;
};

#endif /* IOV_SIM_UTIL_AGGREGATORWRAPPER_H_ */
