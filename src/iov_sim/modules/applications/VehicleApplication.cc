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

#include "iov_sim/modules/applications/VehicleApplication.h"

using namespace veins;
using namespace iov_sim;

Define_Module(iov_sim::VehicleApplication);

VehicleApplication::VehicleApplication() :
        agent(), aggregator(), clusterTable(), neighborList() {
}

void VehicleApplication::initialize(int stage) {
    BaseApplicationLayer::initialize(stage);
    if (stage == 0) {
        // Initializing members and pointers of your application goes here
        Logger::info("Initializing...", nodeName);

        clusterBeaconDelay = par("clusterBeaconDelay");
        clusterElectionTimeout = par("clusterElectionTimeout");
        clusterHeadTimeout = par("clusterHeadTimeout");
        connectivityThreshold = par("connectivityThreshold");
        electionScoreThreshold = par("electionScoreThreshold");
        clusterScoreThreshold = par("clusterScoreThreshold");
        neighborTableTimeout = par("neighborTableTimeout");
        agent.setLocalStepsPerEpoch(par("localStepsPerEpoch"));

        agent.initializeAgent();

        receivedModel = false;
        requestModelTimeout = 3;

        clusterHead = false;
        clusterMember = false;
        parent = nullptr;
        receivedElectionAck = false;
        receivedElectionLeader = false;

        electingLeader = false;
        electionId = nullptr;
        electionParentId = "";
        leaderId = nullptr;
        hasSentElectionAck = false;
        hasLearned = false;

    } else if (stage == 1) {
        // Initializing members that require initialized other modules goes here
        Logger::info("-- initializing message types...", nodeName);

        initTime = simTime().dbl();
        std::cout << simTime().dbl() << std::endl;

        modelRequestTimeout = new Timeout("Requesting model");
        modelRequestTimeout->setTimeout(2);
        ackTimeout = new Timeout("Ack Timeout");
        ackTimeout->setTimeout(4);
        electionTimeout = new Timeout("Election Timeout");
        electionTimeout->setTimeout(2);
        clusterHeartbeatTimeout = new Timeout("Cluster Heartbeat Timeout");
        clusterHeartbeatTimeout->setTimeout(4);
        clusterHeartbeatReplyTimeout = new Timeout(
                "Cluster Heartbeat Reply Timeout");
        clusterHeartbeatReplyTimeout->setTimeout(4);

        startElection = new Timer("Start Election Timer");
        startElection->setTime(5);
        electionDuration = new Timer("Election Duration Timer");
        electionDuration->setTime(3);
        observeEnvironmentTimer = new Timer("Observe Environment Timer");
        observeEnvironmentTimer->setTime(15);
        neighborBeaconTimer = new Timer("Neighbor Beacon Timer");
        neighborBeaconTimer->setTime(3);
        clusterHeartbeatTimer = new Timer("Cluster Heartbeat Timer");
        clusterHeartbeatTimer->setTime(3);
        clusterHeartbeatDuration = new Timer("Heartbeat Duration Timer");
        clusterHeartbeatDuration->setTime(3);
        clusterHealthTimer = new Timer("Cluster Health Timer");
        clusterHealthTimer->setTime(6);
        pruneNeighborListTimer = new Timer("Prune Neighbor List Timer");
        pruneNeighborListTimer->setTime(3);
        timeClusteredTimer = new Timer("Record Time Clustered Timer");
        timeClusteredTimer->setTime(1);
        modelUpdateWindow = new Timer("Model Update Timer");
        modelUpdateWindow->setTime(3);

        sendModelRequestMsg();
        // schedule neighber beacon msg
        scheduleAt(simTime() + neighborBeaconTimer->getTime(),
                neighborBeaconTimer);
        // schedule model request timeout
        scheduleAt(simTime() + modelRequestTimeout->getTimeout(),
                modelRequestTimeout);
        // schedule prune neighbor list
        scheduleAt(simTime() + pruneNeighborListTimer->getTime(),
                pruneNeighborListTimer);
    }
}

