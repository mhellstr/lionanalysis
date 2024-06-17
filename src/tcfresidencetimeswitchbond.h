#ifndef TCFRESIDENCETIMESWITCHBOND_H
#define TCFRESIDENCETIMESWITCHBOND_H
#include "tcfresidencetime.h"


class TCFResidenceTimeSwitchBond : public TCFResidenceTime {
public:
    TCFResidenceTimeSwitchBond();
    void  correlateWithLatest(int externalmaxdt=-1) override;
    void setOverwriteInData(bool a) {mOverwriteInData=a;}
protected:
    bool mOverwriteInData;
};
#endif
