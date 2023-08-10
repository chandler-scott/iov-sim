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

#include "iov_sim/base/python/BaseWrapper.h"

BaseWrapper::BaseWrapper()
    : wrapper(PythonWrapper::getInstance())
{

}


BaseWrapper::~BaseWrapper()
{
}

void BaseWrapper::loadStateDicts(PyObject *pClass, PyObject *pStateDict, PyObject *vStateDict)
{
    if (pClass)
    {
        PyObject *args = PyTuple_Pack(2, pStateDict, vStateDict);

        PyObject *pFunc = PyObject_GetAttrString(pClass, "load_state_dicts");
        if (pFunc)
        {
            PyObject *result = PyObject_CallObject(pFunc, args);
            if (result == nullptr)
            {
                PyErr_Print();
            }
            else
            {
                Py_DECREF(result);
            }
            Py_DECREF(pFunc); // Release the reference to the function
        }
        else
        {
            PyErr_Print();
        }

        Py_XDECREF(args); // Release the reference to the arguments tuple
    }
    else
    {
        PyErr_Print();
    }
}

std::pair<PyObject *, PyObject *> BaseWrapper::getStateDictsAsJson(PyObject *pClass)
{
    if (pClass)
    {
        PyObject *args = PyTuple_Pack(0);
        PyObject *pFunc = PyObject_GetAttrString(pClass, "state_dicts_to_json");

        PyObject *result = PyObject_CallObject(pFunc, args);
        if (result == nullptr)
        {
            PyErr_Print();
        }
        else
        {
            // Assuming the result is a tuple with two elements
            if (PyTuple_Check(result) && PyTuple_Size(result) == 2)
            {
                PyObject *firstElement = PyTuple_GetItem(result, 0);
                PyObject *secondElement = PyTuple_GetItem(result, 1);

                return std::make_pair(firstElement, secondElement);
            }
            else
            {
                Logger::error("1 Unexpected return value from Python function.", "BaseWrapper");
            }
        }
    }
    else
    {
        PyErr_Print();
    }
    return std::make_pair(nullptr, nullptr);
}

std::pair<PyObject *, PyObject *> BaseWrapper::getStateDictsFromJson(PyObject *pClass, PyObject *pBytes, PyObject *vBytes)
{

    if (pClass)
    {
        PyObject *args = PyTuple_Pack(2, pBytes, vBytes);

        if (!args)
        {
            PyErr_Print();
            return std::make_pair(nullptr, nullptr);
        }

        PyObject *pFunc = PyObject_GetAttrString(pClass, "json_to_state_dicts");
        if (!pFunc)
        {
            PyErr_Print();
            Py_DECREF(args); // Properly release the reference
            return std::make_pair(nullptr, nullptr);
        }
        PyObject *result = PyObject_CallObject(pFunc, args);
        Py_DECREF(args); // Release the reference to the args tuple

        if (result == nullptr)
        {
            PyErr_Print();
        }
        else
        {

            // Assuming the result is a tuple with two elements
            if (PyTuple_Check(result) && PyTuple_Size(result) == 2)
            {
                PyObject *firstElement = PyTuple_GetItem(result, 0);
                PyObject *secondElement = PyTuple_GetItem(result, 1);

                return std::make_pair(firstElement, secondElement);
            }
            else
            {
                Logger::error("2 Unexpected return value from Python function.", "BaseWrapper");
            }

            Py_DECREF(result); // Release the reference to the result tuple
        }

        Py_DECREF(pFunc); // Release the reference to the function object
    }
    else
    {
        PyErr_Print();
    }

    return std::make_pair(nullptr, nullptr);
}


const char* BaseWrapper::PyObjectToChar(PyObject* obj) {
    if (obj == nullptr) {
        return nullptr;
    }

    if (!PyUnicode_Check(obj)) {
        PyErr_SetString(PyExc_TypeError, "Input object must be a Unicode string");
        return nullptr;
    }

    return PyUnicode_AsUTF8(obj);
}

PyObject* BaseWrapper::CharToPyObject(const char* value) {
    if (value == nullptr) {
        Py_RETURN_NONE;
    }

    return PyUnicode_FromString(value);
}
