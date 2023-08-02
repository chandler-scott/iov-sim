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
        agent(), clusterTable(), neighborList() {
}

void VehicleApplication::initialize(int stage) {
    BaseApplicationLayer::initialize(stage);
    if (stage == 0) {
        // Initializing members and pointers of your application goes here
        Logger::info("Initializing...", nodeName);

        clusterBeaconDelay = par("clusterBeaconDelay");
        clusterElectionTimeout = par("clusterElectionTimeout");
        clusterHeadTimeout = par("clusterHeadTimeout");
        clusterReelectionThreshold = par("clusterReelectionThreshold");
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
    } else if (stage == 1) {
        // Initializing members that require initialized other modules goes here
        Logger::info("-- initializing message types...", nodeName);

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
        clusterHeartbeatReplyTimeout->setTimeout(2);

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
        clusterHealthTimer = new Timer("Cluster Health Timer");
        clusterHealthTimer->setTime(6);
        pruneNeighborListTimer = new Timer("Prune Neighbor List Timer");
        pruneNeighborListTimer->setTime(3);

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
    if (electionDuration->isScheduled()) {
        cancelEvent(electionDuration);
    }

    delete clusterHeartbeatTimer;
    delete ackTimeout;
    delete startElection;
    delete electionDuration;
    delete electionTimeout;
    delete modelRequestTimeout;
    delete clusterHeartbeatTimeout;
    delete clusterHeartbeatReplyTimeout;

    BaseApplicationLayer::finish();
    // statistics recording goes here
}

void VehicleApplication::onModelMsg(BaseMessage *msg) {
    if (ModelRequest *requestMsg = dynamic_cast<ModelRequest*>(msg)) {
        if (std::strcmp(requestMsg->getOrigin(), "rsu") == 0) {
            Logger::info("received an rsu-originated Model Request Message",
                    nodeName);
            sendModelUpdateMsg();
            Logger::info("-- sending local up-to-date model to rsu...",
                    nodeName);
        }
    } else if (ModelUpdate *appMessage = dynamic_cast<ModelUpdate*>(msg)) {
        if (std::strcmp(appMessage->getDestination(), nodeName) == 0
                && !receivedModel) {
            Logger::info("received a Model Update Message; loading model..",
                    nodeName);
            if (modelRequestTimeout->isScheduled()) {
                cancelEvent(modelRequestTimeout);
            }

            setDisplayColor("#FFC107");
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
        }
    }
}

void VehicleApplication::onElectionMsg(BaseMessage *msg) {
    if (Election *requestMsg = dynamic_cast<Election*>(msg)) {
        // need logic for if we want to join this election
        ElectionHolder holder = ElectionHolder(requestMsg->getElectionHolder());

        bool participate = receivedModel && !(clusterHead || clusterMember)
                && !receivedElectionAck;

        if (participate)
        {
            Logger::info("Participating in election!", nodeName);
            leaveElection();
            sendElectionAck(requestMsg);
        }
    } else if (Ack *appMessage = dynamic_cast<Ack*>(msg)) {
        if (std::strcmp(appMessage->getDestination(), nodeName) == 0
                && electionDuration->isScheduled())
        {
            Logger::info("received an Election Ack Message", nodeName);

            if (electionTimeout->isScheduled())
            {
                cancelEvent(electionTimeout);
            }

            receivedElectionAck = true;

            NeighborEntry entry;

            entry.carsInRange = appMessage->getCarsInRange();
            entry.speed = appMessage->getSpeed();
            entry.velocity = appMessage->getVelocity();
            entry.xVelocity = appMessage->getXVelocity();
            entry.yVelocity = appMessage->getYVelocity();
            entry.acceleration = appMessage->getAcceleration();
            entry.deceleration = appMessage->getDeceleration();
            entry.xPosition = appMessage->getXPosition();
            entry.yPosition = appMessage->getYPosition();
            entry.xDirection = appMessage->getXDirection();
            entry.yDirection = appMessage->getYDirection();
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
        if (std::strcmp(appMessage->getClusterId(), clusterHeadId.c_str())
                == 0) {
            if (clusterHeartbeatTimeout->isScheduled()) {
                cancelEvent(clusterHeartbeatTimeout);
            }

            Logger::info("received a Cluster Heartbeat", nodeName);
            Logger::info("-- from my CH, replying..", nodeName);

            ClusterHeartbeatReply *reply = new ClusterHeartbeatReply();
            reply->setDestination(clusterHeadId.c_str());
            populateWSM(reply);
            sendDown(reply);

            // schedule timeout
            if (clusterHeartbeatTimeout->isScheduled()) {
                cancelEvent(clusterHeartbeatTimeout);
            }

            scheduleAt(simTime() + clusterHeartbeatTimeout->getTimeout(),
                    clusterHeartbeatTimeout);
        }
    } else if (ClusterHeartbeatReply * appMessage =
            dynamic_cast<ClusterHeartbeatReply*>(msg)) {
        if (std::strcmp(appMessage->getDestination(), nodeName) == 0) {
            if (clusterHeartbeatReplyTimeout->isScheduled()) {
                cancelEvent(clusterHeartbeatReplyTimeout);
            }

            Logger::info("received a Cluster Heartbeat Reply", nodeName);
            Logger::info("-- its for me!", nodeName);
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

    if (std::strcmp(nodeName, winnerId) == 0) {
        Logger::info("-- I am cluster head!", nodeName);
        setDisplayColor("#9C27B0");
        getParentModule()->setDisplayName((string(winnerId) + "'s CH").c_str());
        clusterHead = true;
        clusterHeadId = winnerId;
        if (clusterHeartbeatTimer->isScheduled()) {
            cancelEvent(clusterHeartbeatTimer);
        }
        scheduleAt(simTime() + clusterHeartbeatTimer->getTime(),
                clusterHeartbeatTimer);
    } else {
        Logger::info(
                "-- joining " + string(winnerId)
                        + "'s cluster as cluster member", nodeName);
        setDisplayColor("#FF5722");
        getParentModule()->setDisplayName((string(winnerId) + "'s CM").c_str());
        clusterMember = true;
        clusterHeadId = winnerId;

        // schedule timeout
        if (clusterHeartbeatTimeout->isScheduled())
        {
            cancelEvent(clusterHeartbeatTimeout);
        }
        scheduleAt(simTime() + clusterHeartbeatTimeout->getTimeout(),
                clusterHeartbeatTimeout);
    }

    // schedule cluster health timer
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

void VehicleApplication::loadModelUpdate(ModelUpdate *appMessage) {
    if (std::strcmp(appMessage->getOrigin(), "rsu") == 0) {
        Logger::info("-- model from rsu. loading...", nodeName);

        auto [pStateDict, vStateDict] = agent.getStateDictsFromJson(
                appMessage->getPStateDict(), appMessage->getVStateDict());
        agent.loadStateDicts(pStateDict, vStateDict);
    }
}

void VehicleApplication::addSelfToNeighborTable() {
    Coord position = mobility->getPositionAt(simTime());
    Coord direction = mobility->getCurrentDirection();
    double angle = traciVehicle->getAngle();
    double speed = mobility->getSpeed();

    // calculate velocities
    double angleRads = angle * (M_PI / 180.0);
    double xVelocity = speed * cos(angleRads);
    double yVelocity = speed * sin(angleRads);
    double velocity = sqrt((xVelocity * xVelocity) + (yVelocity * yVelocity));

    NeighborEntry entry;

    entry.carsInRange = clusterTable.getSize();
    entry.speed = speed;
    entry.velocity = velocity;
    entry.xVelocity = xVelocity;
    entry.yVelocity = yVelocity;
    entry.acceleration = traciVehicle->getAcceleration();
    entry.deceleration = traciVehicle->getDeccel();
    entry.xPosition = position.x;
    entry.yPosition = position.y;
    entry.xDirection = direction.x;
    entry.yDirection = direction.y;
    entry.timestamp = simTime().dbl();

    // add entry to neighbor table
    Logger::info("-- adding message to neighbor table", nodeName);
    clusterTable.addRow(entry, nodeName);
}

void VehicleApplication::leaveCluster() {
    // leave cluster
    setDisplayColor("#FFC107");
    getParentModule()->setDisplayName("");
    clusterHead = false;
    clusterMember = false;
    clusterHeadId = "";
    receivedElectionAck = false;
    electingLeader = false;
    clusterTable.resetTable();

    // cancel cluster-based timers and timeouts
    if(clusterHealthTimer->isScheduled())
    {
        cancelEvent(clusterHealthTimer);
    }
    if(clusterHeartbeatTimer->isScheduled())
    {
        cancelEvent(clusterHeartbeatTimer);
    }
    if(clusterHeartbeatTimeout->isScheduled())
    {
        cancelEvent(clusterHeartbeatTimeout);
    }
    if(clusterHeartbeatReplyTimeout->isScheduled())
    {
        cancelEvent(clusterHeartbeatReplyTimeout);
    }
}

void VehicleApplication::leaveElection()
{
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
    setDisplayColor("#FFC107");
    getParentModule()->setDisplayName("");
    receivedElectionAck = false;
}

void VehicleApplication::sendModelUpdateMsg() {
    auto [pStateDictJson, vStateDictJson] = agent.getStateDictsAsJson();

    auto pNet = agent.PyObjectToChar(pStateDictJson);
    auto vNet = agent.PyObjectToChar(vStateDictJson);

    BaseApplicationLayer::sendModelUpdateMessage(pNet, vNet, "vehicle");
}

void VehicleApplication::sendModelRequestMsg() {
    Logger::info("-- sending model request message!", nodeName);
    ModelRequest *msg = new ModelRequest();
    populateWSM(msg);
    msg->setData("requesting model");
    msg->setOrigin(nodeName);
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
    setDisplayColor("#25e407");

    electingLeader = true;
    electionId = nodeName;

    Coord position = curPosition;
    Coord direction = mobility->getCurrentDirection();
    double angle = traciVehicle->getAngle();
    double angleRads = angle * (M_PI / 180.0);


    int carsInRange = neighborList.getSize();
    double speed = mobility->getSpeed();
    double xVelocity = speed * cos(angleRads);
    double yVelocity = speed * sin(angleRads);
    double acceleration = traciVehicle->getAcceleration();
    double deceleration = traciVehicle->getDeccel();
    double xPosition = position.x;
    double yPosition = position.y;
    double xDirection = direction.x;
    double yDirection = direction.y;

    ElectionHolder holder = ElectionHolder(
            carsInRange, speed, xVelocity,
            yVelocity, acceleration, deceleration,
            xPosition, yPosition, xDirection,yDirection
    );

    Election *electionMessage = new Election();
    populateWSM(electionMessage);

    electionMessage->setOrigin(nodeName);
    electionMessage->setElectionHolder(holder.toString().c_str());

    sendDown(electionMessage);
    if(electionDuration->isScheduled())
    {
        cancelEvent(electionDuration);
    }
    scheduleAt(simTime() + electionDuration->getTime(), electionDuration);
}

void VehicleApplication::sendElectionAck(Election *msg) {
    electionParentId = msg->getOrigin();
    setDisplayColor("#2196F3");
    getParentModule()->setDisplayName(
            string("electing CH. src: " + electionParentId).c_str());
    Logger::info("participating in " + electionParentId, nodeName);

    electingLeader = true;

    // get mobility data
    double acceleration = traciVehicle->getAcceleration();
    double deceleration = traciVehicle->getDeccel();
    double speed = mobility->getSpeed();
    Coord position = curPosition;
    Coord direction = mobility->getCurrentDirection();
    double angle = traciVehicle->getAngle();

    // calculate velocities
    double angleRads = angle * (M_PI / 180.0);
    double xVelocity = speed * cos(angleRads);
    double yVelocity = speed * sin(angleRads);
    double velocity = sqrt((xVelocity * xVelocity) + (yVelocity * yVelocity));

    Ack *ackMessage = new Ack();
    populateWSM(ackMessage);

    ackMessage->setDestination(electionParentId.c_str());
    ackMessage->setCarsInRange(clusterTable.getSize());

    ackMessage->setAcceleration(acceleration);
    ackMessage->setDeceleration(deceleration);

    ackMessage->setSpeed(speed);
    ackMessage->setVelocity(velocity);
    ackMessage->setXVelocity(xVelocity);
    ackMessage->setYVelocity(yVelocity);

    ackMessage->setXPosition(position.x);
    ackMessage->setYPosition(position.y);
    ackMessage->setXDirection(direction.x);
    ackMessage->setYDirection(direction.y);

    ackMessage->setOrigin(nodeName);

    sendDown(ackMessage);
    Logger::info("-- sent election ack!", nodeName);

    if (ackTimeout->isScheduled()) {
        cancelEvent(ackTimeout);
    }
    scheduleAt(simTime() + ackTimeout->getTimeout(), ackTimeout);
}

void VehicleApplication::sendLeaderMsg(const char *leaderElected) {
    Leader *leaderMessage = new Leader();
    populateWSM(leaderMessage);
    leaderMessage->setOrigin(nodeName);

    leaderMessage->setClusterHeadId(leaderElected);
    sendDelayedDown(leaderMessage, uniform(0.01, 0.2));
}

double VehicleApplication::calculateConnectivityPercentage() {
    int totalNeighbors = neighborList.getSize();

    if (totalNeighbors == 0)
    {
        return 0;
    }

    int matchingNeighbors = 0;
    const std::vector<std::string>& clusterNeighbors = clusterTable.getAllNeighbors();

    // Iterate over all neighbors in NeighborList
    for (const auto& neighbor : neighborList.getAllNeighbors()) {
        // Check if the neighbor id is present in the ClusterTable
        if (std::find(clusterNeighbors.begin(), clusterNeighbors.end(), neighbor) != clusterNeighbors.end()
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
    } else if (msg == startElection) {
        bool inCluster = clusterHead || clusterMember;

        if (!inCluster && !electingLeader) {
            sendElectionMsg();
            scheduleAt(simTime() + electionTimeout->getTimeout(),
                    electionTimeout);
        }
    }
    else if (msg == electionDuration) {
        Logger::info("Election duration.", nodeName);

        // handle election and decide winner
        clusterTable.calculateMetadata();
        clusterTable.scoreNeighbors();

        auto bestCandidateNeighbor =
                clusterTable.getBestScoringNeighbor().c_str();

        handleElection(bestCandidateNeighbor);

        Logger::info("-- sending leader message..", nodeName);
        sendLeaderMsg(bestCandidateNeighbor);
    }
    else if (msg == electionTimeout) {
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

            sendDown(clusterHeartbeat);
            // schedule timer to send next heartbeat
            scheduleAt(simTime() + clusterHeartbeatTimer->getTime(),
                    clusterHeartbeatTimer);

            if (clusterHeartbeatReplyTimeout->isScheduled()) {
                cancelEvent(clusterHeartbeatReplyTimeout);
            }
            // schedule timeout for heartbeat replies
            scheduleAt(simTime() + clusterHeartbeatReplyTimeout->getTimeout(),
                    clusterHeartbeatReplyTimeout);
        }
    }
    else if (msg == clusterHealthTimer)
    {
        Logger::info("Cluster health timer..", nodeName);
        scheduleAt(simTime() + clusterHealthTimer->getTime(), clusterHealthTimer);

        // check cluster/neighbor connectivity
        double connectivity = calculateConnectivityPercentage();

        Logger::info(std::to_string(connectivity), nodeName);
        Logger::info(std::to_string(clusterReelectionThreshold), nodeName);


        // if connectivity < threshold
        if (connectivity < clusterReelectionThreshold)
        {
            Logger::info("-- connectivity is below threshold; holding reelection..",
                    nodeName);
            leaveCluster();
            // start election
            if (!startElection->isScheduled()) {
                scheduleAt(simTime() + startElection->getTime(), startElection);
            }
        }
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

    else {
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
     * cluster reelection threshold
     */
    auto actionValues = agent.toDoublesList(curAction);

    clusterTable.setWeights(actionValues[0], actionValues[1], actionValues[2],
            actionValues[3], actionValues[4], actionValues[5], actionValues[6],
            actionValues[7], actionValues[8], actionValues[9], actionValues[10],
            actionValues[11]);
    clusterReelectionThreshold = actionValues[12];
}

void VehicleApplication::observe() {
    Logger::info("Storing transition to the buffer..", nodeName);

    // calculate reward
    curReward = agent.toPyFloat(1);

    agent.bufferStoreTransition(curObservation, curAction, curReward, curValue,
            curLogp);
}

void VehicleApplication::learn() {
    Logger::info("Learning...", nodeName);
    auto v = agent.toPyFloat(0);
    agent.bufferFinishPath(v);
    agent.learn();
}
