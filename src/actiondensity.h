#ifndef ACTIONDENSITY_H
#define ACTIONDENSITY_H

#include "action.h"
#include "coordinateaxis.h"
#include <vector>

class ActionDensity : public Action {
public:
    ActionDensity();
    ~ActionDensity();
    void setDirection(CoordinateAxis a) { mDirection = a; }
    void setMaxValue(float a) {mMaxValue = a; mReadRangeFromTimestep=false; }
    void setMinValue(float a) {mMinValue = a; mReadRangeFromTimestep=false; }
    void setResolution(float a) { mResolution = a; }
    void setPeriodic(bool a) {mPeriodic =a;}
    void newAddOut(const string&);
protected:
    void internalAction(const vector<Atom>*, const Timestep*) override;
    void internalFinalAction(const vector<Atom>*, const Timestep*) override;
    void internalPrint(const vector<Atom>*, const Timestep*) override;
    float mMaxValue;
    float mMinValue;
    bool mPeriodic; //if true, ensure that all values are >=0 by adding the Timestep->cell{direction} if the number is negative
    bool mReadRangeFromTimestep;
    float mResolution;
    CoordinateAxis mDirection;
    vector<float> mData;
};


#endif
