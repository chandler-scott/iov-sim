/*
 * PythonWrapper.h
 *
 *  Created on: Jul 16, 2023
 *      Author: chandler
 */

#ifndef IOV_SIM_UTIL_PYTHONWRAPPER_H_
#define IOV_SIM_UTIL_PYTHONWRAPPER_H_

#include <Python.h>

class PythonWrapper {
public:
    static PythonWrapper& getInstance();

    PythonWrapper(const PythonWrapper&) = delete;
    PythonWrapper& operator=(const PythonWrapper&) = delete;

    void runPythonFunction(const char* moduleName, const char* functionName, const char* arg = "");

private:
    PythonWrapper();
    ~PythonWrapper();

    void initializePython();
    void finalizePython();

    static PythonWrapper instance;
};


#endif /* IOV_SIM_UTIL_PYTHONWRAPPER_H_ */
