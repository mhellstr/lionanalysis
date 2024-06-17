#include "ttcfresidencetime.h"

std::pair<long long int, long long int> TTCFResidenceTime::internalCorrelateWithLatest(unsigned int oldindex, unsigned int newindex) const {
    //cout << "correlation returning " << mInData1[oldindex]*mInData1[newindex] << endl;
    return std::pair<long long int, long long int>(mInData1[oldindex]*mInData1[newindex], 1);
}
