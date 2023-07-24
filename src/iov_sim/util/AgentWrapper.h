/*
 * AgentWrapper.h
 *
 *  Created on: Jul 22, 2023
 *      Author: Chandler Scott
 */

#ifndef IOV_SIM_UTIL_AGENTWRAPPER_H_
#define IOV_SIM_UTIL_AGENTWRAPPER_H_

#include "iov_sim/util/PythonWrapper.h"
#include "iov_sim/util/BaseWrapper.h"
#include <unordered_map>
#include <string>

using namespace std;

class AgentWrapper : public BaseWrapper
{
public:
    AgentWrapper();
    ~AgentWrapper();

    void loadStateDicts(PyObject *pStateDict, PyObject *vStateDict);

    std::pair<PyObject *, PyObject *> getStateDictsAsJson();
    std::pair<PyObject *, PyObject *> getStateDictsFromJson(const char* pJson, const char* vJson);

    void step();
    void learn();

    void bufferStoreTransition();
    void bufferFinishPath();


private:
    PyObject *pAgent;
};

#endif /* IOV_SIM_UTIL_AGENTWRAPPER_H_ */
