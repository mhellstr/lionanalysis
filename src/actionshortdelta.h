#ifndef ACTIONSHORTDELTA_H
#define ACTIONSHORTDELTA_H

#include "action.h"
#include "group.h"
#include "groupexternal.h"
#include "actiondoubleshortdelta.h"
#include <vector>
void findShortDelta(const Atom* const atom1, const Atom* &atom2, const Atom* &atom3, const Group* const first, const Group* const middle, const Group* const end, const Group* const group3mustbe, bool excludegroup1coordinationtable, double &delta, bool &success);

class ActionShortDelta : public Action {
public:
    ActionShortDelta();
    void addGroup(Group *g) override;
    void setResolution(double a) { mResolution = a; }
    void setMaxDelta(double a) {mMaxDelta = a; }
    void setMinDelta(double a) {mMinDelta = a; }
    void setScaleBy(double a) {mScaleBy = a;}
    void setSaveExactDelta(bool a) {mSaveExactDelta = a;}
    void setNewGroup1(GroupExternal *g) {mNewGroup1 = g; }
    void setNewGroup2(GroupExternal *g) {mNewGroup2 = g; }
    void setNewGroup3(GroupExternal *g) {mNewGroup3 = g; }
    void setSetDelta(int a) {mSetDelta = a;}
    void setNewGroup1CoordinationGroup(int a) { mNewGroup1CoordinationGroup = a; }
    void setNewGroup2CoordinationGroup(int a) { mNewGroup2CoordinationGroup = a; }
    void setNewGroup3CoordinationGroup(int a) { mNewGroup3CoordinationGroup = a; }
    void setMargin(int a) {mMargin=a;}
    void setHistoryWinner(int a) {mHistoryWinner=a;}
    void setDesiredWinner(int a) {mDesiredWinner=a;}
    void setHistoryGroup(Group *g) {mpHistoryGroup=g;}
    void setGroup3MustBe(Group *g) {mpGroup3MustBe = g;}
    void setExcludeGroup1CoordinationTable(bool a) {mExcludeGroup1CoordinationTable=a;}
protected:
    void internalAction(const vector<Atom>* allatoms, const Timestep *t) override;
    void internalPrint(const vector<Atom>* allatoms, const Timestep *t) override;
    void internalFinalAction(const vector<Atom>* allatoms, const Timestep *t) override;
    void actualSetDelta(int, double, int, Group*, const Atom*);
    Group *mpMiddle;
    Group *mpEnd;
    double mResolution;
    double mMaxDelta;
    double mMinDelta;
    double mScaleBy;
    vector<long long int> mOut;
    int   mNewGroup1CoordinationGroup;
    int   mNewGroup2CoordinationGroup;
    int   mNewGroup3CoordinationGroup;
    GroupExternal *mNewGroup1; 
    GroupExternal *mNewGroup2;
    GroupExternal *mNewGroup3;
    int mSetDelta;
    Group *mpGroup3MustBe; //"endmustbe"

    Group *mpParentGroup;
    int mMargin;
    int mDesiredWinner;
    int mHistoryWinner;
    vector<vector<bool>> mMemberStart;
    vector<vector<bool>> mMemberEnd;
    Group *mpHistoryGroup;

    bool mSaveExactDelta;
    bool mExcludeGroup1CoordinationTable;
};

#endif
