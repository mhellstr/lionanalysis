#ifndef ACTIONTRESIDENCETIMESSPCOORDINATIONSHELL_H
#define ACTIONTRESIDENCETIMESSPCOORDINATIONSHELL_H


#include "actiontresidencetimessp.h"
#include "actionwhengroupchanged.h"

class ActionTResidenceTimeSSPCoordinationShell : public ActionTResidenceTimeSSP {
typedef ActionTResidenceTimeSSP::INTYPE INTYPE;
typedef ActionTResidenceTimeSSP::OUTTYPE OUTTYPE;
public:
    ActionTResidenceTimeSSPCoordinationShell();
    ~ActionTResidenceTimeSSPCoordinationShell();
    virtual void addGroup(Group *) override;
    void addActionWhenGroupChanged(Action *a);
protected:
    ActionTResidenceTimeSSPCoordinationShell(ActionType, TCF_t);
    void internalMoreAtomsThanExpected(int numatoms) override;
    void internalAddToTCF(int, const Atom*, const Timestep*) override;
    vector<ActionWhenGroupChangedParent *> mvActionWhenGroupChanged;
    //void internalFinalAction(const vector<Atom>* allatoms, const Timestep *t) override;
};


#endif
