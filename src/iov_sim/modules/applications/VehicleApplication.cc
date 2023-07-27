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

VehicleApplication::VehicleApplication ()
     : agent(), neighborTable()
       { }

void VehicleApplication::initialize(int stage)
{
    BaseApplicationLayer::initialize(stage);
    if (stage == 0) {
        // Initializing members and pointers of your application goes here
        Logger::info("Initializing...", nodeName);

        clusterBeaconDelay = par("clusterBeaconDelay");
        clusterElectionDuration = par("clusterElectionDuration");
        clusterElectionTimeout = par("clusterElectionTimeout");
        clusterHeadTimeout = par("clusterHeadTimeout");
        clusterReelectionThreshold = par("clusterReelectionThreshold");
        neighborTableTimeout = par("neighborTableTimeout");

        clusterHead = false;
        clusterMember = false;
    }
    else if (stage == 1) {
        // Initializing members that require initialized other modules goes here
        Logger::info("-- initializing message types...", nodeName);

        modelRequestMessage = new ModelRequestMessage();
        vehClusterDataMessage = new VehicleClusterDataMessage("ClusterDataMessage");
        rsuClusterDataMessage = new RSUClusterDataMessage("ClusterDataMessage");
        clusterElectionMessage = new ClusterElectionMessage("ClusterElectionMessage");
        clusterSelectionMessage = new ClusterSelectionMessage(nodeName);
        clusterBeaconMessage = new ClusterBeaconMessage();
        clusterBeaconSelfMessage = new ClusterBeaconMessage("ClusterBeaconMessage");
        neighborTablePruneMessage = new NeighborTablePruneMessage("NeighborTablePruneMessage");

        Logger::info("-- populating messages...", nodeName);

        populateWSM(clusterBeaconMessage);
        populateWSM(vehClusterDataMessage);
        populateWSM(rsuClusterDataMessage);
        populateWSM(modelRequestMessage);

        modelRequestMessage->setData("requesting model");

        Logger::info("-- sending model request message!", nodeName);

        sendDown(modelRequestMessage->dup());
        scheduleAt(simTime() + clusterBeaconDelay, clusterBeaconSelfMessage->dup());
        scheduleAt(simTime() + neighborTableTimeout, neighborTablePruneMessage->dup());
    }
}

void VehicleApplication::finish()
{
    // clean up messages
    delete modelRequestMessage;
    delete neighborTablePruneMessage;
    delete clusterBeaconMessage;
    delete clusterBeaconSelfMessage;
    delete clusterElectionMessage;
    delete clusterSelectionMessage;
    delete vehClusterDataMessage;
    delete rsuClusterDataMessage;

    BaseApplicationLayer::finish();
    // statistics recording goes here
}

void VehicleApplication::onBSM(BaseFrame1609_4* bsm)
{
    if (ClusterBeaconMessage* beaconMsg = dynamic_cast<ClusterBeaconMessage*>(bsm)) {
        Logger::info("received a Cluster Beacon Message", nodeName);

        // findHost()->getDisplayString().setTagArg("i", 1, "green");

        NeighborEntry entry;

        entry.carsInRange = beaconMsg->getCarsInRange();
        entry.speed = beaconMsg->getSpeed();
        entry.velocity = beaconMsg->getVelocity();
        entry.xVelocity = beaconMsg->getXVelocity();
        entry.yVelocity = beaconMsg->getYVelocity();
        entry.acceleration = beaconMsg->getAcceleration();
        entry.deceleration = beaconMsg->getDeceleration();
        entry.xPosition = beaconMsg->getXPosition();
        entry.yPosition = beaconMsg->getYPosition();
        entry.xDirection = beaconMsg->getXDirection();
        entry.yDirection = beaconMsg->getYDirection();
        entry.yDirection = beaconMsg->getYDirection();
        entry.timestamp = simTime().dbl();

        auto sender = beaconMsg->getSender();

        // add entry to neighbor table
        Logger::info("-- adding message to neighbor table", nodeName);
        neighborTable.addRow(entry, sender);
    }
    else if (ModelRequestMessage* requestMsg = dynamic_cast<ModelRequestMessage*>(bsm)) {
        Logger::info("received a Model Request Message", nodeName);
        findHost()->getDisplayString().setTagArg("i", 1, "blue");
        auto origin = requestMsg->getOrigin();

        if (std::strcmp(origin, "rsu") == 0)
        {
            Logger::info("-- rsu beacon", nodeName);
            sendModelUpdateMessage();
            Logger::info("-- sending local up-to-date model to rsu.", nodeName);
        }
        else {
            findHost()->getDisplayString().setTagArg("i", 1, "");
            Logger::info("-- ignoring vehicle beacon", nodeName);
        }
    }
    else if (ClusterSelectionMessage* requestMsg = dynamic_cast<ClusterSelectionMessage*>(bsm)) {
        Logger::info("received a cluster selection message", nodeName);
        findHost()->getDisplayString().setTagArg("i", 1, "blue");

        clusterHeadId = requestMsg->getData();
        clusterMember = true;
    }
    else {
        Logger::info("-- received an unknown beacon message type..", nodeName);
    }
}

