#ifndef TTCFVACF_H
#define TTCFVACF_H
#include "ttcf.h"
#include "actionttcf.h"

class TTCFVACF : public TTCF<double, double> {
public:
    TTCFVACF(const ActionTTCFParent* a) : TTCF<double, double>(a) { mJoinedCorrelation=true; mNumItemsPerTimeStep=3; };
    ~TTCFVACF() { };
protected:
    //TTCFVACF(TCF_t t) : TTCF<bool, long long int>(t) { };
    virtual std::pair<double, long long int> internalCorrelateWithLatest(unsigned int, unsigned int) const override;
};

#endif
