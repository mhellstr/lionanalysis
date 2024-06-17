#ifndef ACTIONWHENGROUPCHANGEDPARENT_H
#define ACTIONWHENGROUPCHANGEDPARENT_H
#include "action.h"

class ActionWhenGroupChangedParent : public Action {
public:
    virtual void addNewChange(const Atom* a, int firstentered, int lastentered, int productentered) = 0;
protected:
    ActionWhenGroupChangedParent(ActionType actiontype, int ming, int maxg, int mino, int maxo) :Action(actiontype, ming, maxg, mino, maxo) { };
};

#endif
