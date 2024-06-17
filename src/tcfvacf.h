#ifndef TCFVACF_H
#define TCFVACF_H
#include "tcf.h"

class TCFVACF : public TCF {
public:
    TCFVACF();
protected:
    std::pair<double, long long int> internalCorrelateWithLatest(unsigned int, unsigned int) const override;
};

#endif
