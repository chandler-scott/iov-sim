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

#include "iov_sim/applications/ApplicationLayerTest.h"
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"
#include "veins/modules/messages/BaseFrame1609_4_m.h"

using namespace veins;
using namespace iov_sim;

Define_Module(iov_sim::ApplicationLayerTest);

ApplicationLayerTest::ApplicationLayerTest ()
     : agent() { }

void ApplicationLayerTest::initialize(int stage)
{
    BaseApplicationLayer::initialize(stage);
    if (stage == 0) {
        // Initializing members and pointers of your application goes here
        EV << "Initializing " << par("appName").stringValue() << std::endl;
    }
    else if (stage == 1) {
        // Initializing members that require initialized other modules goes here
        std::cout << "Vehicle: sending init message" << std::endl;
        VehicleInitMessage* wsm = new VehicleInitMessage();
        populateWSM(wsm);
        wsm->setData("Hi there!");
        sendDown(wsm);
    }
}

void ApplicationLayerTest::finish()
{
    BaseApplicationLayer::finish();
    // statistics recording goes here
}

void ApplicationLayerTest::onBSM(BaseFrame1609_4* bsm)
{
    std::cout << "Vehicle: successfully received a beacon message" << std::endl;

    // Your application has received a beacon message from another car or RSU
    // code for handling the message goes here

    if (bsm) {
        if (dynamic_cast<VehicleInitMessage*>(bsm)) {
            // ignore... unless we have the model
            std::cout << "--was a vehicle init message..." << std::endl;
        }
        else {
            // Handle other message types here...

            // If none of the expected types match, you can still delete the message here.
            // However, if you use check_and_cast, it already handles the deletion for you
            // when the cast fails, so you don't need to delete the message manually.
            delete bsm;
        }
    }
}

void ApplicationLayerTest::onWSM(BaseFrame1609_4* frame)
{
    std::cout << "Vehicle: successfully received a data message" << std::endl;

    if (frame) {
        if (dynamic_cast<ModelUpdateMessage*>(frame)) {
            std::cout << "-- data message is a model update" << std::endl;

            // Handle AnotherMessageType
            ModelUpdateMessage* appMessage = check_and_cast<ModelUpdateMessage*>(frame);
            findHost()->getDisplayString().setTagArg("i", 1, "green");

            auto pNetJson = appMessage->getPStateDict();
            auto vNetJson = appMessage->getVStateDict();

            auto pNet = agent.CharToPyObject(pNetJson);
            auto vNet = agent.CharToPyObject(vNetJson);

            auto [pStateDict, vStateDict] = agent.getStateDictsFromJson(pNetJson, vNetJson);

            agent.loadStateDicts(pStateDict, vStateDict);


            std::cout << "-- loaded most up-to-date model" << std::endl;


        }
        else {
            // Handle other message types here...

            // If none of the expected types match, you can still delete the message here.
            // However, if you use check_and_cast, it already handles the deletion for you
            // when the cast fails, so you don't need to delete the message manually.
            delete frame;
        }
    }
}

void ApplicationLayerTest::onWSA(DemoServiceAdvertisment* wsa)
{
    std::cout << "ApplicationLayerTest: omWSA" << std::endl;
    // Your application has received a service advertisement from another car or RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
}

void ApplicationLayerTest::handleSelfMsg(cMessage* msg)
{
    BaseApplicationLayer::handleSelfMsg(msg);
    // this method is for self messages (mostly timers)
    // it is important to call the BaseApplLayer function for BSM and WSM transmission
}

void ApplicationLayerTest::handlePositionUpdate(cObject* obj)
{
    BaseApplicationLayer::handlePositionUpdate(obj);

    // the vehicle has moved. Code that reacts to new positions goes here.
    // member variables such as currentPosition and currentSpeed are updated in the parent class
}
