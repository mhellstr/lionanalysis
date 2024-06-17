#ifndef ACTIONMOVECENTER_H
#define ACTIOMOVECENTER_H

#include "action.h"
#include "group.h"

class ActionMoveCenter : public Action {
public:
    ActionMoveCenter();
    void setMinVal(double a) {mMinVal=a;}
    void setMaxFrac(double a) {mMaxFrac=a;}
    void setProbabilityToMove(double a) {mProbabilityToMove=a;}
    void setProbabilityToChangeFrame(double a) {mProbabilityToChangeFrame=a;}
protected:
    virtual void internalAction(const vector<Atom> *, const Timestep *) override;
    void internalInitialize(const vector<Atom> *atoms, const Timestep *timestep)  override; 
    double mMinVal, mMaxFrac;
    double mProbabilityToMove, mProbabilityToChangeFrame;
private:
};


#endif


