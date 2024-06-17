#ifndef ACTIONDOUBLESHORTDELTA_H
#define ACTIONDOUBLESHORTDELTA_H

#include "actionshortdelta.h"
#include "group.h"
#include "groupexternal.h"
#include <vector>

class ActionDoubleShortDelta : public Action {
public:
    ActionDoubleShortDelta();
    void setStart(Group *g) {mpGroup=g;}
    void setMiddle1(Group *g) {mpMiddle1=g;}
    void setMiddle2(Group *g) {mpMiddle2=g;}
    void setEnd1(Group *g) {mpEnd1=g;}
    void setEnd2(Group *g) {mpEnd2=g;}
    void setMaxDelta(double a)  {mMaxDelta=a;}
    void setResolution(double a)  {mResolution=a;}
protected:
    void internalAction(const vector<Atom>* allatoms, const Timestep *t) override;
    void internalPrint(const vector<Atom>* allatoms, const Timestep *t) override;
    void internalFinalAction(const vector<Atom>* allatoms, const Timestep *t) override;
    Group *mpMiddle1;
    Group *mpMiddle2;
    Group *mpEnd1;
    Group *mpEnd2;
    double mMaxDelta;
    double mResolution;
    vector<vector<double>> mOut;
};

#endif
