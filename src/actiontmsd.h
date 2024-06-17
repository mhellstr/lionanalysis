#ifndef ACTIONTMSD_H
#define ACTIONTMSD_H

#include "actionttcf.h"

class ActionTMSD : public ActionTTCF<double, double> {
public:
    ActionTMSD();
    ~ActionTMSD();
protected:
    typedef double INTYPE;
    typedef double OUTTYPE;
    ActionTMSD(ActionType, TCF_t);
    void internalMoreAtomsThanExpected(int numatoms) override;
    void internalAtomLeftEnteredVector(int zerobasedid) override;
    virtual void internalAddToTCF(int, const Atom*) override;
    virtual TTCF<INTYPE, OUTTYPE>* newTCF() const override;
    void internalFinalAction(const vector<Atom>* allatoms, const Timestep *t) override;
    virtual float getGnuplotFitOffset() const { return 0; };
};


#endif
