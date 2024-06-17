#ifndef ACTIONISMEMBER_H
#define ACTIONISMEMBER_H
#include "action.h"
#include "group.h"

class ActionIsMember : public Action {
public:
    ActionIsMember();
private:
    void internal_action(const vector<Atom> *, const Timestep *);
};

#endif
