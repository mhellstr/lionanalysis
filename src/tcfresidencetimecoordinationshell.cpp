/*
#include "tcfresidencetimecoordinationshell.h"

TCFResidenceTimeCoordinationShell::TCFResidenceTimeCoordinationShell() : TCF(TCF_t::ResidenceTimeCoordinationShell)
{ 
    mNumItemsPerTimeStep = 1; //default
}

std::pair<float,int> TCFResidenceTimeCoordinationShell::internalCorrelateWithLatest(unsigned int oldindex, unsigned int newindex) const {
    float val=0;
    for (int j=0; j<mNumItemsPerTimeStep; j++) {
        val+=mInData1[newindex+j]*mInData1[oldindex+j];
    }
    return std::pair<float,int>(val,mNumItemsPerTimeStep);
}
*/
