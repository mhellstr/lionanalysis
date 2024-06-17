#ifndef ACTIONRESIDENCETIMESSPCOORDINATIONSHELL_H
#define ACTIONRESIDENCETIMESSPCOORDINATIONSHELL_H

#include "actionresidencetimessp.h"
#include "consequence.h"

//enum class Consequence { Product, Uncare };

class ActionResidenceTimeSSPCoordinationShell : public ActionResidenceTimeSSP {
public:
    ActionResidenceTimeSSPCoordinationShell();
    void addGroup(Group *) override;
    void setCenterMustBeOtherwiseConsequence(Consequence a) {mCenterMustBeOtherwiseConsequence=a;}
    void setShellMustBeOtherwiseConsequence(Consequence a) {mShellMustBeOtherwiseConsequence=a;}
protected:
    void internalMoreAtomsThanExpected(int numatoms) override;
    void internalAddToTCF(int, const Atom*) override;
    Group *mpShellGroup;
    Group *mpCenterMustBe;
    Group *mpShellMustBe;
    Consequence mCenterMustBeOtherwiseConsequence;
    Consequence mShellMustBeOtherwiseConsequence;
};

#endif
