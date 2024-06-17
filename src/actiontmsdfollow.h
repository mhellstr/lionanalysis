#ifndef ACTIONTMSDFOLLOW_H
#define ACTIONTMSDFOLLOW_H


#include "actiontmsd.h"
#include "groupexternal.h"
#include <vector>
using namespace std;

class ActionTMSDFollow : public ActionTMSD {
public:
    ActionTMSDFollow();
    void printVectorOut(const vector<double> &, const vector<double> &, int, int,int ,int);
    void setExternalGroup(GroupExternal *g) { mExternalGroup = g; };
    void addPrintIdsFilename(const string &);
    void addPrintPositionsFilename(const string &);
    //void setHungarian(bool a=true) {mHungarian=a;}
protected:
    void internalComplicatedAddToTCF(const Timestep *) override;
    void internalInitialize(const vector<Atom>*, const Timestep*) override;
    vector<float> mOldPositions;
    vector<int> mOldIds;
    float getShortDistance(int atomindex, const Timestep *t, int& switchindex, const vector<int> &vIgnore);
    void addJumpBoolInfo(int zerobasedid, bool value, int n);
    virtual TTCF<INTYPE, OUTTYPE>* newTCF() const override;
    void getOccupiedSwitchIndicesHungarian(vector<int>&, const Timestep *t);
    Group *mExternalGroup;
    ofstream *mPrintPositionsOut;
    ofstream *mPrintIdsOut;
    //bool mHungarian;
};


#endif
