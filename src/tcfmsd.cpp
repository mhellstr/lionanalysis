#include "tcfmsd.h"

TCFMSD::TCFMSD() : TCF(TCF_t::MSD)
{ 
    mNumItemsPerTimeStep = 3; //default
}

std::pair<double, long long int> TCFMSD::internalCorrelateWithLatest(unsigned int oldindex, unsigned int newindex) const {
    double val=0;
        //cout << "mDoIcare.empty(): " << mDoICare.empty() << endl;
        //cout << "mDoIcare.size(): " << mDoICare.size() << endl;
        //cout << oldindex << endl;
        //if (! mDoICare.empty()) cout << "mDoICare.at(" << oldindex/mNumItemsPerTimeStep << "): " << mDoICare.at(oldindex/mNumItemsPerTimeStep) << endl;
    if (mDoICare.empty() || mDoICare.at(oldindex/mNumItemsPerTimeStep)) { //MSDInitial updates mDoICare, normal MSD and MSDFollow do not
        // there are only mInData.size()/mNumItemsPerTimeStep entries in mDoICare
        for (int j=0; j<mNumItemsPerTimeStep; j++) {
            val+=(mInData1[newindex+j]-mInData1[oldindex+j])*(mInData1[newindex+j]-mInData1[oldindex+j]);
        }
        return std::pair<double, long long int>(val,1);
    }
    else {
        return std::pair<double, long long int>(0., 0);
    }
}
