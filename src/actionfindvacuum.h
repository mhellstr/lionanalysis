#ifndef ACTIONFINDVACUUM_H
#define ACTIONFINDVACUUM_H

#include "action.h"

class ActionFindVacuum : public Action {
public:
    ActionFindVacuum();
    ~ActionFindVacuum();
    void setResolution(float x) {mResolution = x;}
protected:
    virtual void internalAction(const vector<Atom> *, const Timestep *);
    virtual void updateDescription();
    float mResolution;
    void help_findVacuum(const Timestep*, float, float, float, float, float, float, float, float&, float&, float&, float&);
private:
};


#endif
