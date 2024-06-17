#ifndef TTCFMSDFOLLOW_H
#define TTCFMSDFOLLOW_H
#include "ttcf.h"
#include "actionttcf.h"

class TTCFMSDFollow : public TTCF<double, double> {
public:
    TTCFMSDFollow(const ActionTTCFParent* a) : TTCF<double, double>(a) { mJoinedCorrelation=true; mNumItemsPerTimeStep=3; };
    virtual ~TTCFMSDFollow() { };
    void correlate(int numtimeorigins, int externalmaxdt = -1) override;
    void clear() override;
protected:
    //TTCFMSDFollow(TCF_t t) : TTCF<bool, long long int>(t) { };
    virtual std::pair<double, long long int> internalCorrelateWithLatest(unsigned int, unsigned int) const override;
    void discretizeLastInterval(int);
    vector<double> mDiscretizedInData;
};

#endif
