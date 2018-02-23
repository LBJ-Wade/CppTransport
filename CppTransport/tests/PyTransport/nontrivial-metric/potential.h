//#This file is part of PyTransport.

//#PyTransport is free software: you can redistribute it and/or modify
//#it under the terms of the GNU General Public License as published by
//#the Free Software Foundation, either version 3 of the License, or
//#(at your option) any later version.

//#PyTransport is distributed in the hope that it will be useful,
//#but WITHOUT ANY WARRANTY; without even the implied warranty of
//#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//#GNU General Public License for more details.

//#You should have received a copy of the GNU General Public License
//#along with PyTransport.  If not, see <http://www.gnu.org/licenses/>.

// This file contains a prototype of the potential.h file of PyTransport -- it is edited by the PyTransScripts module

#ifndef POTENTIAL_H  // Prevents the class being re-defined
#define POTENTIAL_H


#include <iostream>
#include <math.h>
#include <cmath>
#include <vector>

using namespace std;

// #Rewrite
// Potential file rewriten at Mon Jul 17 22:25:04 2017

class potential
{
private:
	int nF; // field number
	int nP; // params number which definFs potential
    
    
public:
	// flow constructor
	potential()
	{
// #FP
nF=2;
nP=5;

//        p.resize(nP);
        
// pdef

    }
	
    //void setP(vector<double> pin){
    //    p=pin;
    //}
	//calculates V()
	double V(vector<double> f, vector<double> p)
	{
		double sum ;
        
// Pot

 sum=0.5*pow(f[0], 2)*pow(p[3], 2.0) + 0.5*pow(f[1], 2.0)*pow(p[4], 2);
         return sum;
	}
	
	//calculates V'()
	vector<double> dV(vector<double> f, vector<double> p)
	{
		vector<double> sum(nF,0.0);
	
// dPot

 sum[0]=1.0*f[0]*pow(p[3], 2.0);

 sum[1]=1.0*pow(f[1], 1.0)*pow(p[4], 2);
        
		return sum;
	}
    
	// calculates V''
	vector<double> dVV(vector<double> f, vector<double> p)
	{
		vector<double> sum(nF*nF,0.0);
		
// ddPot

 sum[0+nF*0]=-4.0*f[0]*pow(p[0], 2)*pow(p[3], 2.0)*sinh(2.0*(f[0] - p[1])/p[2])*pow(cosh(2.0*(f[0] - p[1])/p[2]), -3.0)*pow(cosh((2.0*f[0] - 2.0*p[1])/p[2]), -2.0)/(p[2]*(-pow(p[0], 2)*pow(cosh((2.0*f[0] - 2.0*p[1])/p[2]), -4.0) + 1)) + 4.0*pow(f[1], 1.0)*p[0]*pow(p[4], 2)*sinh(2.0*(f[0] - p[1])/p[2])*pow(cosh(2.0*(f[0] - p[1])/p[2]), -3.0)/(p[2]*(-pow(p[0], 2)*pow(cosh((2.0*f[0] - 2.0*p[1])/p[2]), -4.0) + 1.0)) + 1.0*pow(p[3], 2.0);

 sum[1+nF*1]=1.0*pow(p[4], 2);
     
        return sum;
	}
    
