#include "msd.h"
#include <iostream>
#include <omp.h>
#include "omp.h"
#include "utility.h"

using namespace std;

MSD::MSD(Group *g, int maxtime) {
  //if (g->isStatic()) {
    mGroup = g;
    mInitialized = false;
    mMaxTime = maxtime;
    mAverage = false;
    mMasCen = false;
    mTimeUnit = 0; //default value 0
    mIntelligent = false;
    mDimX = true;
    mDimY = true;
    mDimZ = true;
  //}
  //else {
  //  throw string("Error! Cannot create MSD for non-static group!");
  //}
}

void MSD::update(Timestep *timestep) {
  if (mInitialized == false) {
    //if (mCoordinates.size() != mGroup->mAtoms.size()) mCoordinates.resize(mGroup->mAtoms.size());
    if (mMasCen == false) {
      mCoordinates.resize(mGroup->size());
      mData.resize(mGroup->size());
    }
    else {
      mCoordinates.resize(1);
      mData.resize(1);
    }
    //cout << "mCoordinates.resize(" << mGroup->mAtoms.size() << ")" << endl;
    //for (int i=0; i<mCoordinates.size(); i++) {
    //  mCoordinates.reserve(100);
    //}
    for (int i=0; i<mData.size(); i++) {
      mData[i].resize(mMaxTime+1);
      //cout << "mData[" << i << "].resize(" << mMaxTime+1 << ")" << endl;
    }
    mInitialized = true;
  }

  if (mGroup->size() == 0) {
    return;
  }


  int startcoordinate=mCoordinates[0].size()-mMaxTime*3; //do not backtrack more than mMaxTime timesteps in the coordinate history
  if (startcoordinate < 0) startcoordinate=0;

  //cout << "starting loop - startcoordinate = " << startcoordinate << " mCOordinates.size() = " << mCoordinates.size() << " maxtime= " << mMaxTime << endl;
  //#pragma omp parallel for schedule(static) default(shared) private(x,y,z,dx,dy,dz,r2,dt)

  for (int i=0; i<mCoordinates.size(); i++) { //loop the atoms in the group
    float x=0,y=0,z=0;
    if (mMasCen == true) {
      for (int j=0; j<mGroup->size(); j++) {
        x+=mGroup->atom(j)->xu;
        y+=mGroup->atom(j)->yu;
        z+=mGroup->atom(j)->zu;
      }
      x/=mGroup->size();
      y/=mGroup->size();
      z/=mGroup->size();
    }
    else {
      x=mGroup->atom(i)->xu; //unwrapped coordinates
      y=mGroup->atom(i)->yu;
      z=mGroup->atom(i)->zu;
    }
    if (mIntelligent == true && mCoordinates[i].size() >= 3) { //if "intelligent", translate the coordinates to around the coordinates of the previous frame.
      //thus, the assumption is that no atom will move more than half the cell length in any direction between two frames that contribute to the msd
      //cout << x << " " << y << " " << z << " ";
      TranslateAtomCoordinatesToVicinity(x,y,z,mCoordinates[i][mCoordinates[i].size()-3],mCoordinates[i][mCoordinates[i].size()-2],mCoordinates[i][mCoordinates[i].size()-1],timestep);
      //cout << x << " " << y << " " << z << " " << endl;
    }
    //#pragma omp parallel for schedule(static) default(shared)
    //cout << "startcoordinate= " << startcoordinate << " " << mCoordinates[i].size() << endl;
    for (int j=startcoordinate; j<mCoordinates[i].size(); j+=3) { //loop the coordinates for this atom
      float dx=x-mCoordinates[i][j];
      float dy=y-mCoordinates[i][j+1];
      float dz=z-mCoordinates[i][j+2];
      //float r2=dx*dx+dy*dy+dz*dz;
      float r2=0;
      if (mDimX) r2+=dx*dx;
      if (mDimY) r2+=dy*dy;
      if (mDimZ) r2+=dz*dz;
      int dt=(mCoordinates[i].size()-j)/3;
      mData[i][dt].f+=r2;
      //cout << "mData[" << i << "][" << dt << "].f+=" << r2 << endl;
      mData[i][dt].i++;
      //cout << omp_get_thread_num() << endl;
    }
    mCoordinates[i].push_back(x);
    mCoordinates[i].push_back(y);
    mCoordinates[i].push_back(z);
    //cout << "pushing back " << x << " " << y << " " << z << endl;
  }
}

void MSD::print(ofstream * o, Timestep *t) {
  if (mAverage == false) { //print msd for all atoms in the group individually
    for (int i=0; i<mMaxTime; i++) {
      if (mTimeUnit == 0) (*o) << i << " ";
      else (*o) << i*mTimeUnit << " ";
      
      for (int j=0; j<mData.size(); j++) {
        if (mData[j][i].i == 0) (*o) << "?0 "; //if there is no data for this time, output a ?
        else (*o) << mData[j][i].f/mData[j][i].i << " ";
      }
      (*o) << endl;
    }
  }
  else { //average msd for all atoms in the group and print only one entry.
    float s=0; int numcont=0; //numcont = contributions
    if (mTimeUnit != 0) {
      (*o) << "#time(ps) displacement-squared(A^2) diffusion-coefficient(m^2/s)" << endl;
    }
    else {
      (*o) << "#time(in-units-of-time-between-frames) displacement-squared(A^2)" << endl;
    }
    for (int i=0; i<mMaxTime; i++) {
      s=0; numcont=0;
      for (int j=0; j<mData.size(); j++) {
        s+=mData[j][i].f;
        numcont+=mData[j][i].i;
      }
      if (numcont > 0) {
        if (mTimeUnit == 0) { 
          (*o) << i << " " << s/numcont << endl;
        }
        else {
          (*o) << i*mTimeUnit << " " << s/numcont << " " << (1e-20*s/numcont)/(1e-12*i*mTimeUnit*6) << endl; //1e-20 = angstrom^2 to m^2, 1e-12 = picosec to sec, the 6 is from 2*3
        }
      }

      //else (*o) << i << " ?0" << endl;
    }
  }

}

    
