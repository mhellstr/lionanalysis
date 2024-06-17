#ifndef ACTIONTRESIDENCETIME_H
#define ACTIONTRESIDENCETIME_H


#include "actionttcf.h"

class ActionTResidenceTime : public ActionTTCF<bool, long long int> {
public:
typedef bool INTYPE;
typedef long long int OUTTYPE;
    ActionTResidenceTime();
    ~ActionTResidenceTime();
    //virtual void addGroup(Group *) override;
protected:
    ActionTResidenceTime(ActionType, TCF_t);
    void internalMoreAtomsThanExpected(int numatoms) override;
    void internalAtomLeftEnteredVector(int zerobasedid) override;
    void internalAddToTCF(int, const Atom*) override;
    virtual TTCF<bool, long long int>* newTCF() const override;
    void internalFinalAction(const vector<Atom>* allatoms, const Timestep *t) override;
    virtual float getGnuplotFitOffset() const { return 0; };
    void addValueBoolInfo(int, bool, const int);
};


#endif
