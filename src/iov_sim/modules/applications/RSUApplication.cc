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
#include "iov_sim/modules/messages/ModelUpdateMessage_m.h"

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
        modelRequestMessage = new ModelRequestMessage();
        rsuClusterDataMessage = new RSUClusterDataMessage("ClusterDataMessage");

        populateWSM(modelRequestMessage);
        populateWSM(rsuClusterDataMessage);

        modelRequestMessage->setData("model request");
        modelRequestMessage->setOrigin("rsu");

        sendDelayedDown(modelRequestMessage->dup(), 10 + uniform(0.01, 0.2));
    }
}

void RSUApplication::finish()
{
    BaseApplicationLayer::finish();
    aggregator.saveStateDict(policySave, valueSave);

    delete rsuClusterDataMessage;
    delete modelRequestMessage;
}

void RSUApplication::onBSM(BaseFrame1609_4* bsm)
{
   if (ModelRequestMessage* requestMsg = dynamic_cast<ModelRequestMessage*>(bsm)) {
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
    else if (ClusterSelectionMessage* requestMsg = dynamic_cast<ClusterSelectionMessage*>(bsm)) {
        Logger::info("received a cluster selection message", nodeName);
        auto clusterHeadId = requestMsg->getData();
        /*

        rsuClusterDataMessage->setOrigin(nodeName);
        rsuClusterDataMessage->setDestination(clusterHeadId);

        sendDelayedDown(rsuClusterDataMessage->dup(), simTime()+1);
        auto log = std::string("-- sending data message to ") + clusterHeadId;
        Logger::info(log, nodeName);
        */
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
        if (dynamic_cast<ModelUpdateMessage*>(frame)) {
            Logger::info("received a Model Update Message", nodeName);
            ModelUpdateMessage* appMessage = check_and_cast<ModelUpdateMessage*>(frame);

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
        } else if (dynamic_cast<RSUClusterDataMessage*>(frame)) {
            Logger::info("received a Cluster Data Message", nodeName);
            RSUClusterDataMessage* appMessage = check_and_cast<RSUClusterDataMessage*>(frame);

            auto destination = appMessage->getDestination();

            if (std::strcmp(destination, nodeName) == 0)
            {
                Logger::info("-- its for me!", nodeName);
            }
            else {
                Logger::info("-- not for me...", nodeName);
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
