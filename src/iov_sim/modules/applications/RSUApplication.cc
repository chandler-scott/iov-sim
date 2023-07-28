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

        aggregator.loadStateDict(policyLoad, valueLoad);
    }
    else if (stage == 1) {
        modelRequestMessage = new ModelRequest();

        populateWSM(modelRequestMessage);

        modelRequestMessage->setData("model request");
        modelRequestMessage->setOrigin("rsu");

        sendDelayedDown(modelRequestMessage->dup(), 10 + uniform(0.01, 0.2));
    }
}

void RSUApplication::finish()
{
    BaseApplicationLayer::finish();
    aggregator.saveStateDict(policySave, valueSave);

    delete modelRequestMessage;
}

void RSUApplication::onBSM(BaseFrame1609_4* bsm)
{
   if (ModelRequest* requestMsg = dynamic_cast<ModelRequest*>(bsm)) {
        // RSU has received an initialize message from a vehicle -> they need an
        // updated model. Respond with ModelUpdateMessage (message containing
        // up-to-date AI model)
        Logger::info("received a Model Request Message", nodeName);

        findHost()->getDisplayString().setTagArg("i", 1, "blue");
        auto origin = requestMsg->getOrigin();

        if (std::strcmp(origin, "vehicle") == 0)
        {
            Logger::info("-- request from vehicle. sending...", nodeName);
            sendModelUpdateMessage();
        }
        else {
            Logger::info("-- ignoring rsu beacon", nodeName);
        }
    }
    else {
        Logger::info("received an unknown beacon message type..", nodeName);
    }
}

void RSUApplication::onWSA(DemoServiceAdvertisment* wsa)
{
    // if this RSU receives a WSA for service 42, it will tune to the chan
    if (wsa->getPsid() == 42) {
        mac->changeServiceChannel(static_cast<Channel>(wsa->getTargetChannel()));
    }
}

void RSUApplication::onWSM(BaseFrame1609_4* frame)
{
    if (frame) {
        if (dynamic_cast<ModelUpdate*>(frame)) {
            Logger::info("received a Model Update Message", nodeName);
            ModelUpdate* appMessage = check_and_cast<ModelUpdate*>(frame);

            auto pNetJson = appMessage->getPStateDict();
            auto vNetJson = appMessage->getVStateDict();
            auto origin = appMessage->getOrigin();

            if (std::strcmp(origin, "vehicle") == 0)
            {
                Logger::info("-- request from vehicle", nodeName);
                // parse into state_dicts
                auto [pStateDict, vStateDict] = aggregator.getStateDictsFromJson(pNetJson, vNetJson);

                // aggregate here

                // send reply
                sendModelUpdateMessage();
            }
            else {
                Logger::info("-- ignoring rsu message", nodeName);
            }
        }
        else {
            Logger::info("!!!", nodeName);
        }
    }
}

void RSUApplication::sendModelUpdateMessage()
{
    // get state_dicts as json-formatted PyObject string
    auto [pJson, vJson] = aggregator.getStateDictsAsJson();

    // convert PyObject to const char*
    auto pNet = aggregator.PyObjectToChar(pJson);
    auto vNet = aggregator.PyObjectToChar(vJson);

    // send message
    BaseApplicationLayer::sendModelUpdateMessage(pNet, vNet);
    Logger::info("-- up-to-date model sent!", nodeName);
}
