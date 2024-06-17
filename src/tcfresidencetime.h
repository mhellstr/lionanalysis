#ifndef TCFRESIDENCETIME_H
#define TCFRESIDENCETIME_H
#include "tcf.h"

class TCFResidenceTime : public TCF {
public:
    TCFResidenceTime();
    ~TCFResidenceTime() { };
    void setOldMustBeMember(bool a) { mOldMustBeMember=a; }
    void setAllowRecrossings(bool a) {mAllowRecrossings=a; }
protected:
    TCFResidenceTime(TCF_t);
    virtual std::pair<double, long long int> internalCorrelateWithLatest(unsigned int, unsigned int) const override;
    virtual bool allowedIndices(int, int) override;
    bool mOldMustBeMember;
    bool mAllowRecrossings;
};

#endif
