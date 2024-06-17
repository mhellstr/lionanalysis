#ifndef ACTIONTMSDCM_H
#define ACTIONTMSDCM_H


#include "actiontmsd.h"
using namespace std;

class ActionTMSDCM : public ActionTMSD {
public:
    ActionTMSDCM();
protected:
    virtual void internalAddToTCF(int, const Atom*) override;
};


#endif
