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

#include "iov_sim/applications/RSUApplication.h"


using namespace veins;
using namespace iov_sim;

Define_Module(iov_sim::RSUApplication);

void RSUApplication::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        EV_INFO << "RSUAggregationApplication::Initializing RSUAggregationApplication: Stage 0" << endl;
        PythonWrapper& wrapper = PythonWrapper::getInstance();

    }
}

void RSUApplication::onBSM(DemoSafetyMessage* bsm)
{
    std::cout << "RSUApplication: onBSM" << std::endl;

    DemoSafetyMessage* wsm = check_and_cast<DemoSafetyMessage*>(bsm);

    std::cout << wsm << endl;

    // Your application has received a beacon message from another car or RSU
    // respond with RSUInitMessage
    RSUInitMessage* msg = new RSUInitMessage();
    populateWSM(msg);
    msg->setData("A crafty beacon response!");

    // this rsu repeats the received traffic update in 2 seconds plus some random delay
    sendDelayedDown(msg->dup(), 2 + uniform(0.01, 0.2));
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
    std::cout << "RSUApplication: onWSM!" << std::endl;
    // Your application has received a data message from another car or RSU


    VehicleInitMessage* appMessage = check_and_cast<VehicleInitMessage*>(frame);

    std::cout << appMessage->getData() << std::endl;

    RSUInitMessage* msg = new RSUInitMessage();
    populateWSM(msg);
    msg->setData("A crafty response!");

    // this rsu repeats the received traffic update in 2 seconds plus some random delay
    sendDelayedDown(msg->dup(), 2 + uniform(0.01, 0.2));
}