void VehicleApplication::finish() {
    // clean up messages
    if (clusterHeartbeatTimer->isScheduled()) {
        cancelEvent(clusterHeartbeatTimer);
    }
    if (ackTimeout->isScheduled()) {
        cancelEvent(ackTimeout);
    }
    if (startElection->isScheduled()) {
        cancelEvent(startElection);
    }
    if (electionTimeout->isScheduled()) {
        cancelEvent(electionTimeout);
    }
    if (modelRequestTimeout->isScheduled()) {
        cancelEvent(modelRequestTimeout);
    }
    if (clusterHeartbeatTimeout->isScheduled()) {
        cancelEvent(clusterHeartbeatTimeout);
    }
    if (clusterHeartbeatReplyTimeout->isScheduled()) {
        cancelEvent(clusterHeartbeatReplyTimeout);
    }
    if (clusterHeartbeatDuration->isScheduled()) {
        cancelEvent(clusterHeartbeatDuration);
    }
    if (electionDuration->isScheduled()) {
        cancelEvent(electionDuration);
    }
    if (modelUpdateWindow->isScheduled()) {
        cancelEvent(modelUpdateWindow);
    }

    delete ackTimeout;
    delete startElection;
    delete electionDuration;
    delete electionTimeout;
    delete modelRequestTimeout;
    delete clusterHeartbeatTimer;
    delete clusterHeartbeatDuration;
    delete clusterHeartbeatTimeout;
    delete clusterHeartbeatReplyTimeout;
    delete modelUpdateWindow;

    BaseApplicationLayer::finish();
    // statistics recording goes here

    auto formatToNDecimalPlaces = [](double value, int decimalPlaces) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(decimalPlaces) << value;
        return oss.str();
    };

    double avgPacketDelivery = 0;
    double avgPacketDelay = 0;
    double avgClusterLifetime = 0;
    double percentOfTimeInCluster = 0;

    if (sentPackets != 0)
    {
        avgPacketDelivery = ((double)rcvdPackets/(double)sentPackets) * 100;
    }
    if (rcvdPackets != 0)
    {
        avgPacketDelay = (totalPacketDelay/(double)rcvdPackets) * 1000;
    }
    if (totalClusters != 0)
    {
        avgClusterLifetime = (totalClusterLifetime/(double)totalClusters);
        percentOfTimeInCluster = totalClusterLifetime/(simTime().dbl() - initTime) * 100;
    }

    auto sentPacketsOut = "Sent Packets:\t\t\t" + to_string(sentPackets);
    auto rcvdPacketsOut = "Received Packets:\t\t" + to_string(rcvdPackets);
    auto avgPacketDeliveryOut = "Avg Packet Delivery:\t\t" + formatToNDecimalPlaces(avgPacketDelivery, 2) + "%";
    auto avgPacketDelayOut = "Avg Packet Delay:\t\t" + formatToNDecimalPlaces(avgPacketDelay, 3) + "ms";
    auto totalClusterLifetimeOut = "Total Cluster Lifetime:\t\t" + to_string(totalClusterLifetime) + "s";
    auto avgClusterLifetimeOut = "Avg Cluster Lifetime:\t\t" + formatToNDecimalPlaces(avgClusterLifetime, 2) + "s";
    auto percentTimeInClusterOut = "Lifetime Spent in Cluster:\t" + formatToNDecimalPlaces(percentOfTimeInCluster, 2) + "%";

    Logger::info(sentPacketsOut, nodeName);
    Logger::info(rcvdPacketsOut, nodeName);
    Logger::info(avgPacketDeliveryOut, nodeName);
    Logger::info(avgPacketDelayOut, nodeName);
    Logger::info(totalClusterLifetimeOut, nodeName);
    Logger::info(avgClusterLifetimeOut, nodeName);
    Logger::info(percentTimeInClusterOut, nodeName);
}

void VehicleApplication::onModelMsg(BaseMessage *msg) {
    if (ModelInit *appMessage = dynamic_cast<ModelInit*>(msg)) {
        if (std::strcmp(appMessage->getDestination(), nodeName) == 0
                && !receivedModel) {
            Logger::info("received a Model Init Message; loading model..",
                    nodeName);
            if (modelRequestTimeout->isScheduled()) {
                cancelEvent(modelRequestTimeout);
            }

            setDisplayColor(Color::MODEL_RCV);
            receivedModel = true;

            // load model then step()
            loadModelUpdate(appMessage);
            step();

            Logger::info(
                    "-- scheduling cluster head election for 5 seconds from now",
                    nodeName);
            // schedule election
            if (!startElection->isScheduled()) {
                scheduleAt(simTime() + startElection->getTime(), startElection);
            }
            // handle observation
            numObservations = 0;

            scheduleAt(simTime() + observeEnvironmentTimer->getTime(),
                    observeEnvironmentTimer);

            scheduleAt(simTime() + timeClusteredTimer->getTime(),
                    timeClusteredTimer);
        }
    }
    else if (ModelRequest *requestMsg = dynamic_cast<ModelRequest*>(msg)) {
        if (clusterHead && std::strcmp(requestMsg->getOrigin(), "rsu") == 0) {
            Logger::info("CH received an rsu-originated Model Request Message",
                    nodeName);
            // start timer to receive models
            scheduleAt(simTime() + modelUpdateWindow->getTime(),
                    modelUpdateWindow);
            // forward model request to CMs
            sendModelRequestMsg("");
        } else if (clusterMember && hasLearned
                && std::strcmp(requestMsg->getOrigin(), clusterHeadId.c_str())
                        == 0) {
            Logger::info("CM received a CH-originated Model Request Message",
                    nodeName);
            auto jsonStateDicts = agent.getStateDictsAsJson();

            auto pNet = agent.PyObjectToChar(jsonStateDicts.first);
            auto vNet = agent.PyObjectToChar(jsonStateDicts.second);

            BaseApplicationLayer::sendModelUpdateMessage(clusterHeadId.c_str(),
                    pNet, vNet, nodeName);
            hasLearned = false;
        }
        else if (!(clusterHead || clusterMember) && hasLearned
                && std::strcmp(requestMsg->getOrigin(), "rsu") == 0)
        {
            Logger::info("Not in cluster.. rcv'd model update from rsu",
                    nodeName);
            auto jsonStateDicts = agent.getStateDictsAsJson();

            auto pNet = agent.PyObjectToChar(jsonStateDicts.first);
            auto vNet = agent.PyObjectToChar(jsonStateDicts.second);

            BaseApplicationLayer::sendModelUpdateMessage("rsu",
                    pNet, vNet, nodeName);
        }
    } else if (ModelUpdate *appMessage = dynamic_cast<ModelUpdate*>(msg)) {
        if (std::strcmp(appMessage->getDestination(), nodeName) == 0) {
            Logger::info("CH received a Model Update from a CM.",
                    nodeName);
            // add model to list
            auto stateDicts = agent.getStateDictsFromJson(
                    appMessage->getPStateDict(), appMessage->getVStateDict());

            pStateDicts.push_back(stateDicts.first);
            vStateDicts.push_back(stateDicts.second);
        }
        else if (std::strcmp(appMessage->getOrigin(), "rsu") == 0) {
            Logger::info("Received a Model Update from RSU.",
                    nodeName);
            setDisplayColor(Color::MODEL_RCV);
            // add model to list
            auto stateDicts = agent.getStateDictsFromJson(
                    appMessage->getPStateDict(), appMessage->getVStateDict());

            pStateDicts.push_back(stateDicts.first);
            vStateDicts.push_back(stateDicts.second);
        }
    }
}

