#ifndef ACTIONTVACF_H
#define ACTIONTVACF_H

#include "actionttcf.h"

class ActionTVACF : public ActionTTCF<double, double> {
typedef double INTYPE;
typedef double OUTTYPE;
public:
    ActionTVACF();
    virtual ~ActionTVACF();
    void setManipulateIndividualTCFs(bool a, double min, double max, int type) override;
protected:
    ActionTVACF(ActionType, TCF_t);
    void internalMoreAtomsThanExpected(int numatoms) override;
    void internalAtomLeftEnteredVector(int zerobasedid) override;
    void internalAddToTCF(int, const Atom*) override;
    void manipulateIndividualTCF(const vector<pair<OUTTYPE, long long int>>&, int, bool) override;
    virtual TTCF<INTYPE, OUTTYPE>* newTCF() const override;
    void internalFinalAction(const vector<Atom>* allatoms, const Timestep *t) override;
    virtual float getGnuplotFitOffset() const { return 0; };
    double mManipulateMin, mManipulateMax;
    int mManipulateType; //0 means peak, 1 means weighted average
};


#endif
