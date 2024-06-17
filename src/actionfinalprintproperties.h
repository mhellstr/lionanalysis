#ifndef ACTIONFINALPRINTPROPERTIES_H
#define ACTIONFINALPRINTPROPERTIES_H

#include "actionprintproperties.h"
#include "group.h"

class ActionFinalPrintProperties : public ActionPrintProperties {
public:
    ActionFinalPrintProperties();
protected:
    virtual void internalAction(const vector<Atom> *, const Timestep *) override;
    virtual void internalFinalAction(const vector<Atom> *, const Timestep *) override;
    virtual void internalPrint(const vector<Atom> *, const Timestep *) override;
private:
};


#endif