void VehicleApplication::onElectionMsg(BaseMessage *msg) {
    if (Election *requestMsg = dynamic_cast<Election*>(msg)) {
        if (!receivedModel) {
            return;
        }
        auto electionHolder = NeighborEntry(requestMsg->getNodeMobility());
        auto self = selfToNeighborEntry();

        // compare electionHolder to self
        double score = scoreElectionHolder(electionHolder, self);

        if (score >= electionScoreThreshold) {
            Logger::info("Participating in election!", nodeName);
            leaveElection();

            sendElectionAck(requestMsg);
        }
    } else if (Ack *appMessage = dynamic_cast<Ack*>(msg)) {
        if (std::strcmp(appMessage->getDestination(), nodeName) == 0
                && electionDuration->isScheduled()) {
            Logger::info("received an Election Ack Message", nodeName);

            if (electionTimeout->isScheduled()) {
                cancelEvent(electionTimeout);
            }

            receivedElectionAck = true;
            NeighborEntry entry = NeighborEntry(appMessage->getNodeMobility());
            auto origin = appMessage->getOrigin();

            // add entry to neighbor table
            Logger::info("-- adding message to neighbor table", nodeName);
            clusterTable.addRow(entry, origin);
        }
    } else if (Leader *leaderMsg = dynamic_cast<Leader*>(msg)) {
        if (leaderMsg->getOrigin() == electionParentId) {
            if (ackTimeout->isScheduled()) {
                cancelEvent(ackTimeout);
            }
            Logger::info(
                    "received a Leader message from "
                            + string(leaderMsg->getOrigin())
                            + string(electionParentId), nodeName);
            receivedElectionLeader = true;
            electingLeader = false;
            electionParentId = "";

            handleElection(leaderMsg->getClusterHeadId());
            clusterTable.fromString(leaderMsg->getClusterTable());
        }
    } else if (Probe * requestMsg = dynamic_cast<Probe*>(msg)) {
        Logger::info("received a Probe message", nodeName);
    } else if (Reply * requestMsg = dynamic_cast<Reply*>(msg)) {
        Logger::info("received a Reply message", nodeName);
    } else {
        Logger::info("-- received an unknown beacon message type..", nodeName);
    }
}

void VehicleApplication::onClusterMsg(BaseMessage *msg) {
    if (ClusterHeartbeat *appMessage = dynamic_cast<ClusterHeartbeat*>(msg)) {

        string chId = appMessage->getClusterId();

        if (receivedModel && chId == clusterHeadId) {
            clusterTable.fromString(appMessage->getClusterTable());

            if (clusterScoreThreshold < scoreCluster()) {
                // join cluster
                sendHeartbeatReply();
                cmHeartbeatRcvs++;
            } else {
                leaveCluster();
            }
        } else if (std::strcmp(appMessage->getClusterId(),
                clusterHeadId.c_str()) == 0) {
            Logger::info("received a Cluster Heartbeat from " + clusterHeadId,
                    nodeName);
            Logger::info("-- from my CH, replying..", nodeName);

            clusterTable.fromString(appMessage->getClusterTable());
            cmHeartbeatRcvs++;

            sendHeartbeatReply();
        }
    } else if (ClusterHeartbeatReply * appMessage =
            dynamic_cast<ClusterHeartbeatReply*>(msg)) {
        if (std::strcmp(appMessage->getDestination(), nodeName) == 0) {
            chHeartbeatRcvs++;
            rcvdPackets++;
            totalPacketDelay += simTime().dbl() - appMessage->getTimestamp();

            if (clusterHeartbeatReplyTimeout->isScheduled()) {
                cancelEvent(clusterHeartbeatReplyTimeout);
            }

            Logger::info("received a Cluster Heartbeat Reply", nodeName);
            auto entry = NeighborEntry(appMessage->getNodeMobility());
            clusterTable.addRow(entry, appMessage->getOrigin());

            // add node to cluster list if not already present
            if (std::find(clusterNodes.begin(), clusterNodes.end(),
                    appMessage->getOrigin()) == clusterNodes.end()) {
                clusterNodes.push_back(appMessage->getOrigin());
            }

            // send ACK to CM
            auto *ack = new ClusterHeartbeatReplyAck("Heartbeat reply ACK");
            populateWSM(ack);

            ack->setDestination(appMessage->getOrigin());
            ack->setTimestamp(simTime().dbl());

            sendDown(ack);
        }
    } else if (ClusterHeartbeatReplyAck * appMessage = dynamic_cast<ClusterHeartbeatReplyAck*>(msg)) {
        if (std::strcmp(appMessage->getDestination(), nodeName) == 0)
        {
            Logger::info("rcv'd heartbeat reply ACK", nodeName);
            rcvdPackets++;
            totalPacketDelay += simTime().dbl() - appMessage->getTimestamp();
        }
    } else if (ClusterJoin * appMessage = dynamic_cast<ClusterJoin*>(msg)) {
        Logger::info("received a Cluster Join Message", nodeName);
    } else {
        Logger::error("--> Unknown message type", nodeName);
    }
}

