/*
 * PythonWrapper.h
 *
 *  Created on: Jul 16, 2023
 *      Author: chandler
 */

#ifndef IOV_SIM_UTIL_AGGREGATORWRAPPER_H_
#define IOV_SIM_UTIL_AGGREGATORWRAPPER_H_

#include "iov_sim/util/PythonWrapper.h"

class AggregatorWrapper {
public:
    AggregatorWrapper();

private:
    ~AggregatorWrapper();

    PyObject* pModule;
    PyObject* pClass;
    PythonWrapper& wrapper;

};

#endif /* IOV_SIM_UTIL_PYTHONWRAPPER_H_ */
