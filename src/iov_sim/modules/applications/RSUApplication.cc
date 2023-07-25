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
        EV_INFO << "RSUAggregationApplication::Initializing RSUAggregationApplication: Stage 0" << endl;
        policyLoad = par("policyLoad").stringValue();
        valueLoad = par("valueLoad").stringValue();
        policySave = par("policySave").stringValue();
        valueSave = par("valueSave").stringValue();

        aggregator.loadStateDict(policyLoad, valueLoad);
    }
    else if (stage == 1) {
        ModelRequestMessage* msg = new ModelRequestMessage();
        populateWSM(msg);
        msg->setData("Hi there!");
        msg->setOrigin("rsu");
        sendDelayedDown(msg->dup(), 10 + uniform(0.01, 0.2));
        delete msg;
    }
}

void RSUApplication::finish()
{
    BaseApplicationLayer::finish();
    aggregator.saveStateDict(policySave, valueSave);
}

void RSUApplication::onBSM(BaseFrame1609_4* bsm)
{
    if (ModelRequestMessage* requestMsg = dynamic_cast<ModelRequestMessage*>(bsm)) {
        // RSU has received an initialize message from a vehicle -> they need an
        // updated model. Respond with ModelUpdateMessage (message containing
        // up-to-date AI model)
        std::cout << "RSU: received a Model Request Message" << std::endl;

        findHost()->getDisplayString().setTagArg("i", 1, "blue");
        auto origin = requestMsg->getOrigin();

        if (std::strcmp(origin, "vehicle") == 0)
        {
            std::cout << "-- request from vehicle. sending..." << std::endl;
            sendModelUpdateMessage();
        }
        else {
            std::cout << "-- ignoring rsu beacon" << std::endl;
        }
    }
    else {
        std::cout << "RSU: received a different beacon message than expected.." << std::endl;
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
            std::cout << "RSU: received a Model Update Message" << std::endl;
            ModelUpdateMessage* appMessage = check_and_cast<ModelUpdateMessage*>(frame);

            auto pNetJson = appMessage->getPStateDict();
            auto vNetJson = appMessage->getVStateDict();
            auto origin = appMessage->getOrigin();

            if (std::strcmp(origin, "vehicle") == 0)
            {
                std::cout << "-- request from vehicle" << std::endl;
                // parse into state_dicts
                auto [pStateDict, vStateDict] = aggregator.getStateDictsFromJson(pNetJson, vNetJson);

                // aggregate here

                // send reply
                sendModelUpdateMessage();
            }
            else {
                std::cout << "-- ignoring rsu message" << std::endl;
            }
        }
        else {
            delete frame;
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
    std::cout << "-- up-to-date model sent!" << std::endl;

}