void VehicleApplication::onNeighborMsg(BaseMessage *msg) {
    if (NeighborBeacon *appMessage = dynamic_cast<NeighborBeacon*>(msg)) {
        Logger::info("received a Neighbor Beacon Message", nodeName);
        neighborList.addNeighbor(appMessage->getOrigin(), simTime().dbl());
    } else {
    }
}

void VehicleApplication::handleElection(const char *winnerId) {
    if (electionTimeout->isScheduled()) {
        cancelEvent(electionTimeout);
    }

    leaveElection();
    totalClusters++;
    clusterInitTime = simTime().dbl();

    if (std::strcmp(nodeName, winnerId) == 0) {
        Logger::info("-- I am cluster head!", nodeName);
        setDisplayColor(Color::CLUSTER_HEAD);
        getParentModule()->setDisplayName((string(winnerId) + "'s CH").c_str());
        clusterHead = true;
        clusterHeadId = winnerId;
        if (clusterHeartbeatTimer->isScheduled()) {
            cancelEvent(clusterHeartbeatTimer);
        }
        scheduleAt(simTime() + clusterHeartbeatTimer->getTime(),
                clusterHeartbeatTimer);
    } else {
        // join cluster
        Logger::info(
                "-- joining " + string(winnerId)
                        + "'s cluster as cluster member", nodeName);
        setDisplayColor(Color::CLUSTER_MEMBER);
        getParentModule()->setDisplayName((string(winnerId) + "'s CM").c_str());
        clusterMember = true;
        clusterHeadId = winnerId;

        // schedule timeout
        if (clusterHeartbeatTimeout->isScheduled()) {
            cancelEvent(clusterHeartbeatTimeout);
        }
        scheduleAt(simTime() + clusterHeartbeatTimeout->getTimeout(),
                clusterHeartbeatTimeout);
    }

    // schedule cluster health timer
    if (clusterHealthTimer->isScheduled()) {
        cancelEvent(clusterHealthTimer);
    }
    scheduleAt(simTime() + clusterHealthTimer->getTime(), clusterHealthTimer);
}

void VehicleApplication::handleObservation() {
    numObservations++;
    observe();
    scheduleAt(simTime() + observeEnvironmentTimer->getTime(),
            observeEnvironmentTimer);
}

void VehicleApplication::handleLearn() {
    numObservations = 0;
    learn();
    scheduleAt(simTime() + observeEnvironmentTimer->getTime(),
            observeEnvironmentTimer);
}

void VehicleApplication::loadModelUpdate(ModelBaseMessage *appMessage) {
    if (std::strcmp(appMessage->getOrigin(), "rsu") == 0) {
        Logger::info("-- model from rsu. loading...", nodeName);

        auto stateDicts = agent.getStateDictsFromJson(
                appMessage->getPStateDict(), appMessage->getVStateDict());

        agent.loadStateDicts(stateDicts.first, stateDicts.second);
    }
}

NeighborEntry VehicleApplication::selfToNeighborEntry() {
    Coord position = mobility->getPositionAt(simTime());
    Coord direction = mobility->getCurrentDirection();
    double angle = traciVehicle->getAngle();
    double speed = mobility->getSpeed();

    // calculate velocities
    double angleRads = angle * (M_PI / 180.0);
    double xVelocity = speed * cos(angleRads);
    double yVelocity = speed * sin(angleRads);
    double velocity = sqrt((xVelocity * xVelocity) + (yVelocity * yVelocity));

    NeighborEntry entry = NeighborEntry(clusterTable.getSize(), speed, velocity,
            xVelocity, yVelocity, traciVehicle->getAcceleration(),
            traciVehicle->getDeccel(), position.x, position.y, direction.x,
            direction.y, simTime().dbl());

    return entry;
}

void VehicleApplication::addSelfToNeighborTable() {
    // add entry to neighbor table
    Logger::info("-- adding message to neighbor table", nodeName);
    auto entry = selfToNeighborEntry();
    clusterTable.addRow(entry, nodeName);
}

