#ifndef VAC_H
#define VAC_H

#include "timestep.h"
#include "group.h"
#include "msd.h"
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;


class VAC;

class VAC {
public:
  VAC(Group *, int);
  ~VAC();
  Group *mGroup;
  vector<floatvec> mVelocities; //one element for each atom in the group, one triad of values (x,y,z) for the atom for each timestep
  vector<floatvec> mLastPositions; //for the calculation of velocities if they weren't printed in the output
  //float getx(int atom, int timestep) { return mCoordinates[atom][(timestep-1)*3]; }
  //float gety(int atom, int timestep) { return mCoordinates[atom][(timestep-1)*3+1]; }
  //float getz(int atom, int timestep) { return mCoordinates[atom][(timestep-1)*3+1]; }
  //vector<floatandintvec> mData; 
  vector<float> mData; 
  vector<int> mDataCount; 
  void print(ofstream *, Timestep *);
  int mMaxTime;
  void update(Timestep *timestep=nullptr);
  bool mInitialized;
  //bool mAverage; //whether to average all MSDs... useful if all atoms in the group are equivalent
  //bool mMasCen; //whether to calculate the MSD for the center of mass for the atoms in the group. useful for particle adsorbates, for example.
  //void setAverage(bool a=true) { mAverage = a; }
  //void setMasCen(bool a=true) { mMasCen = a; }
  float mTimeUnit; //in picosec
  void setTimeUnit(float x) { mTimeUnit = x; }
  bool mIntelligent; //whether to be "intelligent", useful if one has forgotten to print the unwrapped coordinates.
  void setIntelligent(bool a=true) { mIntelligent = a; }
  bool mDimX, mDimY, mDimZ; //whether to include component along the x (y,z) axis
  void setDimX(bool a=true) { mDimX = a; }
  void setDimY(bool a=true) { mDimY = a; }
  void setDimZ(bool a=true) { mDimZ = a; }
  void setCalculateVelocities( bool a=true) {mCalculateVelocities=a;}
  bool mCalculateVelocities;
  void calculateVelocities(float, float, float, float, float, float, vector<float>&);
  bool mDoFT;
  void setDoFT(bool a=true) {mDoFT=a;}
  void printFT(ofstream *o, Timestep *timestep=nullptr);
  void normalize();
  void setPadd(int a) {mPadd=a;}
  void setSigmaFraction(float a) {mSigmaFraction=a;}
  int mPadd; //padding of zeroes to the VACF for the FT
  float mSigmaFraction; //gaussian smoothing of the VACF for the FT... this should be in fractional units of mMaxTime... the timteatro script divides by 2.50, which corresponds to mSigmaFraction = 0.4
  bool mContinuousMembers;
  void setContinuousMembers(bool a=true) { mContinuousMembers=a; }
  void update_with_continuous_members(Timestep *);
  vector<int> mEntered;
  //void intelligenceTest() {
  //  if (mIntelligent) cout << " Yes I am intelligent " << endl;
  //  else cout << "No I am not intelligent" << endl;
  //}
};




#endif
