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

#include "iov_sim/util/PythonWrapper.h"

PythonWrapper PythonWrapper::instance;

PythonWrapper& PythonWrapper::getInstance() {
    return instance;
}

PythonWrapper::PythonWrapper() {
    initializePython();
}

PythonWrapper::~PythonWrapper() {
    finalizePython();
}

void PythonWrapper::initializePython() {
    Py_Initialize();
    // Set up Python system path and import modules as needed
}

void PythonWrapper::finalizePython() {
    Py_Finalize();
}

void PythonWrapper::runPythonFunction(const char* moduleName, const char* functionName, const char* arg) {
    PyObject* module = PyImport_ImportModule(moduleName);
    if (module) {
        PyObject* function = PyObject_GetAttrString(module, functionName);
        if (function && PyCallable_Check(function)) {
            PyObject* result = PyObject_CallObject(function, NULL);
            // Process the result or perform error handling
            Py_XDECREF(result);
            Py_DECREF(function);
        }
        else {
            // Handle function access error
        }
        Py_DECREF(module);
    }
    else {
        // Handle module import error
    }
}
