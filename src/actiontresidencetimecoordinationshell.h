#ifndef ACTIONTRESIDENCETIMECOORDINATIONSHELL_H
#define ACTIONTRESIDENCETIMECOORDINATIONSHELL_H


#include "actiontresidencetime.h"

class ActionTResidenceTimeCoordinationShell : public ActionTResidenceTime {
typedef ActionTResidenceTime::INTYPE INTYPE;
typedef ActionTResidenceTime::OUTTYPE OUTTYPE;
public:
    ActionTResidenceTimeCoordinationShell();
    ~ActionTResidenceTimeCoordinationShell();
    virtual void addGroup(Group *) override;
protected:
    ActionTResidenceTimeCoordinationShell(ActionType, TCF_t);
    void internalMoreAtomsThanExpected(int numatoms) override;
    void internalAddToTCF(int, const Atom*) override;
    //void internalFinalAction(const vector<Atom>* allatoms, const Timestep *t) override;
};


#endif
