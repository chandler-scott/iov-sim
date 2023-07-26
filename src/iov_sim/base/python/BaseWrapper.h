/*
 * BaseWrapper.h
 *
 *  Created on: Jul 24, 2023
 *      Author: Chandler Scott
 */
#ifndef IOV_SIM_UTIL_BASEWRAPPER_H_
#define IOV_SIM_UTIL_BASEWRAPPER_H_

#include "iov_sim/base/python/PythonWrapper.h"
#include "iov_sim/base/util/Logger.h"

#include <unordered_map>
#include <string>

using namespace std;

class BaseWrapper
{
public:
    BaseWrapper();
    ~BaseWrapper();

    void loadStateDicts(PyObject *pClass, PyObject *pStateDict, PyObject *vStateDict);

    virtual std::pair<PyObject *, PyObject *> getStateDictsAsJson(PyObject *pClass);
    virtual std::pair<PyObject *, PyObject *> getStateDictsFromJson(PyObject *pClass, PyObject *pBytes, PyObject *vBytes);


    const char *PyObjectToChar(PyObject *obj);
    PyObject* CharToPyObject(const char* json_str);


protected:
    PythonWrapper &wrapper;

    // neural network parameters
    int obs_size = 4;
    int act_size = 3;
    double lower_bound = -1.0;
    double upper_bound = 1.0;
};

#endif /* IOV_SIM_UTIL_AGENTWRAPPER_H_ */
