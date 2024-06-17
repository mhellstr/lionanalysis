#include "ttcfvacf.h"

std::pair<double, long long int> TTCFVACF::internalCorrelateWithLatest(unsigned int oldindex, unsigned int newindex) const {
    //cout << "correlation returning " << mInData1[oldindex]*mInData1[newindex] << endl;
    double val=0;
    int j=0;
    for (int j=0; j<mNumItemsPerTimeStep; j++) { 
        val+=mInData1[oldindex+j] * mInData1[newindex+j];
    }
    return std::pair<double, long long int>(val,1);
}
