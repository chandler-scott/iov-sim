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

#pragma once

#include "iov_sim/iov_sim.h"
#include "iov_sim/util/BaseApplicationLayer.h"
#include "iov_sim/messages/VehicleInitMessage_m.h"
#include "iov_sim/messages/ModelUpdateMessage_m.h"


using namespace veins;

namespace iov_sim {

/**
 * @brief
 * A tutorial demo for TraCI. When the car is stopped for longer than 10 seconds
 * it will send a message out to other cars containing the blocked road id.
 * Receiving cars will then trigger a reroute via TraCI.
 * When channel switching between SCH and CCH is enabled on the MAC, the message is
 * instead send out on a service channel following a Service Advertisement
 * on the CCH.
 *
 * @author Christoph Sommer : initial DemoApp
 * @author David Eckhoff : rewriting, moving functionality to DemoBaseApplLayer, adding WSA
 *
 */

class IOV_SIM_API ApplicationLayerTest : public iov_sim::BaseApplicationLayer {
public:
    void initialize(int stage) override;
    void finish() override;


protected:
    simtime_t lastDroveAt;
    bool sentMessage;
    int currentSubscribedServiceId;

protected:
    void onBSM(BaseFrame1609_4* bsm) override;
    // to handle when the application receives a data message from another car or RSU
    void onWSM(veins::BaseFrame1609_4* wsm) override;
    void onWSA(veins::DemoServiceAdvertisment* wsa) override;

    void handleSelfMsg(cMessage* msg) override;
    void handlePositionUpdate(cObject* obj) override;
};

} // namespace iov_sim
