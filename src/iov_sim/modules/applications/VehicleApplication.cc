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
    }
    else if (stage == 1) {
        // Initializing members that require initialized other modules goes here
        std::cout << "Vehicle: sending Model Request Message" << std::endl;
        ModelRequestMessage* wsm = new ModelRequestMessage();
        populateWSM(wsm);
        wsm->setData("Hi there!");
        sendDown(wsm);

        clusterBeaconSelfMessage = new ClusterBeaconMessage("ClusterBeaconMessage");
        scheduleAt(simTime() + clusterBeaconDelay, clusterBeaconSelfMessage);
        //sendClusterBeaconMessage();
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
        std::cout << "Vehicle: received a Cluster Beacon Message" << std::endl;

        findHost()->getDisplayString().setTagArg("i", 1, "green");

        double speed = beaconMsg->getSpeed();
        double velocity = beaconMsg->getVelocity();
        double xVelocity = beaconMsg->getXVelocity();
        double yVelocity = beaconMsg->getYVelocity();
        double acceleration = beaconMsg->getAcceleration();
        double decceleration = beaconMsg->getDecceleration();
        double xPosition = beaconMsg->getXPosition();
        double yPosition = beaconMsg->getYPosition();
        double xDirection = beaconMsg->getXDirection();
        double yDirection = beaconMsg->getYDirection();

        // add entry to neighbor table
        std::cout << "-- adding message to neighbor table" << std::endl;

    }
    else if (ModelRequestMessage* requestMsg = dynamic_cast<ModelRequestMessage*>(bsm)) {
        std::cout << "Vehicle: received a Model Request Message" << std::endl;
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
        std::cout << "RSU: received a different beacon message than expected.." << std::endl;
        delete bsm;
    }
}

void VehicleApplication::onWSM(BaseFrame1609_4* frame)
{
    if (frame) {
        if (dynamic_cast<ModelUpdateMessage*>(frame)) {
            std::cout << "Vehicle: received a Model Update Message" << std::endl;
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
    std::cout << "Vehicle: sending a Cluster Beacon Message" << std::endl;

    ClusterBeaconMessage* msg = new ClusterBeaconMessage();
    populateWSM(msg);

    // get mobility data
    double acceleration = traciVehicle->getAcceleration();
    double decceleration = traciVehicle->getDeccel();
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
    msg->setDecceleration(decceleration);

    msg->setSpeed(speed);
    msg->setVelocity(velocity);
    msg->setXVelocity(xVelocity);
    msg->setYVelocity(yVelocity);

    msg->setXPosition(position.x);
    msg->setYPosition(position.y);
    msg->setXDirection(direction.x);
    msg->setYDirection(direction.y);


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

void VehicleApplication::onWSA(DemoServiceAdvertisment* wsa)
{
    std::cout << "ApplicationLayerTest: omWSA" << std::endl;
    // Your application has received a service advertisement from another car or RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
}

void VehicleApplication::handleSelfMsg(cMessage* msg)
{
    if (msg == clusterBeaconSelfMessage) {
        sendClusterBeaconMessage();
        scheduleAt(simTime() + clusterBeaconDelay, msg);
    } else {
        // Handle other messages here...
    }
}

void VehicleApplication::handlePositionUpdate(cObject* obj)
{
    BaseApplicationLayer::handlePositionUpdate(obj);

    // the vehicle has moved. Code that reacts to new positions goes here.
    // member variables such as currentPosition and currentSpeed are updated in the parent class
}
