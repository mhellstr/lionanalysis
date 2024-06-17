#ifndef ACTIONTCHARGEFLUX
#define ACTIONTCHARGEFLUX


#include "actiontvacf.h"
using namespace std;

class ActionTChargeFlux : public ActionTVACF {
public:
    ActionTChargeFlux();
protected:
    virtual void internalAddToTCF(int, const Atom*) override;
};


#endif
