#ifndef MSD_H
#define MSD_H

#include "timestep.h"
//#include "cgroup.h"
#include "group.h"
#include <vector>
#include <fstream>

using namespace std;


class FloatAndInt;
class MSD;

typedef vector<float> floatvec;
typedef vector<FloatAndInt> floatandintvec;
typedef vector<floatandintvec> floatandintvecvec;

class FloatAndInt {
public:
  FloatAndInt() :f(0),i(0) { }
  float f;
  int i;
};

class MSD {
public:
  MSD(Group *, int);
  Group *mGroup;
  vector<floatvec> mCoordinates; //one element for each atom in the group, one triad of values (x,y,z) for the atom for each timestep
  float getx(int atom, int timestep) { return mCoordinates[atom][(timestep-1)*3]; }
  float gety(int atom, int timestep) { return mCoordinates[atom][(timestep-1)*3+1]; }
  float getz(int atom, int timestep) { return mCoordinates[atom][(timestep-1)*3+1]; }
  vector<floatandintvec> mData; //one elemen for each atom in the group, then there is one element per timelength, and there the float is the sum of squared displacements, the integer is the number of elements in the sum. together they give the average = MSD
  void print(ofstream *, Timestep *);
  int mMaxTime;
  void update(Timestep *timestep=nullptr);
  bool mInitialized;
  bool mAverage; //whether to average all MSDs... useful if all atoms in the group are equivalent
  bool mMasCen; //whether to calculate the MSD for the center of mass for the atoms in the group. useful for particle adsorbates, for example.
  void setAverage(bool a=true) { mAverage = a; }
  void setMasCen(bool a=true) { mMasCen = a; }
  float mTimeUnit; //in picosec
  void setTimeUnit(float x) { mTimeUnit = x; }
  bool mIntelligent; //whether to be "intelligent", useful if one has forgotten to print the unwrapped coordinates.
  void setIntelligent(bool a=true) { mIntelligent = a; }
  bool mDimX, mDimY, mDimZ; //whether to include component along the x (y,z) axis
  void setDimX(bool a=true) { mDimX = a; }
  void setDimY(bool a=true) { mDimY = a; }
  void setDimZ(bool a=true) { mDimZ = a; }
};




#endif