void VehicleApplication::leaveCluster() {
    // leave cluster
    setDisplayColor(Color::MODEL_RCV);
    getParentModule()->setDisplayName("");
    clusterHead = false;
    clusterMember = false;
    clusterHeadId = "";
    receivedElectionAck = false;
    electingLeader = false;
    clusterTable.resetTable();

    totalClusterLifetime += simTime().dbl() - clusterInitTime;

    auto log = to_string(simTime().dbl()) + "+" +
            to_string(clusterInitTime) + "= " + to_string(totalClusterLifetime);

    Logger::error(log, nodeName);

    // cancel cluster-based timers and timeouts
    if (clusterHealthTimer->isScheduled()) {
        cancelEvent(clusterHealthTimer);
    }
    if (clusterHeartbeatTimer->isScheduled()) {
        cancelEvent(clusterHeartbeatTimer);
    }
    if (clusterHeartbeatDuration->isScheduled()) {
        cancelEvent(clusterHeartbeatDuration);
    }
    if (clusterHeartbeatTimeout->isScheduled()) {
        cancelEvent(clusterHeartbeatTimeout);
    }
    if (clusterHeartbeatReplyTimeout->isScheduled()) {
        cancelEvent(clusterHeartbeatReplyTimeout);
    }
}

void VehicleApplication::leaveElection() {
    // cancel any election timers we may have
    if (electionDuration->isScheduled()) {
        cancelEvent(electionDuration);
    }
    if (electionTimeout->isScheduled()) {
        cancelEvent(electionTimeout);
    }
    if (ackTimeout->isScheduled()) {
        cancelEvent(ackTimeout);
    }

    electionId = "";
    electionParentId = "";
    parent = "";
    electingLeader = false;
    setDisplayColor(Color::MODEL_RCV);
    getParentModule()->setDisplayName("");
    receivedElectionAck = false;
}

void VehicleApplication::sendModelUpdateMsg() {
    auto jsonStateDicts = agent.getStateDictsAsJson();

    auto pNet = agent.PyObjectToChar(jsonStateDicts.first);
    auto vNet = agent.PyObjectToChar(jsonStateDicts.second);

    BaseApplicationLayer::sendModelUpdateMessage(clusterHeadId.c_str(), pNet,
            vNet, nodeName);
}

void VehicleApplication::sendModelRequestMsg(char* destination) {
    Logger::info("-- sending model request message!", nodeName);
    ModelRequest *msg = new ModelRequest();
    populateWSM(msg);
    msg->setData("requesting model");
    msg->setOrigin(nodeName);
    msg->setDestination(destination);
    sendDown(msg);
}

void VehicleApplication::sendNeighborBeacon() {
    Logger::info("sending a Neighbor Beacon Message...", nodeName);

    NeighborBeacon *msg = new NeighborBeacon();
    populateWSM(msg);

    msg->setOrigin(nodeName);

    sendDelayedDown(msg, uniform(0.01, 0.2));
}

void VehicleApplication::sendElectionMsg() {
    Logger::info("holding cluster election...", nodeName);
    getParentModule()->setDisplayName("electing CH src..");
    setDisplayColor(Color::ELECTION_SRC);

    electingLeader = true;
    electionId = nodeName;

    Election *electionMessage = new Election();
    populateWSM(electionMessage);

    electionMessage->setOrigin(nodeName);
    electionMessage->setNodeMobility(selfToNeighborEntry().toString().c_str());

    sendDown(electionMessage);
    if (electionDuration->isScheduled()) {
        cancelEvent(electionDuration);
    }
    scheduleAt(simTime() + electionDuration->getTime(), electionDuration);
}

void VehicleApplication::sendElectionAck(Election *msg) {
    electingLeader = true;
    electionParentId = msg->getOrigin();
    setDisplayColor(Color::ELECTION_PARTICIPANT);
    getParentModule()->setDisplayName(
            string("electing CH. src: " + electionParentId).c_str());
    Logger::info("participating in " + electionParentId + "'s election.",
            nodeName);

    NeighborEntry entry = selfToNeighborEntry();

    Ack *ackMessage = new Ack();
    populateWSM(ackMessage);

    ackMessage->setDestination(electionParentId.c_str());
    ackMessage->setNodeMobility(entry.toString().c_str());
    ackMessage->setOrigin(nodeName);

    sendDown(ackMessage);

    if (ackTimeout->isScheduled()) {
        cancelEvent(ackTimeout);
    }
    scheduleAt(simTime() + ackTimeout->getTimeout(), ackTimeout);
}

void VehicleApplication::sendLeaderMsg(const char *leaderElected) {
    addSelfToNeighborTable();

    Leader *leaderMessage = new Leader();
    populateWSM(leaderMessage);

    leaderMessage->setOrigin(nodeName);
    leaderMessage->setClusterHeadId(leaderElected);
    leaderMessage->setClusterTable(clusterTable.toString().c_str());

    sendDelayedDown(leaderMessage, uniform(0.01, 0.2));
}

void VehicleApplication::sendHeartbeatReply() {
    ClusterHeartbeatReply *reply = new ClusterHeartbeatReply();
    populateWSM(reply);

    reply->setDestination(clusterHeadId.c_str());
    reply->setOrigin(nodeName);
    reply->setTimestamp(simTime().dbl());
    auto entry = selfToNeighborEntry();
    reply->setNodeMobility(entry.toString().c_str());

    sendDown(reply);

    // schedule timeout
    if (clusterHeartbeatTimeout->isScheduled()) {
        cancelEvent(clusterHeartbeatTimeout);
    }

    scheduleAt(simTime() + clusterHeartbeatTimeout->getTimeout(),
            clusterHeartbeatTimeout);

    cmHeartbeatSnds++;
    sentPackets++;
}