void VehicleApplication::onWSM(BaseFrame1609_4* frame)
{
    if (frame) {
        if (dynamic_cast<ModelUpdateMessage*>(frame)) {
            Logger::info("received a Model Update Message", nodeName);
            ModelUpdateMessage* appMessage = check_and_cast<ModelUpdateMessage*>(frame);

            // load up-to-date model
            loadModelUpdate(appMessage);
            Logger::info("-- loaded most up-to-date model", nodeName);

            Logger::info("-- scheduling cluster head election for 5 seconds from now", nodeName);
            scheduleAt(simTime() + 5, clusterElectionMessage->dup());

        }
        else if (dynamic_cast<RSUClusterDataMessage*>(frame) && clusterHead == true) {
            Logger::info("received an RSU Cluster Data Message", nodeName);
            RSUClusterDataMessage* appMessage = check_and_cast<RSUClusterDataMessage*>(frame);

            auto destination = appMessage->getDestination();
            std::string origin = appMessage->getOrigin();

            if (std::strcmp(destination, nodeName) == 0)
            {
                Logger::info("-- its for me!", nodeName);
                Logger::info("-- Forwarding to neighbors..", nodeName);

                auto neighbors = neighborTable.getAllNeighbors();

                neighborTable.printTable();
                for (const auto neighbor : neighbors) {
                    if (neighbor != nodeName)
                    {
                        auto log = "-- sending data message to " + neighbor;
                        Logger::info(log, nodeName);
                        vehClusterDataMessage->setDestination(neighbor.c_str());
                        vehClusterDataMessage->setOrigin(nodeName);
                        sendDelayedDown(vehClusterDataMessage->dup(), uniform(0.01, 0.2));
                    }
                }
            }
            else {
                Logger::info("-- not for me...", nodeName);
            }
        }
        else if (dynamic_cast<VehicleClusterDataMessage*>(frame)) {
            Logger::info("received a Vehicle Cluster Data Message", nodeName);
            VehicleClusterDataMessage* appMessage = check_and_cast<VehicleClusterDataMessage*>(frame);

            auto destination = appMessage->getDestination();
            std::string origin = appMessage->getOrigin();

            if (std::strcmp(destination, nodeName) == 0)
            {
                Logger::info("-- its for me!", nodeName);

                if (clusterHead == true){
                    auto log = "-- CH && msg originated from veh " + origin;
                    Logger::info(log, nodeName);
                }
                else if (clusterMember == true)
                {
                    // message is from another vehicle
                    Logger::info("-- CM recieved data message", nodeName);
                    // disperse to CH
                    vehClusterDataMessage->setDestination(clusterHeadId.c_str());
                    vehClusterDataMessage->setOrigin(nodeName);

                    sendDown(vehClusterDataMessage->dup());
                    Logger::info("-- sent!", nodeName);

                }
            }
            else {
                Logger::info("-- not for me...", nodeName);
            }
        }
        else { Logger::info("!!!", nodeName); }
    }
}



void VehicleApplication::sendClusterBeaconMessage()
{
    Logger::info("sending a Cluster Beacon Message", nodeName);

    // get mobility data
    double acceleration = traciVehicle->getAcceleration();
    double deceleration = traciVehicle->getDeccel();
    double speed = mobility->getSpeed();
    Coord position = mobility->getPositionAt(simTime());
    Coord direction = mobility->getCurrentDirection();
    double angle = traciVehicle->getAngle();


    // calculate velocities
    double angleRads = angle * (M_PI / 180.0);
    double xVelocity = speed * cos(angleRads);
    double yVelocity = speed * sin(angleRads);
    double velocity = sqrt((xVelocity * xVelocity) + (yVelocity * yVelocity));

    // set message fields
    clusterBeaconMessage->setCarsInRange(neighborTable.getSize());

    clusterBeaconMessage->setAcceleration(acceleration);
    clusterBeaconMessage->setDeceleration(deceleration);

    clusterBeaconMessage->setSpeed(speed);
    clusterBeaconMessage->setVelocity(velocity);
    clusterBeaconMessage->setXVelocity(xVelocity);
    clusterBeaconMessage->setYVelocity(yVelocity);

    clusterBeaconMessage->setXPosition(position.x);
    clusterBeaconMessage->setYPosition(position.y);
    clusterBeaconMessage->setXDirection(direction.x);
    clusterBeaconMessage->setYDirection(direction.y);

    clusterBeaconMessage->setSender(nodeName);


    sendDelayedDown(clusterBeaconMessage->dup(), uniform(0.01, 0.2));
}

