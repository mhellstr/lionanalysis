#ifndef ACTIONMSD_H
#define ACTIONMSD_H


#include "actiontcf.h"

class ActionMSD : public ActionTCF {
public:
    ActionMSD();
protected:
    void internalMoreAtomsThanExpected(int numatoms) override;
    void internalAtomLeftEnteredVector(int zerobasedid) override;
    void internalAddToTCF(int, const Atom*) override;
    //void push_back_in_data(const Timestep *) override;
    TCF* newTCF() const override;
};


#endif