double VehicleApplication::scoreCluster() {
    // score cluster
    auto self = selfToNeighborEntry();
    clusterTable.calculateMetadata();

    double score = carsInRangeWeight
            * (clusterTable.avgCarsInRange - self.carsInRange)
            + xVelocityWeight * (clusterTable.avgXVelocity - self.xVelocity)
            + yVelocityWeight * (clusterTable.avgYVelocity - self.yVelocity)
            + velocityWeight * (clusterTable.avgVelocity - self.velocity)
            + speedWeight * (clusterTable.avgSpeed - self.speed)
            + accelerationWeight
                    * (clusterTable.avgAcceleration - self.acceleration)
            + decelerationWeight
                    * (clusterTable.avgDeceleration - self.deceleration)
            + xPositionWeight * (clusterTable.avgXPosition - self.xPosition)
            + yPositionWeight * (clusterTable.avgYPosition - self.yPosition)
            + timestampWeight * (clusterTable.avgTimestamp - self.timestamp)
            + xDirectionWeight * (clusterTable.avgXDirection - self.xDirection)
            + yDirectionWeight * (clusterTable.avgYDirection - self.yDirection);
    return score;
}

double VehicleApplication::scoreElectionHolder(NeighborEntry electionHolder,
        NeighborEntry self) {
    double score = electionSpeedWeight * (electionHolder.speed - self.speed)
            + electionXVelocityWeight * (electionHolder.speed - self.speed)
            + electionYVelocityWeight * (electionHolder.speed - self.speed)
            + electionAccelerationWeight * (electionHolder.speed - self.speed)
            + electionDecelerationWeight * (electionHolder.speed - self.speed)
            + electionXPositionWeight * (electionHolder.speed - self.speed)
            + electionYPositionWeight * (electionHolder.speed - self.speed)
            + electionXDirectionWeight * (electionHolder.speed - self.speed)
            + electionYDirectionWeight * (electionHolder.speed - self.speed)
            + electionReceivedElectionAckWeight * (receivedElectionAck)
            + electionCHWeight * (clusterHead)
            + electionCMWeight * (clusterMember)
            + electionConnectivityWeight * (calculateConnectivityPercentage());

    return score;
}

double VehicleApplication::calculateConnectivityPercentage() {
    int totalNeighbors = neighborList.getSize();

    if (totalNeighbors == 0) {
        return 0; //
    }

    int matchingNeighbors = 0;
    const std::vector<std::string> &clusterNeighbors =
            clusterTable.getAllNeighbors();

    // Iterate over all neighbors in NeighborList
    for (const auto &neighbor : neighborList.getAllNeighbors()) {
        // Check if the neighbor id is present in the ClusterTable
        if (std::find(clusterNeighbors.begin(), clusterNeighbors.end(),
                neighbor) != clusterNeighbors.end()
                && std::strcmp(nodeName, neighbor.c_str()) != 0) {
            matchingNeighbors++;
        }
    }

    return (static_cast<double>(matchingNeighbors) / totalNeighbors);
}

