#ifndef ACTIONRESIDENCETIME_H
#define ACTIONRESIDENCETIME_H


#include "actiontcf.h"

class ActionResidenceTime : public ActionTCF {
public:
    ActionResidenceTime();
    ~ActionResidenceTime();
    virtual void addGroup(Group *) override;
    void setOldMustBeMember(bool a) { mOldMustBeMember = a; }
    void setAllowRecrossings(bool a) {mAllowRecrossings = a;}
protected:
    ActionResidenceTime(ActionType, TCF_t);
    void internalMoreAtomsThanExpected(int numatoms) override;
    void internalAtomLeftEnteredVector(int zerobasedid) override;
    void internalAddToTCF(int, const Atom*) override;
    virtual TCF* newTCF() const override;
    bool mOldMustBeMember;
    bool mAllowRecrossings;
    void internalFinalAction(const vector<Atom>* allatoms, const Timestep *t) override;
    virtual float getGnuplotFitOffset() const { return 0; };
};


#endif
