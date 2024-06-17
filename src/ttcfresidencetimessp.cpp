#include "ttcfresidencetimessp.h"

std::pair<long long int, long long int> TTCFResidenceTimeSSP::internalCorrelateWithLatest(unsigned int oldindex, unsigned int newindex) const {
    //cout << "correlation returning " << mInData1[oldindex]*mInData1[newindex] << endl;
    long long int val=0;
    //for (int j=0; j<mNumItemsPerTimeStep; j++) {
    //    val += 1 - mInData1[oldindex+j] * mInData2[newindex+j];
    //}
    //return std::pair<long long int, long long int>(val, mNumItemsPerTimeStep);
    
    // mJoinedCorrelation is FALSE, so don't do the above loop, just return a simple correlation between oldindex and newindex
    val = 1 - mInData1[oldindex] * mInData2[newindex];
    return std::pair<long long int, long long int>(val, 1);
}
