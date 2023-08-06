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

#include "iov_sim/modules/applications/RSUApplication.h"

using namespace veins;
using namespace std;
using namespace iov_sim;

Define_Module(iov_sim::RSUApplication);

RSUApplication::RSUApplication()
    : aggregator() { }

void RSUApplication::initialize(int stage)
{
    BaseApplicationLayer::initialize(stage);

    if (stage == 0) {
        // add rsu
        policyLoad = par("policyLoad").stringValue();
        valueLoad = par("valueLoad").stringValue();
        policySave = par("policySave").stringValue();
        valueSave = par("valueSave").stringValue();

        if (par("loadModel").boolValue())
        {
            aggregator.loadStateDict(policyLoad, valueLoad);
        }
    }
    else if (stage == 1) {
        modelRequestMessage = new ModelRequest("Model Request");

        populateWSM(modelRequestMessage);

        modelRequestMessage->setData("model request");
        modelRequestMessage->setOrigin("rsu");

        // sendDelayedDown(modelRequestMessage->dup(), 10 + uniform(0.01, 0.2));
    }
}

void RSUApplication::finish()
{
    BaseApplicationLayer::finish();

    if (par("saveModel").boolValue())
    {
        aggregator.saveStateDict(policySave, valueSave);
    }

    delete modelRequestMessage;
}

void RSUApplication::onModelMsg(BaseMessage* msg)
{
   if (ModelRequest* requestMsg = dynamic_cast<ModelRequest*>(msg)) {
        // RSU has received an initialize message from a vehicle -> they need an
        // updated model. Respond with ModelUpdateMessage (message containing
        // up-to-date AI model)
        Logger::info("received a Model Request Message", nodeName);

        findHost()->getDisplayString().setTagArg("i", 1, "green");
        auto origin = requestMsg->getOrigin();

        if (std::strcmp(origin, "rsu") != 0)
        {
            Logger::info("-- request from vehicle. sending...", nodeName);
            sendModelUpdateMessage(origin);
        }
    }
   else { }
}

void RSUApplication::sendModelUpdateMessage(const char* destination)
{
    // get state_dicts as json-formatted PyObject string
    auto jsonStateDicts = aggregator.getStateDictsAsJson();

    // convert PyObject to const char*
    auto pNet = aggregator.PyObjectToChar(jsonStateDicts.first);
    auto vNet = aggregator.PyObjectToChar(jsonStateDicts.second);

    // send message
    BaseApplicationLayer::sendModelUpdateMessage(destination, pNet, vNet);
    Logger::info("-- up-to-date model sent!", nodeName);
}
