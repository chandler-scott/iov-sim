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

#pragma once
#include "iov_sim/iov_sim.h"
#include "iov_sim/base/applications/BaseApplicationLayer.h"
#include "iov_sim/base/python/AggregatorWrapper.h"
#include "iov_sim/modules/messages/model/ModelInit_m.h"
#include "iov_sim/modules/messages/model/RSUCheckModelInit_m.h"
#include "iov_sim/base/messages/ModelInitTimer_m.h"


using namespace veins;
namespace iov_sim {

class IOV_SIM_API RSUApplication : public BaseApplicationLayer {
public:
    RSUApplication();

    void initialize(int stage) override;
    void finish() override;


protected:
    void onModelMsg(BaseMessage* msg) override;

    void sendModelInitMessage(const char* destination);
    void sendModelUpdateMessage(const char* destination);
    void sendRSUCheckModelInitMessage(const char* requestingNode, double signalStrength);

    void handleSelfMsg(cMessage* msg) override;

private:
    AggregatorWrapper aggregator;
    std::string policyLoad;
    std::string valueLoad;
    std::string policySave;
    std::string valueSave;

    vector<std::pair<std::string, double>> requestingNodes;


    /* @brief Timer for window to rcv model update */
    Timer *modelAggTimer = nullptr;
    Timer *modelAggWindow = nullptr;


    vector<PyObject*> pStateDicts;
    vector<PyObject*> vStateDicts;
};

} // namespace iov_sim
