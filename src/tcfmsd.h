#ifndef TCFMSD_H
#define TCFMSD_H
#include "tcf.h"

class TCFMSD : public TCF {
public:
    TCFMSD();
protected:
    std::pair<double, long long int> internalCorrelateWithLatest(unsigned int, unsigned int) const override;
};

#endif
