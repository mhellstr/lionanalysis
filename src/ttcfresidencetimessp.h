#ifndef TTCFRESIDENCETIMESSP_H
#define TTCFRESIDENCETIMESSP_H
#include "ttcfresidencetime.h"
#include "actionttcf.h"

class TTCFResidenceTimeSSP : public TTCFResidenceTime {
public:
    TTCFResidenceTimeSSP(const ActionTTCFParent* a) : TTCFResidenceTime(a) { };
    ~TTCFResidenceTimeSSP() { };
protected:
    //TTCFResidenceTime(TCF_t t) : TTCF<bool, long long int>(t) { };
    virtual std::pair<long long int, long long int> internalCorrelateWithLatest(unsigned int, unsigned int) const override;
};

#endif
