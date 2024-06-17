#ifndef ACTIONDIHEDRALRDF_H
#define ACTIONDIHEDRALRDF_H

#include "action.h"

class ActionDihedralRDF : public Action {
public:
    ActionDihedralRDF();
protected:
    void internalFinalAction(const vector<Atom> *, const Timestep *) override;
    void internalPrint(const vector<Atom> *, const Timestep *) override;
};


#endif
