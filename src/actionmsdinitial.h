#ifndef ACTIONMSDINITIAL_H
#define ACTIONMSDINITIAL_H


#include "actionmsd.h"

class ActionMSDInitial : public ActionMSD {
public:
    ActionMSDInitial();
protected:
    void internalAddToTCF(int, const Atom*) override;
    void addGroup(Group *g) override;
    TCF* newTCF() const override;
};


#endif
