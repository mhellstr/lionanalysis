#ifndef ACTIONRDF_H
#define ACTIONRDF_H

#include "action.h"

class ActionRDF : public Action {
public:
    ActionRDF();
protected:
    void internalFinalAction(const vector<Atom> *, const Timestep *) override;
    void internalPrint(const vector<Atom> *, const Timestep *) override;
};


#endif
