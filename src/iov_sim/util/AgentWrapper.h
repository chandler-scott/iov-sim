/*
 * AgentWrapper.h
 *
 *  Created on: Jul 22, 2023
 *      Author: Chandler Scott
 */

#ifndef IOV_SIM_UTIL_AGENTWRAPPER_H_
#define IOV_SIM_UTIL_AGENTWRAPPER_H_

#include "iov_sim/util/PythonWrapper.h"
#include <unordered_map>
#include <string>

using namespace std;

class AgentWrapper {
public:
    AgentWrapper();
    ~AgentWrapper();

    void loadStateDicts(std::unordered_map<std::string, PyObject*> stateDicts);

    std::pair<PyObject*, PyObject*> getStateDictsAsBytes();
    std::unordered_map<std::string, PyObject*> getStateDictsFromBytes(PyObject* pBytes, PyObject* vBytes);

    PyObject* stringToPyDict(const char *value);

    void step();
    void learn();

    void bufferStoreTransition();
    void bufferFinishPath();

private:
    PyObject* pModule;
    PyObject* pAgent;
    PyObject* pClass;
    PythonWrapper& wrapper;

    // neural network parameters
    int obs_size = 4;
    int act_size = 3;
    double lower_bound = -1.0;
    double upper_bound = 1.0;
};

#endif /* IOV_SIM_UTIL_AGENTWRAPPER_H_ */
