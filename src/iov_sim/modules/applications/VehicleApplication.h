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
#include "iov_sim/base/util/ClusterTable.h"
#include "iov_sim/base/util/NeighborList.h"
#include "iov_sim/base/util/NeighborEntry.h"
#include "iov_sim/base/util/ElectionHolder.h"

#include <cmath>
#include <algorithm> // Added for std::find

/* *
 *
 *  COLOR SCHEME
 *  received model: #FFC107 (Amber)
 *  CH election start source: #25e407 (Light Green)
 *  CH election retry source: #1c4c15 (Dark Green)
 *  CH election participant: #2196F3 (Blue)
 *  Cluster head: #9C27B0 (Purple)
 *  Cluster member: #FF5722 (Orange)
 * */



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
    void onModelMsg(BaseMessage* msg) override;
    void onElectionMsg(BaseMessage* msg) override;
    void onClusterMsg(BaseMessage* msg) override;
    void onNeighborMsg(BaseMessage* msg) override;


    void sendModelUpdateMsg();
    void sendModelRequestMsg();
    void sendNeighborBeacon();

    void sendElectionMsg();
    void sendElectionAck(Election* msg);
    void sendLeaderMsg(const char* leaderElected);

    double calculateConnectivityPercentage();

    void loadModelUpdate(ModelUpdate* appMessage);
    void addSelfToNeighborTable();

    void leaveCluster();
    void leaveElection();

    void handleElection(const char* winnerId);
    void handleObservation();
    void handleLearn();
    void handleSelfMsg(cMessage* msg) override;

    void step();
    void observe();
    void learn();



private:
    AgentWrapper agent;

    /* @brief Timeout for node to receive reply for Ack message */
    Timeout *ackTimeout = nullptr;
    /* @brief Timeout for node to receive reply for Election message */
    Timeout *electionTimeout = nullptr;
    /* @brief Timeout for node to receive reply for Model Request message */
    Timeout *modelRequestTimeout = nullptr;
    /* @brief Timeout for node to receive reply for Cluster Heartbeat message */
    Timeout *clusterHeartbeatTimeout = nullptr;
    /* @brief Timeout for node to receive reply for Cluster Heartbeat Reply message */
    Timeout *clusterHeartbeatReplyTimeout = nullptr;


    /* @brief Timer for node to start an election */
    Timer *startElection = nullptr;
    /* @brief Timer for election duration */
    Timer *electionDuration = nullptr;
    /* @brief Timer for node to observe environment state */
    Timer *observeEnvironmentTimer = nullptr;
    /* @brief Timer for node to send neighbor beacons */
    Timer *neighborBeaconTimer = nullptr;
    /* @brief Timer for node to send cluster heartbeat */
    Timer *clusterHeartbeatTimer = nullptr;
    /* @brief Timer for node to check cluster connectivity */
    Timer *clusterHealthTimer = nullptr;
    /* @brief Timer for node to observe environment state */
    Timer *pruneNeighborListTimer = nullptr;


    // AI params
    PyObject* curObservation;
    PyObject* curAction;
    PyObject* curReward;
    PyObject* curValue;
    PyObject* curLogp;
    int numObservations;

    // model request parameters
    bool receivedModel;
    double requestModelTimeout;

    ClusterTable clusterTable;
    NeighborList neighborList;
    int clusterBeaconDelay;
    double neighborTableTimeout;
    const char* parent;


    /* ELECTION PARAMS */
    // true if node is in process of electing a leader
    // false if node has a leader
    bool electingLeader;
    const char* electionId;
    string electionParentId;
    const char* leaderId;
    bool hasSentElectionAck;
    bool receivedElectionAck;
    bool receivedElectionLeader;


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