void VehicleApplication::sendModelUpdateMessage()
{
    auto [pStateDictJson, vStateDictJson] = agent.getStateDictsAsJson();

    auto pNet = agent.PyObjectToChar(pStateDictJson);
    auto vNet = agent.PyObjectToChar(vStateDictJson);

    BaseApplicationLayer::sendModelUpdateMessage(pNet, vNet, "vehicle");
}

void VehicleApplication::loadModelUpdate(ModelUpdateMessage* appMessage)
{
    findHost()->getDisplayString().setTagArg("i", 1, "purple");

    auto pNetJson = appMessage->getPStateDict();
    auto vNetJson = appMessage->getVStateDict();
    auto origin = appMessage->getOrigin();



    if (std::strcmp(origin, "rsu") == 0)
    {
        Logger::info("-- model from rsu. loading...", nodeName);

        auto [pStateDict, vStateDict] = agent.getStateDictsFromJson(pNetJson, vNetJson);
        agent.loadStateDicts(pStateDict, vStateDict);
    }
    else {
        Logger::info("-- ignoring vehicle request", nodeName);
    }

}

void VehicleApplication::addSelfToNeighborTable()
{
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

    entry.carsInRange = neighborTable.getSize();
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
    auto sender = nodeName;

    // add entry to neighbor table
    Logger::info("-- adding message to neighbor table", nodeName);
    neighborTable.addRow(entry, sender);
}


void VehicleApplication::handleSelfMsg(cMessage* msg)
{
    if (ClusterBeaconMessage* m = dynamic_cast<ClusterBeaconMessage*>(msg)) {
        sendClusterBeaconMessage();
        scheduleAt(simTime() + clusterBeaconDelay, m->dup());
        delete m;
    } else if (NeighborTablePruneMessage* m = dynamic_cast<NeighborTablePruneMessage*>(msg)) {
        Logger::info("pruning neighbor table...", nodeName);
        neighborTable.pruneTable(simTime().dbl());

        Logger::info("-- new neighbor table", nodeName);
        neighborTable.printTable();
        scheduleAt(simTime() + neighborTableTimeout, m->dup());
        delete m;
    } else if (ClusterElectionMessage* m = dynamic_cast<ClusterElectionMessage*>(msg)) {
        Logger::info("holding cluster election...", nodeName);
        // election setup

        /* START TRAINING CODE */

        addSelfToNeighborTable();
        neighborTable.calculateMetadata();

        // step in the environment with observation
        auto observation = agent.toTensor(neighborTable.toList());
        auto [action, value, logp] = agent.step(observation);
        Logger::info("-- step works", nodeName);

        // parse actions into neighbor table

        std::vector<double> doubleData = agent.toDoublesList(action);
        Logger::info("-- tensorToList works", nodeName);

        neighborTable.setWeights(doubleData[0], doubleData[1], doubleData[2], doubleData[3],
                doubleData[4], doubleData[5], doubleData[6], doubleData[7],
                doubleData[8], doubleData[9]);
        Logger::info("-- table weights updated.", nodeName);


        auto reward = agent.toPyFloat(1);
        // must store a transition for localStepsPerEpoch
        agent.bufferStoreTransition(observation, action, reward, value, logp);
        agent.bufferStoreTransition(observation, action, reward, value, logp);
        agent.bufferStoreTransition(observation, action, reward, value, logp);
        agent.bufferStoreTransition(observation, action, reward, value, logp);
        agent.bufferStoreTransition(observation, action, reward, value, logp);

        auto v = agent.toPyFloat(0);
        agent.bufferFinishPath(v);
        Logger::info("-- buffer finish path works: ", nodeName);

        agent.learn();
        Logger::info("-- learn works: ", nodeName);

        /* END TRAINING CODE */

        auto bestCandidateNeighbor = neighborTable.getBestScoringNeighbor();
        neighborTable.printAverages();
        neighborTable.printScores();
        Logger::info("-- winner: " + bestCandidateNeighbor, nodeName);

        // if we won self-election, broadcast
        if (nodeName == bestCandidateNeighbor)
        {
            findHost()->getDisplayString().setTagArg("i", 1, "orange");
            clusterHead = true;
            Logger::info("-- I won! alerting nearby nodes..", nodeName);

            clusterSelectionMessage->setData(nodeName);
            populateWSM(clusterSelectionMessage);
            sendDelayedDown(clusterSelectionMessage->dup(), uniform(0.01, 0.2));
        }
        else
        {
            clusterHead = false;
            findHost()->getDisplayString().setTagArg("i", 1, "transparent");
            Logger::info("-- I lost! keeping that to myself..", nodeName);
        }

        delete m;
    } else {}
}

void VehicleApplication::handlePositionUpdate(cObject* obj)
{
    BaseApplicationLayer::handlePositionUpdate(obj);

    // the vehicle has moved. Code that reacts to new positions goes here.
    // member variables such as currentPosition and currentSpeed are updated in the parent class
}
