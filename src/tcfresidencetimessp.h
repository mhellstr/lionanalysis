#ifndef TCFRESIDENCETIMESSP_H
#define TCFRESIDENCETIMESSP_H
#include "tcfresidencetime.h"

class TCFResidenceTimeSSP : public TCFResidenceTime {
public:
    TCFResidenceTimeSSP();
    void  correlateWithLatest(int externalmaxdt=-1) override;
    void tidyInData() override;
protected:
    //virtual std::pair<float,int> internalCorrelateWithLatest(unsigned int, unsigned int) const override;
    //bool allowedIndices(int oldindex, int newindex) override;
private:
    //bool perTimestepAllowedIndices(int,int) const;
};


#endif
