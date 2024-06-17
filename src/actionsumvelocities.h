#ifndef ACTIONSUMVELOCITIES_H
#define ACTIONSUMVELOCITIES_H

#include "action.h"

class ActionSumVelocities : public Action {
public:
    ActionSumVelocities();
protected:
    void internalAction(const vector<Atom> *, const Timestep *) override;
};


#endif
