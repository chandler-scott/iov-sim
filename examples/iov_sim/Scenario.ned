//
// Copyright (C) 2017 Christoph Sommer <sommer@ccs-labs.org>
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

package iov_sim.examples.iov_sim;
import org.car2x.veins.nodes.RSU;
import org.car2x.veins.subprojects.iov_sim.base.python.PythonLifecycle;


network Scenario extends org.car2x.veins.nodes.Scenario
{
    parameters:
        int n = default(12);
		rsu[0].mobility.y = 2500;
		rsu[2].mobility.y = 2500;
			
		rsu[6].mobility.y = 3750;	
		// high y coord
		rsu[0].mobility.x = 4000;
		rsu[1].mobility.y = 1500;
		rsu[1].mobility.x = 6750;
		rsu[2].mobility.x = 9000;
		// middle y coord
		rsu[3].mobility.y = 4000;
		rsu[3].mobility.x = 3500;
		rsu[4].mobility.y = 3350;
		rsu[4].mobility.x = 5200;
		rsu[5].mobility.y = 3850;
		rsu[5].mobility.x = 7600;
		
		rsu[6].mobility.x = 10000;
		// low y coord
	    rsu[7].mobility.y = 6000;
		rsu[7].mobility.x = 2750;
		rsu[8].mobility.y = 5750;
		rsu[8].mobility.x = 5750;
		rsu[9].mobility.y = 5500;
		rsu[9].mobility.x = 9000;
		
		rsu[10].mobility.y = 3450;
		rsu[10].mobility.x = 6200;
		rsu[11].mobility.y = 1350;
		rsu[11].mobility.x = 3250;
		rsu[*].mobility.z = 3;
        
        
    submodules:
        rsu[n]: RSU {
            @display("i=veins/sign/yellowdiamond;is=vs"); 
        }
        MySimulationModule: PythonLifecycle {
            @display("p=100,100;i=block/green");
        }
        

}
