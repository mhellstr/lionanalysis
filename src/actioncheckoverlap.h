#ifndef ACTIONCHECKOVERLAP_H
#define ACTIONCHECKOVERLAP_H
#include "action.h"
#include "group.h"

class ActionCheckOverlap : public Action {
public:
    ActionCheckOverlap();
    void internalAction(const vector<Atom> *allatoms, const Timestep *timestep) override;
    void addGroup(Group *g) override;
    void setPrintWarning(bool a) { mPrintWarning = a; };
protected:
    bool mPrintWarning;
};

#endif
