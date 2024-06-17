#ifndef ACTIONFINALSHELL_H
#define ACTIONFINALSHELL_H

#include "action.h"

class ActionFinalShell : public Action {
public:
    ActionFinalShell();
    void setCommand(const string &s) {mCommand=s;}
protected:
    virtual void internalAction(const vector<Atom> *, const Timestep *) override;
    virtual void internalFinalAction(const vector<Atom> *, const Timestep *) override;
    string mCommand;
};



#endif
