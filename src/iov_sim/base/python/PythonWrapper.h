/*
 * PythonWrapper.h
 *
 *  Created on: Jul 16, 2023
 *      Author: chandler
 */

#ifndef IOV_SIM_UTIL_PYTHONWRAPPER_H_
#define IOV_SIM_UTIL_PYTHONWRAPPER_H_

#include <Python.h>
#include "iov_sim/base/util/Logger.h"
#include <iostream>
#include <vector>


using namespace std;

class PythonWrapper {
public:
    static PythonWrapper& getInstance();
    static void destroyInstance();

    void printPyObjectType(PyObject* pyObject);


    PythonWrapper(const PythonWrapper&) = delete;
    PythonWrapper& operator=(const PythonWrapper&) = delete;

    std::vector<double> callToDoublesList(PyObject* pyObject);
    PyObject* callToTensor(const std::vector<double>& inputList, double lower_bound = -1, double upper_bound = 1);
    PyObject* callZerosBoxSpace(int box_size, double lower_bound = -1, double upper_bound = 1);
    void loadPythonModule(const char* moduleName, PyObject*& module);

    std::string serializeStateDict(PyObject* stateDictObject);
    PyObject* deserializeStateDict(std::string stateDictString);

    PyObject* ppoModule;
    PyObject* utilModule;

private:
    PythonWrapper();
    ~PythonWrapper();

    void initializePython();
    void finalizePython();

    static PythonWrapper instance;
};

#endif /* IOV_SIM_UTIL_PYTHONWRAPPER_H_ */
