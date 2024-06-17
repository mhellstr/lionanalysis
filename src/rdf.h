#ifndef RDF_H
#define RDF_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "timestep.h"
#include "atom.h"
#include "group.h"
#include "cobject.h"

using namespace std;

class RDF : public CObject {
public:
  RDF();
  ~RDF();
  RDF(float,float,float);
  void print(ofstream *, const Timestep *);
  void update(const vector<Atom> *, const Timestep *timestep) override;
  void setPeriodicImages(bool b=true) { mUsePeriodicImages = b; }
  void setPrintHeader(bool b=true) {mPrintHeader = b; }
  void setFromGroup(Group *g) {anchorgroup=g;}
  void setToGroup(Group *g) {targetgroup=g;}
  void setMinDist(float a) {mMinDist=a;}
  void setMaxDist(float a) {mMaxDist=a;}
  void setResolution(float a) {mResolution=a;}
protected:
  bool mPrintHeader;
  bool mUsePeriodicImages;
  //bool autoscale;
  float mMinDist;
  float mMaxDist;
  float mResolution;
  Group *targetgroup;
  Group *anchorgroup;
  int mTotCountTargetGroup; //count the number of atoms in the group in order to average at the end for the normalization
  int mTotCountAnchorGroup; 
  int mCountTimesteps;
  vector<long long int> values;
};

#endif
