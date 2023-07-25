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

#include "VehicleApplication.h"

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
        EV << "Initializing " << par("appName").stringValue() << std::endl;
        clusterBeaconDelay = par("clusterBeaconDelay");
        neighborTableTimeout = par("neighborTableTimeout");

    }
    else if (stage == 1) {
        // Initializing members that require initialized other modules goes here
        std::cout << nodeName << ": sending Model Request Message" << std::endl;
        ModelRequestMessage* wsm = new ModelRequestMessage();
        populateWSM(wsm);
        wsm->setData("Hi there!");
        sendDown(wsm);

        clusterBeaconSelfMessage = new ClusterBeaconMessage("ClusterBeaconMessage");
        neighborTablePruneMessage = new NeighborTablePruneMessage("NeighborTablePruneMessage");
        scheduleAt(simTime() + clusterBeaconDelay, clusterBeaconSelfMessage);
        scheduleAt(simTime() + neighborTableTimeout, neighborTablePruneMessage);
    }
}

void VehicleApplication::finish()
{
    BaseApplicationLayer::finish();
    // statistics recording goes here
}

void VehicleApplication::onBSM(BaseFrame1609_4* bsm)
{
    if (ClusterBeaconMessage* beaconMsg = dynamic_cast<ClusterBeaconMessage*>(bsm)) {
        std::cout << nodeName << ": received a Cluster Beacon Message" << std::endl;

        findHost()->getDisplayString().setTagArg("i", 1, "green");

        NeighborEntry entry;

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
        entry.timestamp = simTime().dbl();
        auto sender = beaconMsg->getSender();

        // add entry to neighbor table
        std::cout << "-- adding message to neighbor table" << std::endl;
        neighborTable.addRow(entry, sender);
        neighborTable.printTable();

    }
    else if (ModelRequestMessage* requestMsg = dynamic_cast<ModelRequestMessage*>(bsm)) {
        std::cout << nodeName << ": received a Model Request Message" << std::endl;
        findHost()->getDisplayString().setTagArg("i", 1, "blue");
        auto origin = requestMsg->getOrigin();

        if (std::strcmp(origin, "rsu") == 0)
        {
            std::cout << "-- rsu beacon" << std::endl;
            sendModelUpdateMessage();
            std::cout << "-- sending local up-to-date model to rsu." << std::endl;


        }
        else {
            findHost()->getDisplayString().setTagArg("i", 1, "");

            std::cout << "-- ignoring vehicle beacon" << std::endl;
        }

    }
    else {
        std::cout << nodeName << ": received a different beacon message than expected.." << std::endl;
        delete bsm;
    }
}

void VehicleApplication::onWSM(BaseFrame1609_4* frame)
{
    if (frame) {
        if (dynamic_cast<ModelUpdateMessage*>(frame)) {
            std::cout << nodeName << ": received a Model Update Message" << std::endl;
            ModelUpdateMessage* appMessage = check_and_cast<ModelUpdateMessage*>(frame);

            // load up-to-date model
            loadModelUpdate(appMessage);
            std::cout << "-- loaded most up-to-date model" << std::endl;
        }
        else {
            delete frame;
        }
    }
}



void VehicleApplication::sendClusterBeaconMessage()
{
    std::cout << nodeName << ": sending a Cluster Beacon Message" << std::endl;

    ClusterBeaconMessage* msg = new ClusterBeaconMessage();
    populateWSM(msg);

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
    msg->setAcceleration(acceleration);
    msg->setDeceleration(deceleration);

    msg->setSpeed(speed);
    msg->setVelocity(velocity);
    msg->setXVelocity(xVelocity);
    msg->setYVelocity(yVelocity);

    msg->setXPosition(position.x);
    msg->setYPosition(position.y);
    msg->setXDirection(direction.x);
    msg->setYDirection(direction.y);

    msg->setSender(nodeName);


    sendDelayedDown(msg->dup(), uniform(0.01, 0.2));
    delete msg;
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
        std::cout << "-- model from rsu. loading..." << std::endl;
        auto [pStateDict, vStateDict] = agent.getStateDictsFromJson(pNetJson, vNetJson);
        agent.loadStateDicts(pStateDict, vStateDict);
    }
    else {
        std::cout << "-- ignoring vehicle request" << std::endl;
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
    std::cout << "-- adding message to neighbor table" << std::endl;
    neighborTable.addRow(entry, sender);
}


void VehicleApplication::handleSelfMsg(cMessage* msg)
{
    if (msg == clusterBeaconSelfMessage) {
        sendClusterBeaconMessage();
        scheduleAt(simTime() + clusterBeaconDelay, msg);
    } else if (msg == neighborTablePruneMessage) {
        std::cout << nodeName << ": pruning neighbor table..." << std::endl;
        neighborTable.pruneTable(simTime().dbl());
        addSelfToNeighborTable();

        std::cout << "-- scoring  neighbors..." << std::endl;
        neighborTable.scoreNeighbors();

        std::cout << "-- new neighbor table:" << std::endl;
        neighborTable.printTable();
        neighborTable.printAverages();
        neighborTable.printScores();

        scheduleAt(simTime() + neighborTableTimeout, msg);
    } else {}
}

void VehicleApplication::handlePositionUpdate(cObject* obj)
{
    BaseApplicationLayer::handlePositionUpdate(obj);

    // the vehicle has moved. Code that reacts to new positions goes here.
    // member variables such as currentPosition and currentSpeed are updated in the parent class
}
