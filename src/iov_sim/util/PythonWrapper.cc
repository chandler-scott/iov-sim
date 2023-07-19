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
#include <unistd.h>
#include <iostream>

PythonWrapper PythonWrapper::instance;

using namespace std;

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
    std::cout << "Initializing Python interpreter!" << std::endl;
    Py_Initialize();

    char cwd[256];  // Buffer to hold the current working directory

     if (getcwd(cwd, sizeof(cwd)) != nullptr) {
         std::cout << "Current working directory: " << cwd << std::endl;
     } else {
         std::cout << "Failed to get the current working directory." << std::endl;
     }

}


void PythonWrapper::finalizePython() {
    std::cout << "Closing Python interpreter!" << std::endl;
    Py_Finalize();
}

void PythonWrapper::destroyInstance() {
    instance.~PythonWrapper();  // Destruct the instance
}


PyObject* PythonWrapper::runPythonFunction(PyObject* module, const char* functionName, const char* arg) {
    cout << "Running Python Function:" << endl;

    if (module) {
        PyObject* function = PyObject_GetAttrString(module, functionName);
        if (function && PyCallable_Check(function)) {
            PyObject* result = PyObject_CallObject(function, NULL);
            // Process the result or perform error handling
            Py_DECREF(function);
            return result;
        }
        else {
            // Handle function access error
        }
    }
    else {
        // Handle invalid module object
    }

    // Return nullptr in case of error
    return nullptr;
}
