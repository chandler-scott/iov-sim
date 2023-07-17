//
// Copyright (C) 2016 David Eckhoff <david.eckhoff@fau.de>
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

#include "iov_sim/applications/RSUAggregationApplication.h"
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"

using namespace veins;
using namespace iov_sim;

Define_Module(iov_sim::RSUAggregationApplication);

void RSUAggregationApplication::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        EV_INFO << "RSUAggregationApplication::Initializing RSUAggregationApplication: Stage 0" << endl;
        PythonWrapper& wrapper = PythonWrapper::getInstance();

        //PyObject *pModule = PyImport_ImportModule("ppo");
        //Py_DECREF(pModule);

           // Check if the module was imported successfully
        /*
       if (pModule != NULL)
           {
               // Get the 'get_hello' function from the module
               PyObject *pFunction = PyObject_GetAttrString(pModule, "get_hello");

               // Check if the function was retrieved successfully
               if (pFunction != NULL && PyCallable_Check(pFunction))
               {
                   // Call the 'get_hello' function
                   PyObject *pResult = PyObject_CallObject(pFunction, NULL);

                   // Check if the function call was successful
                   if (pResult != NULL)
                   {
                       // Convert the result to a C string
                       const char *helloStr = PyUnicode_AsUTF8(pResult);
                       std::cout << "Received: " << helloStr << std::endl;

                       // Release the result object
                       Py_DECREF(pResult);
                   }

                   // Release the function object
                   Py_DECREF(pFunction);
               }

               // Release the module object
           }
           else
           {
               std::cout << "PPO module not found." << endl;
           }
           */
    }
}

void RSUAggregationApplication::onWSA(DemoServiceAdvertisment* wsa)
{
    // if this RSU receives a WSA for service 42, it will tune to the chan
    if (wsa->getPsid() == 42) {
        mac->changeServiceChannel(static_cast<Channel>(wsa->getTargetChannel()));
    }
}

void RSUAggregationApplication::onWSM(BaseFrame1609_4* frame)
{
    TraCIDemo11pMessage* wsm = check_and_cast<TraCIDemo11pMessage*>(frame);

    // this rsu repeats the received traffic update in 2 seconds plus some random delay
    sendDelayedDown(wsm->dup(), 2 + uniform(0.01, 0.2));
}
