/*
 * AggregatorWrapper.h
 *
 *  Created on: Jul 16, 2023
 *      Author: chandler
 */

#ifndef IOV_SIM_UTIL_SERIALIZEUTIL_H_
#define IOV_SIM_UTIL_SERIALIZEUTIL_H_

#include <Python.h>
#include <string>
#include <unordered_map>
#include <sstream>

using namespace std;

class SerializeUtil {
public:
    // Helper function to convert a PyObject* to a string representation
    static std::string getPyObjectString(PyObject* obj);

    // Serialize the std::unordered_map<std::string, PyObject*> to a string
    static std::string serializeMapToString(const std::unordered_map<std::string, PyObject*>& map);



    // Helper function to convert a string to a PyObject*
    static PyObject* getPyObjectFromString(const std::string& str);

    static std::unordered_map<std::string, std::string> parseStream(std::istream& stream);


    // Deserialize the serialized string back to std::unordered_map<std::string, PyObject*>
    static std::unordered_map<std::string, PyObject*> deserializeMapFromString(const std::string& serializedString);
};

#endif
