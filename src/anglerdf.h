
#ifndef ANGLERDF_H
#define ANGLERDF_H

#define ANGLERDF_NEWGROUPCOORDINATION_BOTH 7

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "timestep.h"
#include "atom.h"
#include "group.h"
#include "utility.h"
#include "groupexternal.h"
#include "angletype.h"

using namespace std;

typedef vector<long long int> IntVector;


enum class AngleRDFGroupType {Real, DummyVector, CoordinationShellCenterOfMass};

void push_into_new_group(GroupExternal *, const Atom*, const Atom*, const Atom* c=nullptr);
class AngleRDF : public CObject {
public:
    AngleRDF();
    void print(const vector<ofstream*> &, const Timestep*); //first 8 elements of vector<ofstream*> are used
    //void printGnu(const vector<ofstream*> &, const string &, const string &); //first 8 elements of vector<ofstream*> are discarded, next 6 are used
    void update(const vector<Atom> *, const Timestep *t);
    void setMinAngle(float a) { mMinAngle = a; }
    void setMaxAngle(float a) { mMaxAngle = a; }
    void setMinDist12(float a) { mGroup1MinDist = a; }
    void setMaxDist12(float a) { mGroup1MaxDist = a; }
    void setMinDist23(float a) { mGroup3MinDist = a; }
    void setMaxDist23(float a) { mGroup3MaxDist = a; }
    void setMinDist13(float a) { mGroup13MinDist = a; }
    void setMaxDist13(float a) { mGroup13MaxDist = a; }
    void setUnique12(bool a) { mUnique12=a;}
    void setAngleResolution(float a) { mAngleResolution = a; }
    void setDistanceResolution(float a) { mDistanceResolution = a; }
    void setNewGroup1(GroupExternal *g) {mNewGroup1 = g; }
    void setNewGroup2(GroupExternal *g) {mNewGroup2 = g; }
    void setNewGroup3(GroupExternal *g) {mNewGroup3 = g; }
    void setMinDist23Frac12(float a) { mMinDist23Frac12= a; }
    void setMinDist13Frac12(float a) { mMinDist13Frac12= a; }
    void setMinDist13Frac23(float a) { mMinDist13Frac23= a; }
    void setMaxDist23Frac12(float a) { mMaxDist23Frac12= a; }
    void setMaxDist13Frac12(float a) { mMaxDist13Frac12= a; }
    void setMaxDist13Frac23(float a) { mMaxDist13Frac23= a; }
    void setCopyGroup1(bool a) {mCopyGroup1 = a;}
    void setCopyGroup2(bool a) {mCopyGroup2 = a;}
    void setCopyGroup3(bool a) {mCopyGroup3 = a;}
    void setGroup1(Group *g) { mGroup1 = g; }
    void setGroup2(Group *g) { mGroup2 = g; }
    void setGroup3(Group *g) { mGroup3 = g; }
    void setMinHits12(int a) { mMinHits12 = a; }
    void setMaxHits12(int a) { mMaxHits12 = a; }
    void setNewGroup1CoordinationGroup(int a) { mNewGroup1CoordinationGroup = a; }
    void setNewGroup2CoordinationGroup(int a) { mNewGroup2CoordinationGroup = a; }
    void setNewGroup3CoordinationGroup(int a) { mNewGroup3CoordinationGroup = a; }
    void setAngleType(AngleType a) { mAngleType = a; }
    void setGroupDummyVector(int, float, float, float);
    void setGroupCoordinationShellCenterOfMass(int);
    void setGroupMustBeInCoordinationShellOf(int, int, bool);
    void setNegateGroupMustBeInCoordinationShellOf(bool a) { mNegateMustBeInCoordinationShell = a; }
    void setSignedAngle(bool a) {mSignedAngle=a;}
private:
    Group *mGroup1;
    Group *mGroup2; //the center group!
    Group *mGroup3;
    GroupExternal *mNewGroup1; //pointer to an EXTERNAL type group, the anglerdf incrdf() function will update the members in mGroup at each timestep
    GroupExternal *mNewGroup2;
    GroupExternal *mNewGroup3;
    void makeGroupDummyVector(GroupExternal *, const vector<float>&, const Timestep*);
    void makeGroupCoordinationShellCenterOfMass(GroupExternal *, const Timestep*);
    void cleanUpPseudoGroup(GroupExternal *);
    float coord(int, float, float, float);
    float mAngleResolution;
    float mDistanceResolution;
    float mMinAngle;
    float mMaxAngle;
    float mGroup1MinDist;
    float mGroup1MaxDist;
    float mGroup3MinDist;
    float mGroup3MaxDist;
    float mGroup13MinDist;
    float mGroup13MaxDist;
    float mMinDist23Frac12;
    float mMinDist13Frac12;
    float mMinDist13Frac23;
    float mMaxDist23Frac12;
    float mMaxDist13Frac12;
    float mMaxDist13Frac23;
    int   mNewGroup1CoordinationGroup;
    int   mNewGroup2CoordinationGroup;
    int   mNewGroup3CoordinationGroup;
    float getMinDist23(float ) const;
    float getMinDist13(float, float ) const;
    float getMaxDist23(float ) const;
    float getMaxDist13(float, float) const;
    //void push_into_new_group(Group *, Atom*);
    int mCount; //number of times update() function has been called

    bool mCopyGroup1;
    bool mCopyGroup2;
    bool mCopyGroup3;

    vector<bool> mGroupMustBeInCoordinationShellOf; 
    bool groupMustBeInCoordinationShellOf(int, int);
    bool mNegateMustBeInCoordinationShell;
    bool checkCoordinationShell(int, int, int, const Atom*);

    bool mUnique12;
    vector<IntVector> mDistances1; //the IntVector is angle rdf for each distance
    vector<IntVector> mDistances3; //the IntVector is angle rdf for each distance
    vector<IntVector> mDistances13; //the IntVector is angle rdf for each distance
    vector<IntVector> mDistanceDistance; //the IntVector is distance rdf (3) for each distance (1)

    AngleType mAngleType;

    //the minimum number of times a SPECIFIC pair in (Group1, Group2) satisfies distance and bond critera for ANY group 3 before the mCoordinationNumber in NewGroup1 is increased
    //example: you want to look at H2O molecules coordinated to Na. But you do not want to increase the coordinatoin number for each hydrogen - instead, you
    //increase it ONLY if BOTH hydrogens satisfy the criteria. So set mMinHits12 to 2.
    int mMinHits12; 
    int mMaxHits12; 
    std::vector< std::vector<int> > mHits12; //mHits[i][j], i is index in Group 2 and j is index in Group 1
    void initializeArrays();
    vector<float> mGroup3DummyVector;
    vector<float> mGroup1DummyVector;
    AngleRDFGroupType mGroup1Type;
    AngleRDFGroupType mGroup3Type;

    bool mSignedAngle;
};

#endif
