#include "tcfresidencetime.h"

TCFResidenceTime::TCFResidenceTime() : TCF(TCF_t::ResidenceTime), mOldMustBeMember(false), mAllowRecrossings(false)
{ 
    mNumItemsPerTimeStep = 1; //default
}
TCFResidenceTime::TCFResidenceTime(TCF_t type) : TCF(type), mOldMustBeMember(false), mAllowRecrossings(false) //called from TCFResidenceTimeContinuous
{ 
    mNumItemsPerTimeStep = 1; //default
}

std::pair<double, long long int> TCFResidenceTime::internalCorrelateWithLatest(unsigned int oldindex, unsigned int newindex) const {
    double val=0;
    val+=mInData1[oldindex]*mInData1[newindex];
    //for (int j=0; j<mNumItemsPerTimeStep; j++) {
        //val+=(mInData1[newindex+j]-mInData1[oldindex+j])*(mInData1[newindex+j]-mInData1[oldindex+j]);
    //}
    return std::pair<double, long long int>(val,1);
}

bool TCFResidenceTime::allowedIndices(int oldindex, int newindex) { //called from void TCF::correlateWithLatest(int externalmaxdt) 
    if (mOldMustBeMember) { 
        //cout << "oldindex == " << oldindex << " mInData1[oldindex] == " << mInData1[oldindex] << endl;
        return mInData1[oldindex] == 1;
    }
    return true;
}
