#include "tcfvacf.h"

TCFVACF::TCFVACF() : TCF(TCF_t::VACF)
{ 
    mNumItemsPerTimeStep = 3; //default
}

std::pair<double, long long int> TCFVACF::internalCorrelateWithLatest(unsigned int oldindex, unsigned int newindex) const {
    double val=0;
    for (int j=0; j<mNumItemsPerTimeStep; j++) {
        val+=mInData1[oldindex+j] * mInData1[newindex+j];
    }
    return std::pair<double, long long int>(val,1);
}
