/*
 * Logger.h
 *
 *  Created on: Jul 25, 2023
 *      Author: chandler
 */


#ifndef IOV_SIM_BASE_UTIL_LOGGER_H_
#define IOV_SIM_BASE_UTIL_LOGGER_H_

#include <iostream>
#include <iomanip>
#include <fstream>
#include <omnetpp.h> // Include OMNeT++ header for simtime_t

using namespace omnetpp;

class Logger {
public:
    // Function to set the output stream.
    static void setOutputStream(std::ostream& outputStream);

    // Function to log an INFO message with an OMNeT++ timestamp and nodeName.
    static void info(const std::string& message, const std::string& nodeName);

    // Function to log an ERROR message with an OMNeT++ timestamp and nodeName.
    static void error(const std::string& errorMessage, const std::string& nodeName);

private:
    // Private constructor and destructor to prevent instantiation.
    Logger();
    ~Logger();

    // Output stream for logging.
    static std::ostream* outputStream;
};

#endif /* IOV_SIM_BASE_UTIL_LOGGER_H_ */
