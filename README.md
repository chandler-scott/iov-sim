# IoV Federated Learning #

Implementing federated deep reinforcement learning to enable cluster based communication in IoV.


## Proximal Policy Optimization Details ## 
### Observation ### 
The agent observes averages for each cluster table value and node connectivity every 15 seconds and then selects the optimal action.

### Reward ###
The reward is comprised of the connectivity, time spent in a cluster, cluster head successful heartbeat percentage, and cluster member heartbeat percentage. Maximum score at time of writing is 45.

### Action ###
This simulation outputs 40 parameter values (ranging from -1 to 1) to set the values for neighbor score weights, election holder score weights, cluster join score weights, connectivity threshold for reelection, election score threshold for joining an election, and cluster score threshold for joining an existing cluster


## Supported program versions ##

- Veins 5.2 (see <http://veins.car2x.org/>)
- OMNeT++ 5.6.2 (see <https://omnetpp.org/>)

## License ##

Veins is composed of many parts. See the version control log for a full list of
contributors and modifications. Each part is protected by its own, individual
copyright(s), but can be redistributed and/or modified under an open source
license. License terms are available at the top of each file. Parts that do not
explicitly include license text shall be assumed to be governed by the "GNU
General Public License" as published by the Free Software Foundation -- either
version 2 of the License, or (at your option) any later version
(SPDX-License-Identifier: GPL-2.0-or-later). Parts that are not source code and
do not include license text shall be assumed to allow the Creative Commons
"Attribution-ShareAlike 4.0 International License" as an additional option
(SPDX-License-Identifier: GPL-2.0-or-later OR CC-BY-SA-4.0). Full license texts
are available with the source distribution.



