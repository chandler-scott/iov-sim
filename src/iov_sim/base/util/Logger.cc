/*
 * Logger.cc
 *
 *  Created on: Jul 25, 2023
 *      Author: chandler
 */

#include "Logger.h"

// Initialize the static member.
std::ostream* Logger::outputStream = &std::cout; // Default to std::cout

// Function to set the output stream.
void Logger::setOutputStream(std::ostream& outputStream) {
    Logger::outputStream = &outputStream;
}

// Function to log an INFO message with an OMNeT++ timestamp and nodeName.
void Logger::info(const std::string& message, const std::string& nodeName)
{
    // Convert the simtime to seconds with three decimal places
    simtime_t timestamp = simTime();
    double timestampInSeconds = timestamp.dbl();

    // Set fixed widths for timestamp and node name fields (adjust as needed)
    int timestampWidth = 8; // For example, use 10 characters for the timestamp field
    int nodeNameWidth = 10;  // For example, use 15 characters for the node name field

    // Print to standard output
    (*outputStream) << "[" << std::fixed << std::setprecision(2) << std::setw(timestampWidth) << timestampInSeconds << "s] "
                    << "[" << std::setw(nodeNameWidth) << nodeName << "] [INFO] " << message << std::endl;

    // Print to OMNeT++ Event Log using the EV macro
    EV << "[" << std::fixed << std::setprecision(3) << std::setw(timestampWidth) << timestampInSeconds << "s] "
       << "[" << std::setw(nodeNameWidth) << nodeName << "] [INFO] " << message << std::endl;
}

// Function to log an ERROR message with an OMNeT++ timestamp and nodeName.
void Logger::error(const std::string& errorMessage,  const std::string& nodeName)
{
    // Convert the simtime to seconds with three decimal places
    simtime_t timestamp = simTime();
    double timestampInSeconds = timestamp.dbl();

    // Set fixed widths for timestamp and node name fields (adjust as needed)
    int timestampWidth = 10; // For example, use 10 characters for the timestamp field
    int nodeNameWidth = 15;  // For example, use 15 characters for the node name field

    // Print to standard error
    (*outputStream) << "[" << std::fixed << std::setprecision(3) << std::setw(timestampWidth) << timestampInSeconds << "s] "
                    << "[" << std::setw(nodeNameWidth) << nodeName << "] [ERROR] " << errorMessage << std::endl;

    // Print to OMNeT++ Event Log using the EV macro
    EV_ERROR << "[" << std::fixed << std::setprecision(3) << std::setw(timestampWidth) << timestampInSeconds << "s] "
             << "[" << std::setw(nodeNameWidth) << nodeName << "] [ERROR] " << errorMessage << std::endl;
}
