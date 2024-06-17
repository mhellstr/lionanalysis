#ifndef ACTIONVACF_H
#define ACTIONVACF_H


#include "actiontcf.h"

class ActionVACF : public ActionTCF {
public:
    ActionVACF();
protected:
    void internalMoreAtomsThanExpected(int numatoms) override;
    void internalAtomLeftEnteredVector(int zerobasedid) override;
    void internalAddToTCF(int, const Atom*) override;
    TCF* newTCF() const override;
};


#endif
