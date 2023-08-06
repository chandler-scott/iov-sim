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

#include "iov_sim/base/python/AgentWrapper.h"
#include <iostream>

AgentWrapper::AgentWrapper() : BaseWrapper()
{ }


AgentWrapper::~AgentWrapper()
{
    Py_DECREF(pAgent);
}

void AgentWrapper::initializeAgent()
{
    // Access the Aggregator class from the Python module
    PyObject *pModule = wrapper.ppoModule;
    PyObject *pClass = PyObject_GetAttrString(pModule, "Agent");

    PyObject *obsDim = wrapper.callZerosBoxSpace(obs_size);
    PyObject *actDim = wrapper.callZerosBoxSpace(act_size);
    PyObject *localSteps = PyLong_FromLong(localStepsPerEpoch);

    PyObject *args = PyTuple_Pack(3, obsDim, actDim, localSteps);
    // // Create an instance of the Aggregator class

    pAgent = PyObject_CallObject(pClass, args);
    Py_DECREF(args);

    // Check for exceptions raised during the function call
    if (pAgent == nullptr)
    {
        // Handle the exception here
        PyErr_Print();
        Logger::error("Error creating Agent instance!", "AgentWrapper");
    }
    else
    {
        Logger::info("-- Agent successfully initialized!", "AgentWrapper");
    }}

int AgentWrapper::getLocalStepsPerEpoch()
{
    return localStepsPerEpoch;
}

void AgentWrapper::setLocalStepsPerEpoch(int value)
{
    localStepsPerEpoch = value;
}

PyObject* AgentWrapper::toPyFloat(double value)
{
    return PyFloat_FromDouble(value);
}


PyObject* AgentWrapper::toTensor(std::vector<double> list)
{
    return wrapper.callToTensor(list);
}

std::vector<double> AgentWrapper::toDoublesList(PyObject* pyObject)
{
    return wrapper.callToDoublesList(pyObject);
}

void AgentWrapper::loadStateDicts(PyObject *pStateDict, PyObject *vStateDict)
{
    if (pAgent)
    {
        PyObject *args = PyTuple_Pack(2, pStateDict, vStateDict);

        PyObject *pFunc = PyObject_GetAttrString(pAgent, "load_state_dicts");
        PyObject *result = PyObject_CallObject(pFunc, args);
        if (result == nullptr)
        {
            PyErr_Print();
        }
        else
        {
            Py_DECREF(result);
        }
    }
    else
    {
        PyErr_Print();
    }
}

std::pair<PyObject *, PyObject *> AgentWrapper::getStateDictsAsJson()
{
    if (pAgent)
    {
        return BaseWrapper::getStateDictsAsJson(pAgent);
    }
    else
    {
        PyErr_Print();
    }

    return std::make_pair(nullptr, nullptr);
}

std::pair<PyObject *, PyObject *> AgentWrapper::getStateDictsFromJson(const char* pJson, const char* vJson)
{

    if (pAgent)
    {
        auto pNet = BaseWrapper::CharToPyObject(pJson);
        auto vNet = BaseWrapper::CharToPyObject(vJson);

        return BaseWrapper::getStateDictsFromJson(pAgent, pNet, vNet);
    }
    else
    {
        PyErr_Print();
    }

    return std::make_pair(nullptr, nullptr);
}

std::tuple<PyObject *, PyObject *, PyObject *> AgentWrapper::step(PyObject* observation)
{
    if (pAgent)
    {
        PyObject *args = PyTuple_Pack(1, observation);
        PyObject *pFunc = PyObject_GetAttrString(pAgent, "step");

        PyObject *result = PyObject_CallObject(pFunc, args);
        if (result == nullptr)
        {
            PyErr_Print();
        }
        else
        {
            // Assuming the result is a tuple with two elements
            if (PyTuple_Check(result) && PyTuple_Size(result) == 3)
            {
                PyObject *action = PyTuple_GetItem(result, 0);
                PyObject *value = PyTuple_GetItem(result, 1);
                PyObject *logp = PyTuple_GetItem(result, 2);


                return std::make_tuple(action, value, logp);
            }
            else
            {
                Logger::error("3 Unexpected return value from Python function.", "BaseWrapper");
            }
            Py_DECREF(result);
        }
    }
    else
    {
        PyErr_Print();
    }
    return std::make_tuple(nullptr, nullptr, nullptr);

}

void AgentWrapper::learn()
{
    if (pAgent)
    {
        PyObject *args = PyTuple_Pack(0);
        PyObject *pFunc = PyObject_GetAttrString(pAgent, "learn");

        PyObject *result = PyObject_CallObject(pFunc, args);
        if (result == nullptr)
        {
            PyErr_Print();
        }
        else
        {
            Py_DECREF(result);
        }
    }
    else
    {
        PyErr_Print();
    }
}

void AgentWrapper::bufferStoreTransition(PyObject* observation, PyObject* action, PyObject* reward,
        PyObject* value, PyObject* logp)
{
    if (pAgent)
    {
        PyObject *pBuffer = PyObject_GetAttrString(pAgent, "buffer");

        PyObject *args = PyTuple_Pack(5, observation, action, reward, value, logp);
        PyObject *pFunc = PyObject_GetAttrString(pBuffer, "store");

        PyObject *result = PyObject_CallObject(pFunc, args);
        if (result == nullptr)
        {
            PyErr_Print();
        }
        else
        {
            Py_DECREF(result);
        }
    }
    else
    {
        PyErr_Print();
    }
}

void AgentWrapper::bufferFinishPath(PyObject* lastValue)
{
    if (pAgent)
    {
        PyObject *pBuffer = PyObject_GetAttrString(pAgent, "buffer");

        PyObject *args = PyTuple_Pack(1, lastValue);
        PyObject *pFunc = PyObject_GetAttrString(pBuffer, "finish_path");

        PyObject *result = PyObject_CallObject(pFunc, args);
        if (result == nullptr)
        {
            PyErr_Print();
        }
        else
        {
            Py_DECREF(result);
        }
    }
    else
    {
        PyErr_Print();
    }
}
