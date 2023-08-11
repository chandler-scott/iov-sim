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

#include "iov_sim/base/python/AggregatorWrapper.h"
#include <iostream>

using namespace std;

AggregatorWrapper::AggregatorWrapper() : BaseWrapper()
{

    // Access the Aggregator class from the Python module
    PyObject* pModule = wrapper.ppoModule;
    PyObject* pClass = PyObject_GetAttrString(pModule, "Aggregator");
    Logger::info("Initialized the Aggregator Wrapper Class!", "AggregatorWrapper");
    Logger::info("-- now trying to init Aggregator class", "AggregatorWrapper");


    PyObject* obsDim = wrapper.callZerosBoxSpace(obs_size);
    PyObject* actDim = wrapper.callZerosBoxSpace(act_size);

    PyObject* args = PyTuple_Pack(2, obsDim, actDim);
    // // Create an instance of the Aggregator class
    try {
        pAggregator = PyObject_CallObject(pClass, args);
        Py_DECREF(args);

        // Check for exceptions raised during the function call
        if (pAggregator == nullptr) {
            // Handle the exception here
            PyErr_Print();
            Logger::error("Error creating Aggregator instance!", "AggregatorWrapper");
        }
        else {
            Logger::info("Successfully initialized the Aggregator!", "AggregatorWrapper");
        }
    }
    catch (const std::exception& e) {
        // Handle any C++ exceptions that might occur during the PyObject_CallObject call
        string ex = "Caught C++ exception: ";
        ex +=  e.what();
        Logger::error(ex, "AgentWrapper");    }
    catch (...) {
        // Handle any other unexpected C++ exceptions
        Logger::error("Caught an unexpected C++ exception!", "AggregatorWrapper");
    }
}

std::pair<PyObject*, PyObject*> AggregatorWrapper::aggregateModels(PyObject* pNets, PyObject* vNets)
{
    if (pAggregator)
            {
                PyObject *args = PyTuple_Pack(2, pNets, vNets);
                PyObject *pFunc = PyObject_GetAttrString(pAggregator, "aggregate");


                PyObject *result = PyObject_CallObject(pFunc, args);
                if (result == nullptr)
                {
                    PyErr_Print();
                }
                else
                {
                    if (PyTuple_Check(result) && PyTuple_Size(result) == 2)
                    {
                        PyObject *firstElement = PyTuple_GetItem(result, 0);
                        PyObject *secondElement = PyTuple_GetItem(result, 1);

                        return std::make_pair(firstElement, secondElement);
                    }
                }
            }
            else
            {
                PyErr_Print();
            }
    return std::make_pair(nullptr, nullptr);

}

std::pair<PyObject*, PyObject*> AggregatorWrapper::getStateDictsAsJson()
{
    if (pAggregator)
    {
        return BaseWrapper::getStateDictsAsJson(pAggregator);
    }
    else {
        PyErr_Print();
    }  

    return std::make_pair(nullptr, nullptr);
}

std::pair<PyObject *, PyObject *> AggregatorWrapper::getStateDictsFromJson(const char* pJson, const char* vJson)
{
    if (pAggregator)
    {
        auto pNet = BaseWrapper::CharToPyObject(pJson);
        auto vNet = BaseWrapper::CharToPyObject(vJson);

        return BaseWrapper::getStateDictsFromJson(pAggregator, pNet, vNet);
    }
    else
    {
        PyErr_Print();
    }

    return std::make_pair(nullptr, nullptr);
}


void AggregatorWrapper::saveStateDict(const std::string& policySave, const std::string& valueSave)
{
    if (pAggregator) {
        // Convert C++ strings to Python objects (PyObjects)
        PyObject* pSaveArgObj = PyUnicode_FromString(policySave.c_str());
        PyObject* vSaveArgObj = PyUnicode_FromString(valueSave.c_str());

        if (pSaveArgObj == nullptr || vSaveArgObj == nullptr) {
            // Error handling: Failed to convert C++ strings to Python objects
            PyErr_Print();
            // Don't proceed with the method call if conversion failed
            return;
        }

        PyObject* args = PyTuple_Pack(2, pSaveArgObj, vSaveArgObj);

        // Use the state_dict() method to get the state dictionaries of the neural networks
        PyObject* pFunc = PyObject_GetAttrString(pAggregator, "save_aggregate");
        PyObject* result = PyObject_CallObject(pFunc, args);

        if (result == nullptr) {

            PyErr_Print();
        }
        else {
            Py_DECREF(result);
            Logger::info("model saved!", "AggregatorWrapper");
        }
    }
    else {
        PyErr_Print();
    }
}

void AggregatorWrapper::loadStateDict(const std::string& policyLoad, const std::string& valueLoad)
{
    if (pAggregator) {
        PyObject* pLoadArgObj = PyUnicode_FromString(policyLoad.c_str());
        PyObject* vLoadArgObj = PyUnicode_FromString(valueLoad.c_str());

        if (pLoadArgObj == nullptr || vLoadArgObj == nullptr) {
            // Error handling: Failed to convert C++ strings to Python objects
            PyErr_Print();
            // Don't proceed with the method call if conversion failed
            return;
        }

        PyObject* args = PyTuple_Pack(2, pLoadArgObj, vLoadArgObj);

        // Use the state_dict() method to get the state dictionaries of the neural networks
        PyObject* pFunc = PyObject_GetAttrString(pAggregator, "load");

        PyObject* result = PyObject_CallObject(pFunc, args);
        if (result == nullptr) {

            PyErr_Print();
        }
        else {
            Py_DECREF(result);
            Logger::info("model loaded!", "AggregatorWrapper");
        }
    }
    else {
        PyErr_Print();
    }
}

AggregatorWrapper::~AggregatorWrapper()
{

    Py_DECREF(pAggregator);
}
