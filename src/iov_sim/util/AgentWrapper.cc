//
// Copyright (C) 2006-2011 Christoph Sommer <christoph.sommer@uibk.ac.at>
//
// Documentation for these modules is at http://veins.car2x.org/
//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "iov_sim/util/AgentWrapper.h"
#include <iostream>
#include <Python.h>

AgentWrapper::AgentWrapper()
    : wrapper(PythonWrapper::getInstance())
{

    // Access the Aggregator class from the Python module
    PyObject* pModule = wrapper.ppoModule;
    PyObject* pClass = PyObject_GetAttrString(pModule, "Agent");
    std::cout << "Initialized the Agent Wrapper Class!\n"
              << "--now trying to init Agent class" << std::endl;

    PyObject* obsDim = wrapper.callZerosBoxSpace(obs_size);
    PyObject* actDim = wrapper.callZerosBoxSpace(act_size);

    PyObject* args = PyTuple_Pack(2, obsDim, actDim);
    // // Create an instance of the Aggregator class
    try {
        pAgent = PyObject_CallObject(pClass, args);
        Py_DECREF(args);

        // Check for exceptions raised during the function call
        if (pAgent == nullptr) {
            // Handle the exception here
            PyErr_Print();
            std::cerr << "Error creating Agent instance!" << std::endl;
        }
        else {
            std::cout << "Successfully initialized the Agent!" << std::endl;
        }
    }
    catch (const std::exception& e) {
        // Handle any C++ exceptions that might occur during the PyObject_CallObject call
        std::cerr << "Caught C++ exception: " << e.what() << std::endl;
    }
    catch (...) {
        // Handle any other unexpected C++ exceptions
        std::cerr << "Caught an unexpected C++ exception!" << std::endl;
    }
}

void AgentWrapper::loadStateDicts(std::unordered_map<std::string, PyObject*> stateDicts)
{
    if (pAgent) {
        auto pStateDict = stateDicts["p_net"];
        auto vStateDict = stateDicts["v_net"];
        PyObject* args = PyTuple_Pack(2, pStateDict, vStateDict);

        PyObject* pFunc = PyObject_GetAttrString(pAgent, "load_state_dicts");
        PyObject* result = PyObject_CallObject(pFunc, args);
        if (result == nullptr) {
            PyErr_Print();
        }
        else {
            Py_DECREF(result);
            std::cout << "state dict loaded!" << std::endl;
        }
    }
    else {
        PyErr_Print();
    }
}

std::pair<PyObject*, PyObject*> AgentWrapper::getStateDictsAsBytes()
{
    std::unordered_map<std::string, PyObject*> stateDictsBytes;

    if (pAgent) {
        PyObject* args = PyTuple_Pack(0);
        PyObject* pFunc = PyObject_GetAttrString(pAgent, "state_dicts_to_bytes");

        PyObject* result = PyObject_CallObject(pFunc, args);
        if (result == nullptr) {
            PyErr_Print();
        }
        else {
            // Assuming the result is a tuple with two elements
            if (PyTuple_Check(result) && PyTuple_Size(result) == 2) {
                PyObject* firstElement = PyTuple_GetItem(result, 0);
                PyObject* secondElement = PyTuple_GetItem(result, 1);

                if (PyBytes_Check(firstElement)) {
                    // debug check
                    std::cout << "ok1" << std::endl;
                }
                if (PyBytes_Check(secondElement)) {
                    // debug check
                    std::cout << "ok" << std::endl;
                }

                return std::make_pair(firstElement, secondElement);

            }
            else {
                std::cerr << "Error: Unexpected return value from Python function." << std::endl;
            }
        }
    }
    else {
        PyErr_Print();
    }

}

