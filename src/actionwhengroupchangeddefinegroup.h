#ifndef ACTIONWHENGROUPCHANGEDDEFINEGROUP_H
#define ACTIONWHENGROUPCHANGEDDEFINEGROUP_H
#include "action.h"
#include "histogram.h"
#include "group.h"
#include "actionwhengroupchangedparent.h"
#include "atomdurationdelay.h"
#include <vector>

class ActionWhenGroupChangedDefineGroup : public ActionWhenGroupChangedParent {
public:
    ActionWhenGroupChangedDefineGroup();
    void addNewChange(const Atom* a, int firstentered, int lastentered, int productentered) override;
    void addGroup(Group *) override;
    void setDuration(int a) {mDuration=a;}
    void setDelay(int a) {mDelay=a;}
    void setParentGroup(Group *g) {mParentGroup=g;}
protected:
    void internalAction(const vector<Atom> *allatoms, const Timestep *timestep) override; //called at each timestep from main loop
    int mDuration;
    int mDelay;
    vector<AtomDurationDelay> mvToDo;
    void processSingleToDo(AtomDurationDelay&);
    Group *mParentGroup;
};


#endif
