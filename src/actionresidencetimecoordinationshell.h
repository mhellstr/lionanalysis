#ifndef ACTIONRESIDENCETIMECOORDINATIONSHELL_H
#define ACTIONRESIDENCETIMECOORDINATIONSHELL_H


#include "actiontcf.h"
#include "actionresidencetimecontinuous.h"

class ActionResidenceTimeCoordinationShell : public ActionResidenceTimeContinuous {
public:
    ActionResidenceTimeCoordinationShell();
    void addGroup(Group *) override;
protected:
    void internalMoreAtomsThanExpected(int numatoms) override;
    void internalAtomLeftEnteredVector(int zerobasedid) override;
    void internalAddToTCF(int, const Atom*) override;
    //TCF* newTCF() const override;
    //GroupCoordination *mpSubGroup;
    Group *mpShellGroup;
};


#endif
