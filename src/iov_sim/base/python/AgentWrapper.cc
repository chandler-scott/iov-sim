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
#include <Python.h>

AgentWrapper::AgentWrapper() : BaseWrapper()
{
    // Access the Aggregator class from the Python module
    PyObject *pModule = wrapper.ppoModule;
    PyObject *pClass = PyObject_GetAttrString(pModule, "Agent");
    std::cout << "Initialized the Agent Wrapper Class!\n"
              << "--now trying to init Agent class" << std::endl;

    PyObject *obsDim = wrapper.callZerosBoxSpace(obs_size);
    PyObject *actDim = wrapper.callZerosBoxSpace(act_size);

    PyObject *args = PyTuple_Pack(2, obsDim, actDim);
    // // Create an instance of the Aggregator class
    try
    {
        pAgent = PyObject_CallObject(pClass, args);
        Py_DECREF(args);

        // Check for exceptions raised during the function call
        if (pAgent == nullptr)
        {
            // Handle the exception here
            PyErr_Print();
            std::cerr << "Error creating Agent instance!" << std::endl;
        }
        else
        {
        }
    }
    catch (const std::exception &e)
    {
        // Handle any C++ exceptions that might occur during the PyObject_CallObject call
        std::cerr << "Caught C++ exception: " << e.what() << std::endl;
    }
    catch (...)
    {
        // Handle any other unexpected C++ exceptions
        std::cerr << "Caught an unexpected C++ exception!" << std::endl;
    }
}


AgentWrapper::~AgentWrapper()
{
    Py_DECREF(pAgent);
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

void AgentWrapper::step()
{
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

void AgentWrapper::bufferStoreTransition()
{
}

void AgentWrapper::bufferFinishPath()
{
    if (pAgent)
    {
        PyObject *pBuffer = PyObject_GetAttrString(pAgent, "buffer");

        PyObject *args = PyTuple_Pack(0);
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
