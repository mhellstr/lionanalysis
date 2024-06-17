#ifndef ACTIONCHANGEGROUPTIME_H
#define ACTIONCHANGEGROUPTIME_H
#include "action.h"
#include "actionwhengroupchanged.h"

class ActionChangeGroupTime : public Action {
public:
    ActionChangeGroupTime();
    void setMinReactantTime(int a) {mMinReactantTime=a;}
    void setMaxReactionTime(int a) {mMaxReactionTime=a;}
    void setMinProductTime(int a) {mMinProductTime=a;}
    void addGroup(Group *g) override;
    void addActionWhenGroupChanged(Action *a);
    void setShellGroup(Group *g) {mpShellGroup = g;}
    void setCheckCoordinationTable(bool a) { mCheckCoordinationTable=a;}
protected:
    void internalAction(const vector<Atom> *allatoms, const Timestep *timestep) override; //called at each timestep from main loop
    void internalFinalAction(const vector<Atom> *, const Timestep *) override; //called when all timesteps have been read
    void internalActionCheckCoordinationTable(const vector<Atom>* atoms, const Timestep *t);
    Group *mpReactants;
    Group *mpProducts;
    Group *mpShellGroup;
    Group *mpCannotBe;
    bool mCheckCoordinationTable;
    vector<int> mvFirstEntered;
    vector<int> mvContinuousBeforeLeaving;
    vector<int> mvLastEntered;
    vector<int> mvProductEntered;
    vector<bool> mvMemberBefore;
    void internalInitialize(const vector<Atom> *allatoms, const Timestep *timestep) override;
    void printLine(int);
    void printLine(const Atom*, int);
    bool satisfyPrintConstraints(int) const;
    bool mDoPrintFile;
    int mMinReactantTime;
    int mMinProductTime;
    int mMaxReactionTime; // timestep product entered minus timestep left reactants
    vector<ActionWhenGroupChangedParent *> mvActionWhenGroupChanged;
    //virtual void internalMoreAtomsThanExpected(int) override; //used in ActionTCF
};


#endif