void VehicleApplication::handleSelfMsg(cMessage *msg) {
    if (msg == modelRequestTimeout) {
        sendModelRequestMsg();
        scheduleAt(simTime() + requestModelTimeout, modelRequestTimeout);
    } else if (msg == neighborBeaconTimer) {
        sendNeighborBeacon();
        scheduleAt(simTime() + neighborBeaconTimer->getTime(),
                neighborBeaconTimer);
    } else if (msg == pruneNeighborListTimer) {
        neighborList.pruneList(simTime().dbl());
        scheduleAt(simTime() + pruneNeighborListTimer->getTime(),
                pruneNeighborListTimer);
    } else if (msg == observeEnvironmentTimer) {
        if (numObservations < agent.getLocalStepsPerEpoch()) {
            handleObservation();
        } else {
            handleLearn();
        }
        step();
    } else if (msg == startElection) { //
        bool inCluster = clusterHead || clusterMember;

        if (!inCluster && !electingLeader) {
            sendElectionMsg();
            scheduleAt(simTime() + electionTimeout->getTimeout(),
                    electionTimeout);
        }
    } else if (msg == electionDuration) {
        Logger::info("Election duration.", nodeName);

        // handle election and decide winner
        clusterTable.calculateMetadata();
        clusterTable.scoreNeighbors();

        const std::string &bestNeighbor = clusterTable.getBestScoringNeighbor();
        handleElection(bestNeighbor.c_str());

        Logger::info("-- sending leader message..", nodeName);
        sendLeaderMsg(bestNeighbor.c_str());
    } else if (msg == electionTimeout) {
        Logger::info("Election timeout..", nodeName);

        if (!receivedElectionAck) {
            sendElectionMsg();
            scheduleAt(simTime() + electionTimeout->getTimeout(),
                    electionTimeout);
        }
    } else if (msg == ackTimeout) {
        Logger::info("Ack timeout..", nodeName);
        leaveElection();

        // start election
        Logger::info(
                "-- scheduling cluster head election for 5 seconds from now",
                nodeName);
        if (!startElection->isScheduled()) {
            scheduleAt(simTime() + startElection->getTime(), startElection);
        }

    } else if (msg == clusterHeartbeatTimer) {
        if (clusterHead == true) {
            Logger::info(
                    string("sending cluster ") + nodeName
                            + string("'s heartbeat..."), nodeName);

            // set up heartbeat
            ClusterHeartbeat *clusterHeartbeat = new ClusterHeartbeat();
            populateWSM(clusterHeartbeat);

            clusterHeartbeat->setClusterId(nodeName);
            clusterHeartbeat->setClusterTable(clusterTable.toString().c_str());
            clusterHeartbeat->setTimestamp(simTime().dbl());

            sendDown(clusterHeartbeat);

            chHeartbeatSnds += 1 * clusterTable.getSize();
            sentPackets += 1 * clusterTable.getSize();
            heartbeats++;

            if (clusterHeartbeatReplyTimeout->isScheduled()) {
                cancelEvent(clusterHeartbeatReplyTimeout);
            }
            // schedule timeout for heartbeat replies
            scheduleAt(simTime() + clusterHeartbeatReplyTimeout->getTimeout(),
                    clusterHeartbeatReplyTimeout);

            if (clusterHeartbeatDuration->isScheduled()) {
                cancelEvent(clusterHeartbeatDuration);
            }
            // schedule timeout for heartbeat duration
            scheduleAt(simTime() + clusterHeartbeatDuration->getTime(),
                    clusterHeartbeatDuration);
        }
    } else if (msg == clusterHealthTimer) {
        Logger::info("Cluster health timer..", nodeName);
        scheduleAt(simTime() + clusterHealthTimer->getTime(),
                clusterHealthTimer);

        // check cluster/neighbor connectivity
        double connectivity = calculateConnectivityPercentage();

        Logger::info(std::to_string(connectivity), nodeName);
        Logger::info(std::to_string(connectivityThreshold), nodeName);

        // if connectivity < threshold
        if (connectivity < connectivityThreshold) {
            Logger::info(
                    "-- connectivity is below threshold; holding reelection..",
                    nodeName);
            leaveCluster();
            // start election
            if (!startElection->isScheduled()) {
                scheduleAt(simTime() + startElection->getTime(), startElection);
            }
        }
    } else if (msg == clusterHeartbeatDuration) {
        Logger::info("Cluster heartbeat Duration.", nodeName);

        if (clusterHeartbeatReplyTimeout->isScheduled()) {
            cancelEvent(clusterHeartbeatReplyTimeout);
        }

        // if no responses
        if (clusterNodes.size() == 0) {
            Logger::info("No responses, leaving cluster.", nodeName);
            leaveCluster();
        } else {
            // push self on clusterNodes and prune cluster table
            if (std::find(clusterNodes.begin(), clusterNodes.end(), nodeName)
                    == clusterNodes.end()) {
                clusterNodes.push_back(nodeName);
                clusterTable.pruneTable(clusterNodes);
            }
        }

        clusterNodes.clear();

        // schedule timer to send next heartbeat
        if (clusterHeartbeatTimer->isScheduled()) {
            cancelEvent(clusterHeartbeatTimer);
        }
        scheduleAt(simTime() + 0.1, clusterHeartbeatTimer);
    }
    // if CH did not receive heartbeat replies..
    else if (msg == clusterHeartbeatReplyTimeout) {
        Logger::info("Cluster heartbeat Reply timeout.. starting new election.",
                nodeName);

        leaveCluster();
        // start election
        if (!startElection->isScheduled()) {
            scheduleAt(simTime() + startElection->getTime(), startElection);
        }
    }
    // if CM did not receive heartbeat..
    else if (msg == clusterHeartbeatTimeout) {
        Logger::info("Cluster heartbeat timeout.. starting new election.",
                nodeName);

        leaveCluster();
        // start election
        if (!startElection->isScheduled()) {
            scheduleAt(simTime() + startElection->getTime(), startElection);
        }
    }
    // timeClustered for reward calculation
    else if (msg == timeClusteredTimer) {
        if (clusterHead || clusterMember) {
            timeClustered += 1;
        }

        scheduleAt(simTime() + timeClusteredTimer->getTime(),
                timeClusteredTimer);
    } else if (msg == modelUpdateWindow) {
        Logger::info("End of model update rcv window; aggregating...",
                nodeName);
        // add self to list
        auto stateDicts = agent.getStateDicts();

        pStateDicts.push_back(stateDicts.first);
        vStateDicts.push_back(stateDicts.second);

        // convert vector to python list
        PyObject* pList = PyList_New(pStateDicts.size());
        PyObject* vList = PyList_New(vStateDicts.size());

        for (size_t i = 0; i < pStateDicts.size(); ++i) {
            PyList_SET_ITEM(pList, i, pStateDicts[i]);
        }
        for (size_t i = 0; i < vStateDicts.size(); ++i) {
            PyList_SET_ITEM(vList, i, vStateDicts[i]);
        }

        // aggregate list of rcv'd models and load
        auto models = aggregator.aggregateModels(pList, vList);
        agent.loadStateDicts(models.first, models.second);

        // fwd to rsu
        auto jsonStateDicts = agent.getStateDictsAsJson();

        auto pNet = agent.PyObjectToChar(jsonStateDicts.first);
        auto vNet = agent.PyObjectToChar(jsonStateDicts.second);

        BaseApplicationLayer::sendModelUpdateMessage("rsu", pNet,
                vNet, nodeName);

        pStateDicts.clear();
        vStateDicts.clear();
    } else {
        Logger::error(
                string("unknown message type: ") + msg->getClassAndFullName(),
                nodeName);
    }
}

