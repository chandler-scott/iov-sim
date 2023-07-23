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

#include "iov_sim/util/SerializeUtil.h"
#include "iov_sim/util/PythonWrapper.h""
#include <numpy/arrayobject.h> // Include the NumPy header for C API
#include <iostream>
#include <Python.h>
#include <vector>


using namespace std;

// Helper function to convert a PyObject* to a string representation
std::string SerializeUtil::getPyObjectString(PyObject* obj)
{
    if (obj == nullptr)
        return "";

    PyObject* pString = PyObject_Repr(obj);
    if (pString) {
        const char* pStringStr = PyUnicode_AsUTF8(pString);
        std::string result(pStringStr);
        Py_DECREF(pString);
        return result;
    }

    PyErr_Print();
    return "";
}

std::string pythonBytesToString(PyObject* pyBytes) {
    std::string cppString;

    if (PyBytes_Check(pyBytes)) {
        char* bytesData = PyBytes_AsString(pyBytes);
        Py_ssize_t bytesSize = PyBytes_Size(pyBytes);
        cppString.assign(bytesData, bytesSize);
    }

    return cppString;
}

// Serialize the std::unordered_map<std::string, PyObject*> to a string
std::string SerializeUtil::serializeMapToString(const std::unordered_map<std::string, PyObject*>& map)
{
    std::string serializedString;

    std::cout << "serializing message..." << std::endl;

    for (const auto& entry : map) {

        std::string keyStr = entry.first;
        std::string valueStr = "placeholder";

        serializedString += keyStr + "\n" + valueStr + "\n|\n";

        std::cout << serializedString << std::endl;
    }

    return serializedString;
}

PyObject* SerializeUtil::getPyObjectFromString(const std::string& str)
{
    PyObject* pGlobals = PyDict_New();
    PyObject* pLocals = PyDict_New();
    PyObject* pResult = PyRun_String(str.c_str(), Py_eval_input, pGlobals, pLocals);

    if (!pResult) {
        PyErr_Print();
    }

    Py_XDECREF(pGlobals);
    Py_XDECREF(pLocals);
    return pResult;
}


std::unordered_map<std::string, std::string> SerializeUtil::parseStream(std::istream& stream) {
    std::unordered_map<std::string, std::string> keyValueMap;
    std::string line;
    std::string key;
    std::string value;

    while (std::getline(stream, line)) {
        if (line == "|") {
            // Found a delimiter, store the key-value pair in the map
            keyValueMap[key] = value;
            key.clear();
            value.clear();
        } else if (key.empty()) {
            // Store the line as the key
            key = line;
        } else {
            // Store the line as the value
            value = line;
        }
    }

    // If there is an unprocessed key-value pair at the end, store it in the map
    if (!key.empty() && !value.empty()) {
        keyValueMap[key] = value;
    }

    return keyValueMap;
}


// Deserialize the serialized string back to std::unordered_map<std::string, PyObject*>
std::unordered_map<std::string, PyObject*> SerializeUtil::deserializeMapFromString(const std::string& serializedString)
{
    PythonWrapper& wrapper = PythonWrapper::getInstance();
    std::cout << "deserializing message..." << std::endl;

    std::unordered_map<std::string, PyObject*> resultMap;
    std::istringstream stream(serializedString);

    auto c = parseStream(stream);
    std::cout << "parse stream worked.." << std::endl;


    std::unordered_map<std::string, PyObject*> result;

    for (const auto& kv : c) {
        std::cout << "loop" << std::endl;
        std::string key = kv.first;
        std::string serializedValue = kv.second;
        std::cout << key << ": " << serializedValue <<std::endl;

        // Call the deserialize method on the PythonWrapper to convert the string to PyObject*
        PyObject* deserializedValue = wrapper.deserializeStateDict(serializedValue);
        std::cout << key << ": " << deserializedValue <<std::endl;
        // Add the deserialized PyObject* to the resultMap with the corresponding key
        resultMap[key] = deserializedValue;
    }
    std::cout << "finished deserializing" << std::endl;

    return resultMap;
}