	// calculates V'''
	vector<double> dVVV(vector<double> f, vector<double> p)
	{
        vector<double> sum(nF*nF*nF,0.0);
// dddPot

 sum[0+nF*0+nF*nF*0]=-1.0*f[0]*pow(p[3], 2.0)*(4.0*pow(p[0], 2)*(2.0*pow(p[0], 2)*sinh(2.0*f[0]/p[2] - 2.0*p[1]/p[2]) - 10.0*sinh(2.0*f[0]/p[2] - 2.0*p[1]/p[2])*pow(cosh(2.0*f[0]/p[2] - 2.0*p[1]/p[2]), 4.0))*sinh(2.0*f[0]/p[2] - 2.0*p[1]/p[2])/pow(-pow(p[0], 2)*p[2]*pow(cosh(2.0*f[0]/p[2] - 2.0*p[1]/p[2]), 1.0) + p[2]*pow(cosh(2.0*f[0]/p[2] - 2.0*p[1]/p[2]), 5.0), 2) + 8.0*pow(p[0], 2)*cosh(2.0*f[0]/p[2] - 2.0*p[1]/p[2])/(p[2]*(-pow(p[0], 2)*p[2]*pow(cosh(2.0*f[0]/p[2] - 2.0*p[1]/p[2]), 1.0) + p[2]*pow(cosh(2.0*f[0]/p[2] - 2.0*p[1]/p[2]), 5.0)))) - 1.0*pow(f[1], 1.0)*pow(p[4], 2)*(-4.0*p[0]*(-2.0*pow(p[0], 2)*sinh(2.0*f[0]/p[2] - 2.0*p[1]/p[2])*pow(cosh(2.0*f[0]/p[2] - 2.0*p[1]/p[2]), -2.0) - 6.0*sinh(2.0*f[0]/p[2] - 2.0*p[1]/p[2])*pow(cosh(2.0*f[0]/p[2] - 2.0*p[1]/p[2]), 2.0))*sinh(2.0*f[0]/p[2] - 2.0*p[1]/p[2])/pow(-pow(p[0], 2)*p[2]*1.0/cosh(2.0*f[0]/p[2] - 2.0*p[1]/p[2]) + 1.0*p[2]*pow(cosh(2.0*f[0]/p[2] - 2.0*p[1]/p[2]), 3.0), 2) - 8.0*p[0]*cosh(2.0*f[0]/p[2] - 2.0*p[1]/p[2])/(p[2]*(-pow(p[0], 2)*p[2]*1.0/cosh(2.0*f[0]/p[2] - 2.0*p[1]/p[2]) + 1.0*p[2]*pow(cosh(2.0*f[0]/p[2] - 2.0*p[1]/p[2]), 3.0)))) - 4.0*pow(p[0], 2)*pow(p[3], 2.0)*sinh(2.0*f[0]/p[2] - 2.0*p[1]/p[2])/(-pow(p[0], 2)*p[2]*pow(cosh(2.0*f[0]/p[2] - 2.0*p[1]/p[2]), 1.0) + p[2]*pow(cosh(2.0*f[0]/p[2] - 2.0*p[1]/p[2]), 5.0)) - 8.0*pow(p[0], 2)*(-4.0*f[0]*pow(p[0], 2)*pow(p[3], 2.0)*sinh(2.0*(f[0] - p[1])/p[2])*pow(cosh(2.0*(f[0] - p[1])/p[2]), -3.0)*pow(cosh((2.0*f[0] - 2.0*p[1])/p[2]), -2.0)/(p[2]*(-pow(p[0], 2)*pow(cosh((2.0*f[0] - 2.0*p[1])/p[2]), -4.0) + 1)) + 4.0*pow(f[1], 1.0)*p[0]*pow(p[4], 2)*sinh(2.0*(f[0] - p[1])/p[2])*pow(cosh(2.0*(f[0] - p[1])/p[2]), -3.0)/(p[2]*(-pow(p[0], 2)*pow(cosh((2.0*f[0] - 2.0*p[1])/p[2]), -4.0) + 1.0)) + 1.0*pow(p[3], 2.0))*sinh(2.0*(f[0] - p[1])/p[2])*pow(cosh(2.0*(f[0] - p[1])/p[2]), -3.0)*pow(cosh((2.0*f[0] - 2.0*p[1])/p[2]), -2.0)/(p[2]*(-pow(p[0], 2)*pow(cosh((2.0*f[0] - 2.0*p[1])/p[2]), -4.0) + 1));

 sum[0+nF*0+nF*nF*1]=4.0*p[0]*pow(p[4], 2)*sinh(2.0*f[0]/p[2] - 2.0*p[1]/p[2])/(-pow(p[0], 2)*p[2]*1.0/cosh(2.0*f[0]/p[2] - 2.0*p[1]/p[2]) + 1.0*p[2]*pow(cosh(2.0*f[0]/p[2] - 2.0*p[1]/p[2]), 3.0));

 sum[0+nF*1+nF*nF*0]=4.0*p[0]*pow(p[4], 2)*sinh(2.0*(f[0] - p[1])/p[2])*pow(cosh(2.0*(f[0] - p[1])/p[2]), -3.0)/(p[2]*(-pow(p[0], 2)*pow(cosh((2.0*f[0] - 2.0*p[1])/p[2]), -4.0) + 1.0));

 sum[1+nF*0+nF*nF*0]=4.0*p[0]*pow(p[4], 2)*sinh(2.0*(f[0] - p[1])/p[2])*pow(cosh(2.0*(f[0] - p[1])/p[2]), -3.0)/(p[2]*(-pow(p[0], 2)*pow(cosh((2.0*f[0] - 2.0*p[1])/p[2]), -4.0) + 1.0));
       
        return sum;
	}
    
    int getnF()
    {
        return nF;
    }
    
    int getnP()
    {
        return nP;
    }

};
#endif