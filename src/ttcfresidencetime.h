#ifndef TTCFRESIDENCETIME_H
#define TTCFRESIDENCETIME_H
#include "ttcf.h"
#include "actionttcf.h"

class TTCFResidenceTime : public TTCF<bool, long long int> {
public:
    TTCFResidenceTime(const ActionTTCFParent* a) : TTCF<bool, long long int>(a) { };
    ~TTCFResidenceTime() { };
protected:
    //TTCFResidenceTime(TCF_t t) : TTCF<bool, long long int>(t) { };
    virtual std::pair<long long int, long long int> internalCorrelateWithLatest(unsigned int, unsigned int) const override;
};

#endif
