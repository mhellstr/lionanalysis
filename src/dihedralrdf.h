#ifndef DIHEDRALRDF_H
#define DIHEDRALRDF_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "timestep.h"
#include "atom.h"
#include "group.h"
#include "utility.h"

using namespace std;

//typedef vector<int> IntVector; //defined in anglerdf.h

class DihedralRDF : public CObject {
public:
  DihedralRDF();
  DihedralRDF(Group *, float, float, Group *, Group *, float, float, Group*, float, float, float, float, float);
  void print(ofstream *, const Timestep *);
  void update(const std::vector<Atom> *, const Timestep *) override;
  float mGroup1MinDist;
  float mGroup1MaxDist;
  float mGroup3MinDist;
  float mGroup3MaxDist;
  float mGroup4MinDist;
  float mGroup4MaxDist;
  float mAngleResolution;
  float mMinAngle;
  float mMaxAngle;
  Group *mGroup1;
  Group *mGroup2; //the center group!
  Group *mGroup3;
  Group *mGroup4;
  vector<float> mValues;
  //float coord(int, float, float, float);
};

#endif