std::unordered_map<std::string, PyObject*> AgentWrapper::getStateDictsFromBytes(PyObject* pBytes, PyObject* vBytes)
{
    std::unordered_map<std::string, PyObject*> stateDicts;

    if (pAgent) {
        std::cout << "V11" << std::endl;
        PyObject* args = PyTuple_Pack(2, pBytes, vBytes);
        std::cout << "V102" << std::endl;

        if (!args) {
            PyErr_Print();
            return stateDicts; // Return an empty map to signal the error
        }

        std::cout << "V12" << std::endl;
        PyObject* pFunc = PyObject_GetAttrString(pAgent, "json_to_state_dicts");
        if (!pFunc) {
            PyErr_Print();
            Py_DECREF(args); // Properly release the reference
            return stateDicts; // Return an empty map to signal the error
        }
        std::cout << "V13" << std::endl;
        PyObject* result = PyObject_CallObject(pFunc, args);
        Py_DECREF(args); // Release the reference to the args tuple

        if (result == nullptr) {
            PyErr_Print();
        }
        else {
            std::cout << "V14" << std::endl;

            // Assuming the result is a tuple with two elements
            if (PyTuple_Check(result) && PyTuple_Size(result) == 2) {
                PyObject* firstElement = PyTuple_GetItem(result, 0);
                PyObject* secondElement = PyTuple_GetItem(result, 1);

                // Add elements to the unordered_map
                stateDicts["p_net"] = firstElement;
                stateDicts["v_net"] = secondElement;
            }
            else {
                std::cerr << "Error: Unexpected return value from Python function." << std::endl;
            }

            Py_DECREF(result); // Release the reference to the result tuple
        }

        Py_DECREF(pFunc); // Release the reference to the function object
    }
    else {
        PyErr_Print();
    }

    return stateDicts;
}

PyObject* AgentWrapper::stringToPyDict(const char *jsonString)
{
       PyObject* jsonModule = PyImport_ImportModule("json");
       if (jsonModule == NULL) {
           PyErr_SetString(PyExc_ImportError, "Failed to import 'json' module.");
           return NULL;
       }

       PyObject* jsonLoadsFunc = PyObject_GetAttrString(jsonModule, "loads");
       if (jsonLoadsFunc == NULL) {
           Py_DECREF(jsonModule);
           PyErr_SetString(PyExc_AttributeError, "Failed to get 'loads' function from 'json' module.");
           return NULL;
       }

       PyObject* pyJsonString = PyUnicode_FromString(jsonString);
       if (pyJsonString == NULL) {
           Py_DECREF(jsonModule);
           Py_DECREF(jsonLoadsFunc);
           PyErr_SetString(PyExc_TypeError, "Failed to convert C-string to PyObject.");
           return NULL;
       }

       PyObject* pyDict = PyObject_CallFunctionObjArgs(jsonLoadsFunc, pyJsonString, NULL);
       if (pyDict == NULL) {
           Py_DECREF(jsonModule);
           Py_DECREF(jsonLoadsFunc);
           Py_DECREF(pyJsonString);
           PyErr_SetString(PyExc_RuntimeError, "Failed to convert JSON string to Python dictionary.");
           return NULL;
       }

       Py_DECREF(jsonModule);
       Py_DECREF(jsonLoadsFunc);
       Py_DECREF(pyJsonString);

       return pyDict;
}


void AgentWrapper::step()
{
}

void AgentWrapper::learn()
{
    if (pAgent) {
        PyObject* args = PyTuple_Pack(0);
        PyObject* pFunc = PyObject_GetAttrString(pAgent, "learn");

        PyObject* result = PyObject_CallObject(pFunc, args);
        if (result == nullptr) {
            PyErr_Print();
        }
        else {
            Py_DECREF(result);
            std::cout << "agent successfully learned!" << std::endl;
        }
    }
    else {
        PyErr_Print();
    }
}

void AgentWrapper::bufferStoreTransition()
{
}

void AgentWrapper::bufferFinishPath()
{
    if (pAgent) {
        PyObject* pBuffer = PyObject_GetAttrString(pAgent, "buffer");

        PyObject* args = PyTuple_Pack(0);
        PyObject* pFunc = PyObject_GetAttrString(pBuffer, "finish_path");

        PyObject* result = PyObject_CallObject(pFunc, args);
        if (result == nullptr) {
            PyErr_Print();
        }
        else {
            Py_DECREF(result);
            std::cout << "Finish path successful." << std::endl;
        }
    }
    else {
        PyErr_Print();
    }
}

AgentWrapper::~AgentWrapper()
{
    Py_DECREF(pAgent);
}
