#ifndef CGROUPBOND_H
#define CGROUPBOND_H
#include "groupcoordination.h"
class GroupBond : public GroupCoordination {
public:
    GroupBond(string s="");
    void setFromGroup(Group *g) {mpFromGroup=g;}
    void setToGroup(Group *g) {mpToGroup=g;}
    void setCorrespondingGroup(GroupCoordination *g) {mpCorrespondingGroup=g;}
    void setMinDist(float a) { mBondMinDist=a; }
    void setMaxDist(float a) { mBondMaxDist=a; }
    void setInheritDelta(bool a) {mInheritDelta=a;}
    void setIncludeToGroup(bool a) { mIncludeToGroup=a; }
private:
    virtual void internalUpdate(const vector<Atom> *, const Timestep *);
    Group *mpFromGroup;
    Group *mpToGroup;
    GroupCoordination *mpCorrespondingGroup;
    float mBondMinDist;
    float mBondMaxDist;
    bool mInheritDelta; //copy delta parameter from mpToGroup
    bool mIncludeToGroup;
    //int mMinTargetCoordinationNumber;
    //int mMaxTargetCoordinationNumber;
};
#endif
