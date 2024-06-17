#ifndef ACTIONTIMEDENSITY_H
#define ACTIONTIMEDENSITY_H

#include "action.h"
#include <vector>
#include "groupintersection.h"

class ActionTimeDensity : public Action {
public:
    ActionTimeDensity();
    void addGroup(Group *g) override;
protected:
    void internalAction(const vector<Atom>*, const Timestep*) override;
    void internalFinalAction(const vector<Atom>*, const Timestep*) override;
    vector<float> mvNumberDensity;
    vector<float> mvMassDensity;
    float mTimeUnit;
    GroupIntersection mIntersectionGroup;
    Group *mpRegionGroup;
};


#endif
