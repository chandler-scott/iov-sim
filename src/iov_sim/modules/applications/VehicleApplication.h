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
#include "iov_sim/base/applications/BaseApplicationLayer.h"
#include "iov_sim/base/python/AgentWrapper.h"
#include "iov_sim/base/util/NeighborTable.h"
#include "iov_sim/base/util/NeighborEntry.h"
#include "iov_sim/modules/messages/NeighborTablePruneMessage_m.h"
#include "iov_sim/modules/messages/ClusterElectionMessage_m.h"
#include "iov_sim/modules/messages/ClusterSelectionMessage_m.h"



#include <cmath>


using namespace veins;

namespace iov_sim {


class IOV_SIM_API VehicleApplication : public iov_sim::BaseApplicationLayer {
public:
    VehicleApplication();

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

    void sendClusterBeaconMessage();
    void sendModelUpdateMessage();
    void loadModelUpdate(ModelUpdateMessage* appMessage);

    void handleSelfMsg(cMessage* msg) override;
    void handlePositionUpdate(cObject* obj) override;

    void addSelfToNeighborTable();

private:
    AgentWrapper agent;
    ModelRequestMessage *modelRequestMessage = nullptr;
    NeighborTablePruneMessage *neighborTablePruneMessage = nullptr;
    ClusterBeaconMessage *clusterBeaconSelfMessage = nullptr;
    ClusterBeaconMessage *clusterBeaconMessage = nullptr;
    ClusterElectionMessage *clusterElectionMessage = nullptr;
    ClusterSelectionMessage *clusterSelectionMessage = nullptr;
    VehicleClusterDataMessage *vehClusterDataMessage = nullptr;
    RSUClusterDataMessage *rsuClusterDataMessage = nullptr;


    NeighborTable neighborTable;
    int clusterBeaconDelay;
    double neighborTableTimeout;

    // duration for election
    double clusterElectionDuration;

    // timeout for nodes to buy into election
    double clusterElectionTimeout;

    // timeout for cluster head to accept role
    double clusterHeadTimeout;

    // threshold for cluster head to call for new election
    double clusterReelectionThreshold;

    std::string clusterHeadId;
    bool clusterHead;
    bool clusterMember;
};

} // namespace iov_sim
