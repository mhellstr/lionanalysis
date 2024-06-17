#ifndef ACTIONSPHERICALHARMONICS_H
#define ACTIONSPHERICALHARMONICS_H
#include "action.h"
#include "group.h"
#include "groupexternal.h"
#include <vector>

class SphericalHarmonicsRange {
public:
    SphericalHarmonicsRange() :op(0),min(0),max(0) { }
    SphericalHarmonicsRange(int o, double m, double M) :op(o), min(m), max(M) { }
    int op;
    double min;
    double max;
};
class GroupWithVectorOfSphericalHarmonicsRange {
public:
    GroupExternal *group;
    vector<SphericalHarmonicsRange> range;
};

class ActionSphericalHarmonics : public Action {
public:
    ActionSphericalHarmonics();
    void setMaxOrderParameter(int a) {mMaxOrderParameter=a;}
    void addRange(GroupWithVectorOfSphericalHarmonicsRange a) { mvGroupRanges.push_back(a); }
    void setCompareToLibrary(bool a) {mCompareToLibrary=a;}
    void setMaxLibraryError(double a) {mMaxLibraryError=a;}
private:
    void internalAction(const vector<Atom> *, const Timestep *) override;
    int mOutputLines;
    int mMaxOrderParameter;
    bool mCompareToLibrary;
    vector<GroupWithVectorOfSphericalHarmonicsRange> mvGroupRanges;
    void doGroupRanges(const vector<double>&, int);
    void compareToLibrary(const vector<double>&, int);
    double mMaxLibraryError;
    //vector<vector<SphericalHarmonicsRange>> vGroupRanges; //one element for each group in mvpGroup
};

#endif
