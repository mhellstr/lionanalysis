#ifndef TTCFMSD_H
#define TTCFMSD_H
#include "ttcf.h"
#include "actionttcf.h"

class TTCFMSD : public TTCF<double, double> {
public:
    TTCFMSD(const ActionTTCFParent* a) : TTCF<double, double>(a) { mJoinedCorrelation=true; mNumItemsPerTimeStep=3; };
    ~TTCFMSD() { };
protected:
    //TTCFMSD(TCF_t t) : TTCF<bool, long long int>(t) { };
    virtual std::pair<double, long long int> internalCorrelateWithLatest(unsigned int, unsigned int) const override;
};

#endif
