#ifndef ACTIONTWODIMSDF_H
#define ACTIONTWODIMSDF_H

#include "anglerdf.h"
#include "group.h"

class ActionTwoDimSDF : public Action {
public:
    ActionTwoDimSDF();
    void setMaxX(double a) {mMaxX=a;}
    void setMaxY(double a) {mMaxY=a;}
    void setXResolution(double a) {mXResolution=a;}
    void setYResolution(double a) {mYResolution=a;}
    void setAngleType(AngleType a) {mAngleType=a;}
    void addGroup(Group *) override;
protected:
    void internalAction(const vector<Atom> *atoms, const Timestep *timestep) override;
    void internalFinalAction(const vector<Atom> *atoms, const Timestep *timestep) override;
    void internalInitialize(const vector<Atom>*, const Timestep*) override;
    void internalPrint(const vector<Atom>*, const Timestep*) override;
    vector<vector<long long int>> mData;
    double mMaxX;
    double mMaxY;
    double mXResolution;
    double mYResolution;
    AngleType mAngleType;
    Group *mpToGroup;
};


#endif
