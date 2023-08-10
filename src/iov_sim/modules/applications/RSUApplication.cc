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

#include "iov_sim/modules/applications/RSUApplication.h"

using namespace veins;
using namespace std;
using namespace iov_sim;

Define_Module(iov_sim::RSUApplication);

RSUApplication::RSUApplication()
    : aggregator() { }

void RSUApplication::initialize(int stage)
{
    BaseApplicationLayer::initialize(stage);

    if (stage == 0) {
        // add rsu
        policyLoad = par("policyLoad").stringValue();
        valueLoad = par("valueLoad").stringValue();
        policySave = par("policySave").stringValue();
        valueSave = par("valueSave").stringValue();

        if (par("loadModel").boolValue())
        {
            aggregator.loadStateDict(policyLoad, valueLoad);
        }
    }
    else if (stage == 1) {
        modelAggTimer = new Timer("Model Aggregation Timer");
        modelAggTimer->setTime(120);
        modelAggWindow = new Timer("Model Aggregation Window");
        modelAggWindow->setTime(5);

        scheduleAt(simTime() + modelAggTimer->getTime(), modelAggTimer);
    }
}

void RSUApplication::finish()
{
    BaseApplicationLayer::finish();

    if (par("saveModel").boolValue())
    {
        aggregator.saveStateDict(policySave, valueSave);
    }

    if (modelAggTimer->isScheduled())
    {
        cancelEvent(modelAggTimer);
    }

    if (modelAggWindow->isScheduled())
    {
        cancelEvent(modelAggWindow);
    }

    delete modelAggTimer;
    delete modelAggWindow;
}

void RSUApplication::onModelMsg(BaseMessage* msg)
{
   if (ModelRequest* requestMsg = dynamic_cast<ModelRequest*>(msg)) {
        // RSU has received an initialize message from a vehicle -> they need an
        // updated model. Respond with ModelUpdateMessage (message containing
        // up-to-date AI model)
        if (std::strcmp(requestMsg->getOrigin(), "rsu") != 0
                && std::strcmp(requestMsg->getDestination(), "rsu") == 0)
        {
            Logger::info(string("received a Model Request Message from ") + requestMsg->getOrigin(),
                    nodeName);

            findHost()->getDisplayString().setTagArg("i", 1, "green");

            Logger::info("-- request from vehicle. sending...", nodeName);
            Logger::info("-- sending model request message!", nodeName);

            // get state_dicts as json-formatted PyObject string
            auto jsonStateDicts = aggregator.getStateDictsAsJson();

            // convert PyObject to const char*
            auto pNet = aggregator.PyObjectToChar(jsonStateDicts.first);
            auto vNet = aggregator.PyObjectToChar(jsonStateDicts.second);

            ModelInit *msg = new ModelInit();
            populateWSM(msg);


            msg->setData("requesting model");
            msg->setOrigin("rsu");
            msg->setDestination(requestMsg->getOrigin());
            msg->setPStateDict(pNet);
            msg->setVStateDict(vNet);

            sendDown(msg);
        }
    }
    else if (ModelUpdate* updateMsg = dynamic_cast<ModelUpdate*>(msg)) {
        if (std::strcmp(updateMsg->getDestination(), "rsu") == 0)
        {
            Logger::info("received a Model Update Message", nodeName);

            findHost()->getDisplayString().setTagArg("i", 1, "purple");

            auto stateDicts = aggregator.getStateDictsFromJson(
                    updateMsg->getPStateDict(), updateMsg->getVStateDict());

            pStateDicts.push_back(stateDicts.first);
            vStateDicts.push_back(stateDicts.second);
        }
    }
   else { }
}

void RSUApplication::sendModelUpdateMessage(const char* destination)
{
    // get state_dicts as json-formatted PyObject string
    auto jsonStateDicts = aggregator.getStateDictsAsJson();

    // convert PyObject to const char*
    auto pNet = aggregator.PyObjectToChar(jsonStateDicts.first);
    auto vNet = aggregator.PyObjectToChar(jsonStateDicts.second);

    // send message
    BaseApplicationLayer::sendModelUpdateMessage(destination, pNet, vNet);
    Logger::info("-- up-to-date model sent!", nodeName);
}

void RSUApplication::handleSelfMsg(cMessage *msg) {
    if (msg == modelAggTimer) {
        Logger::info("Sending model aggregation request!", nodeName);

        ModelRequest *msg = new ModelRequest();
        populateWSM(msg);

        msg->setData("requesting model");
        msg->setOrigin("rsu");

        sendDown(msg);
        scheduleAt(simTime() + modelAggWindow->getTime(), modelAggWindow);

    }  else if (msg == modelAggWindow) {
        Logger::info("End of model agg rcv window; aggregating...",
                nodeName);
        // convert vector to python list
        if (pStateDicts.size() == 0)
        {
            Logger::info("No models received.. Starting agg timer back up",
                    nodeName);
            findHost()->getDisplayString().setTagArg("i", 1, "");
            scheduleAt(simTime() + modelAggTimer->getTime(), modelAggTimer);
            return;
        }

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
        aggregator.loadStateDicts(aggregator.getAggregatorClass(),
                models.first, models.second);

        Logger::info("Aggregation successful. Disseminating newly updated model.",
                nodeName);

        // get state_dicts as json-formatted PyObject string
        auto jsonStateDicts = aggregator.getStateDictsAsJson();

        // convert PyObject to const char*
        auto pNet = aggregator.PyObjectToChar(jsonStateDicts.first);
        auto vNet = aggregator.PyObjectToChar(jsonStateDicts.second);

        BaseApplicationLayer::sendModelUpdateMessage("all", pNet, vNet);

        pStateDicts.clear();
        vStateDicts.clear();

        findHost()->getDisplayString().setTagArg("i", 1, "");
        scheduleAt(simTime() + modelAggTimer->getTime(), modelAggTimer);
    } else {
        Logger::error(
                string("unknown message type: ") + msg->getClassAndFullName(),
                nodeName);
    }
}

