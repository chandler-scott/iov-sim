/*
 * AgentWrapper.h
 *
 *  Created on: Jul 22, 2023
 *      Author: Chandler Scott
 */

#ifndef IOV_SIM_UTIL_AGENTWRAPPER_H_
#define IOV_SIM_UTIL_AGENTWRAPPER_H_

#include "iov_sim/base/python/PythonWrapper.h"
#include "iov_sim/base/python/BaseWrapper.h"
#include <unordered_map>
#include <string>
#include <tuple>

using namespace std;

class AgentWrapper : public BaseWrapper
{
public:
    AgentWrapper();
    ~AgentWrapper();

    void initializeAgent();


    int getLocalStepsPerEpoch();
    void setLocalStepsPerEpoch(int value);

    void loadStateDicts(PyObject *pStateDict, PyObject *vStateDict);

    PyObject* toTensor(std::vector<double> list);
    PyObject* toPyFloat(double value);
    std::vector<double> toDoublesList(PyObject* pyObject);

    std::pair<PyObject *, PyObject *> getStateDicts();
    std::pair<PyObject *, PyObject *> getStateDictsAsJson();
    std::pair<PyObject *, PyObject *> getStateDictsFromJson(const char* pJson, const char* vJson);

    std::tuple<PyObject *, PyObject *, PyObject *> step(PyObject* observation);
    void learn();

    void bufferStoreTransition(PyObject* observation, PyObject* action, PyObject* reward,
            PyObject* value, PyObject* logp);
    void bufferFinishPath(PyObject* lastValue);



private:
    PyObject *pAgent;
    int localStepsPerEpoch;
};

#endif /* IOV_SIM_UTIL_AGENTWRAPPER_H_ */