void VehicleApplication::step() {
    Logger::info("Stepping in the environment...", nodeName);
    addSelfToNeighborTable();
    clusterTable.calculateMetadata();

    // step in the environment with observation
    double connectivity = calculateConnectivityPercentage();
    std::vector<double> obs = clusterTable.toList();
    obs.push_back(connectivity);

    /*
     * Current observation includes: averages for each value in
     * neighbor table and node connectivity (nodes in cluster that
     * are in range)-> 13 total values
     */
    curObservation = agent.toTensor(obs);
    std::tie(curAction, curValue, curLogp) = agent.step(curObservation);

    /*
     * Action values include: weights for neighbor score parameters,
     * election holder score, cluster join score
     * connectivity threshold, election score threshold, and
     * cluster score threshold (40 parameters total)
     */
    auto actionValues = agent.toDoublesList(curAction);

    clusterTable.setWeights(actionValues[0], actionValues[1], actionValues[2],
            actionValues[3], actionValues[4], actionValues[5], actionValues[6],
            actionValues[7], actionValues[8], actionValues[9], actionValues[10],
            actionValues[11]);

    electionSpeedWeight = actionValues[12];
    electionXVelocityWeight = actionValues[13];
    electionYVelocityWeight = actionValues[14];
    electionAccelerationWeight = actionValues[15];
    electionDecelerationWeight = actionValues[16];
    electionXPositionWeight = actionValues[17];
    electionYPositionWeight = actionValues[18];
    electionXDirectionWeight = actionValues[19];
    electionYDirectionWeight = actionValues[20];
    electionReceivedElectionAckWeight = actionValues[21];
    electionCHWeight = actionValues[22];
    electionCMWeight = actionValues[23];
    electionConnectivityWeight = actionValues[24];

    carsInRangeWeight = actionValues[25];
    xVelocityWeight = actionValues[26];
    yVelocityWeight = actionValues[27];
    velocityWeight = actionValues[28];
    speedWeight = actionValues[29];
    accelerationWeight = actionValues[30];
    decelerationWeight = actionValues[31];
    xPositionWeight = actionValues[32];
    yPositionWeight = actionValues[33];
    timestampWeight = actionValues[34];
    xDirectionWeight = actionValues[35];
    yDirectionWeight = actionValues[36];

    connectivityThreshold = actionValues[37];
    electionScoreThreshold = actionValues[38];
    clusterScoreThreshold = actionValues[39];
}

void VehicleApplication::observe() {
    Logger::info("Storing transition to the buffer..", nodeName);

    // calculate reward
    // max 25 connectivity
    double connectivity = calculateConnectivityPercentage() * 25;

    // max 15 clusterTime
    double clusterTime = timeClustered;

    // set to 1 if 0
    if (chHeartbeatSnds == 0) {
        chHeartbeatSnds = 1;
    }

    if (cmHeartbeatSnds == 0) {
        cmHeartbeatSnds = 1;
    }

    // max ~30
    double chHeartbeatPercent = (chHeartbeatRcvs / chHeartbeatSnds) * heartbeats
            * 10;
    // max ~30
    double cmHeartbeatPercent = (cmHeartbeatRcvs / cmHeartbeatSnds)
            * cmHeartbeatSnds * 10;

    // max possible reward: 85
    double reward = connectivity + clusterTime + chHeartbeatPercent
            + cmHeartbeatPercent;

    // log reward
    std::string output = "REWARD LOG:\n";
    output += "Connectivity\t" + std::to_string(connectivity) + "\n";
    output += "Cluster Time\t" + std::to_string(clusterTime) + "\n";
    output += "CH Heartbeat %\t" + std::to_string(chHeartbeatRcvs) + "/"
            + std::to_string(chHeartbeatSnds) + " = "
            + std::to_string(chHeartbeatPercent) + "%\n";
    output += "CM Heartbeat %\t" + std::to_string(cmHeartbeatRcvs) + "/"
            + std::to_string(cmHeartbeatSnds) + " = "
            + std::to_string(cmHeartbeatPercent) + "%\n";
    output += "Total Reward\t" + std::to_string(reward) + "\n";
    Logger::info(output, nodeName);

    curReward = agent.toPyFloat(reward);

    agent.bufferStoreTransition(curObservation, curAction, curReward, curValue,
            curLogp);

    timeClustered = 0;
    chHeartbeatSnds = 0;
    chHeartbeatRcvs = 0;
    cmHeartbeatSnds = 0;
    cmHeartbeatRcvs = 0;
    heartbeats = 0;
}

void VehicleApplication::learn() {
    Logger::info("Learning...", nodeName);
    auto v = agent.toPyFloat(0);
    agent.bufferFinishPath(v);
    agent.learn();
    hasLearned = true;
}
