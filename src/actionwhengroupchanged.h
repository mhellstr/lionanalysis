#ifndef ACTIONWHENGROUPCHANGED_H
#define ACTIONWHENGROUPCHANGED_H
#include "action.h"
#include "histogram.h"
#include "group.h"
#include "actionwhengroupchangedparent.h"

class ActionWhenGroupChanged : public ActionWhenGroupChangedParent {
public:
    ActionWhenGroupChanged();
    void setBeforeTime(double a, bool realtime=false);
    void setAfterTime(double a, bool realtime=false);
    void setProperty(GROUP_PROPERTY a) {mProp = a;}
    void addNewChange(const Atom* a, int firstentered, int lastentered, int productentered);
    void setResolution(double a) { mResolution = a; }
    void addGroup(Group *g) override;
    void setReactantMustBe(Group *g) { mpReactantMustBe = g; }
    void setMinProductTime(int a) {mMinProductTime=a;}
    void setWriteSuccessfulOut(bool a) {mWriteSuccessfulOut=a;}
protected:
    void internalAction(const vector<Atom> *allatoms, const Timestep *timestep) override; //called at each timestep from main loop
    void internalFinalAction(const vector<Atom> *, const Timestep *) override; //called when all timesteps have been read
    int mBeforeTime;
    int mAfterTime;
    double mResolution;
    vector< vector<double> > mvSavedData;
    vector< vector<bool> > mvIsProduct;
    vector< vector<bool> > mvIsReactant;
    vector<double> mvSum;
    vector< std::pair<int, int> > mvToDo; //<atom index in mpGroup, timestep at which to update mHisto>
    Histogram mHisto;
    void internalInitialize(const vector<Atom> *allatoms, const Timestep *timestep) override;
    void internalPrint(const vector<Atom> *allatoms, const Timestep *timestep) override;
    void checkToDoList(const Timestep *timestep);
    GROUP_PROPERTY mProp;
    Group *mpProductMustBe;
    Group *mpReactantMustBe;
    int mMinProductTime;
    bool mWriteSuccessfulOut; //will write to mvpOuts[1]
};


#endif
