#ifndef ACTIONDOUBLECOORDINATIONSHORTDELTA_H
#define ACTIONDOUBLECOORDINATIONSHORTDELTA_H

#include "action.h"
#include "group.h"
#include "groupexternal.h"
#include <vector>

void newFindShortDelta(const GroupCoordination* const left, const GroupCoordination* const middle, int inleftid, const Atom* const atom1, const Atom* &atom2, const Atom* &atom3, double &delta, bool &success, const Timestep *, ofstream *);

class ActionDoubleCoordinationShortDelta : public Action {
public:
    ActionDoubleCoordinationShortDelta();
    void addGroup(Group *g) override;
    void setResolution(double a) { mResolution = a; }
    void setMaxDelta(double a) {mMaxDelta = a; }
    void setMinDelta(double a) {mMinDelta = a; }
    void setNewGroupL1(GroupExternal *g) {mNewGroupL1 = g; }
    void setNewGroupL2(GroupExternal *g) {mNewGroupL2 = g; }
    void setNewGroupL3(GroupExternal *g) {mNewGroupL3 = g; }
    void setNewGroupL1CoordinationGroup(int a) { mNewGroupL1CoordinationGroup = a; }
    void setNewGroupL2CoordinationGroup(int a) { mNewGroupL2CoordinationGroup = a; }
    void setNewGroupL3CoordinationGroup(int a) { mNewGroupL3CoordinationGroup = a; }
    void setNewGroupR1(GroupExternal *g) {mNewGroupR1 = g; }
    void setNewGroupR2(GroupExternal *g) {mNewGroupR2 = g; }
    void setNewGroupR3(GroupExternal *g) {mNewGroupR3 = g; }
    void setNewGroupR1CoordinationGroup(int a) { mNewGroupR1CoordinationGroup = a; }
    void setNewGroupR2CoordinationGroup(int a) { mNewGroupR2CoordinationGroup = a; }
    void setNewGroupR3CoordinationGroup(int a) { mNewGroupR3CoordinationGroup = a; }
    void setLGroup3MustBe(Group *g) {mpLGroup3MustBe=g;}
    void setRGroup3MustBe(Group *g) {mpRGroup3MustBe=g;}
    void setLGroup2MustBe(Group *g) {mpLGroup2MustBe=g;}
    void setRGroup2MustBe(Group *g) {mpRGroup2MustBe=g;}
    void setMin13(double a) {mMin13 = a;}
    void setMax13(double a) {mMax13 = a;}
    void setResolution13(double a) {mResolution13 = a; }
    void setWellAtZero(bool a) {mWellAt0=a;}
    void setPrintAllDeltas(const string &);
protected:
    void internalAction(const vector<Atom>* allatoms, const Timestep *t) override;
    void internalPrint(const vector<Atom>* allatoms, const Timestep *t) override;
    void internalFinalAction(const vector<Atom>* allatoms, const Timestep *t) override;
    double mResolution;
    double mMaxDelta;
    double mMinDelta;
    double mMin13, mMax13, mResolution13;
    vector<vector<long long int>> mOutLeft, mOutRight;
    int   mNewGroupL1CoordinationGroup;
    int   mNewGroupL2CoordinationGroup;
    int   mNewGroupL3CoordinationGroup;
    int   mNewGroupR1CoordinationGroup;
    int   mNewGroupR2CoordinationGroup;
    int   mNewGroupR3CoordinationGroup;
    void addResults(double delta, vector<vector<long long int>>& out, const Atom* const atom1, const Atom* const atom2, const Atom* const atom3, GroupExternal* newgroup1, GroupExternal *newgroup2, GroupExternal *newgroup3, int newgroup1coordinationgroup, int newgroup2coordinationgroup, int newgroup3coordinationgroup);
    GroupExternal *mNewGroupL1; 
    GroupExternal *mNewGroupL2;
    GroupExternal *mNewGroupL3;
    GroupExternal *mNewGroupR1; 
    GroupExternal *mNewGroupR2;
    GroupExternal *mNewGroupR3;
    GroupCoordination *mpLHSLeft, *mpLHSMiddle, *mpRHSLeft, *mpRHSMiddle;
    Group *mpLGroup2MustBe;
    Group *mpLGroup3MustBe;
    Group *mpRGroup2MustBe;
    Group *mpRGroup3MustBe;
    bool mWellAt0;
    bool mPrintAllDeltas;
    ofstream *oPrintAllDeltas;
};

#endif
