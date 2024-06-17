#include "tcfresidencetimessp.h"

TCFResidenceTimeSSP::TCFResidenceTimeSSP()
: TCFResidenceTime(TCF_t::ResidenceTimeSSP)
{
    //mCorrelateForward=true;
}

void TCFResidenceTimeSSP::tidyInData() {
    TCF::tidyInData(); //cleans up mInData1, mInData2
    if (mTidyOld > 0) {
        if (mDoICare.size() > mNumItemsPerTimeStep*mTidyOld*mMaxTime) {
            mDoICare.erase(mDoICare.begin(), mDoICare.end()-(getMaxDT()+1)*mNumItemsPerTimeStep);
        }
    }
}

void TCFResidenceTimeSSP::correlateWithLatest(int externalmaxdt) { //overrides TCF::correlateWithLatest()
    tidyInData();
    int maxdt=getMaxDT();
    if (externalmaxdt >= 0 && externalmaxdt < maxdt) maxdt=externalmaxdt;
    //because the correlation for SSP is done FORWARDS in time, and the time origin is "before" the current timestep
    //you do not want to use the same time origin (0) more than once
    //therefore, return if maxdt < mMaxTime-1
    else if (maxdt < mMaxTime-1) return;
    //cout << "mTidyOld is " << mTidyOld << " maxdt = " << maxdt << endl;
    int lastindex=mInData1.size()-mNumItemsPerTimeStep;
    int oldindex=lastindex-maxdt*mNumItemsPerTimeStep;
    //cout << "in the right place" << endl;
    if (oldindex < 0) return; //this should never happen, since getMaxDT() should know what it's doing
    //if (oldindex < 0) { cout << "olindex < 0" << endl; oldindex=0; }
    // suppose that mInData2[newindex+j] = 1 at dt=5, i.e. product has formed at dt=5 for some coordinated species j
    // then ALL mInData2[newindex+j] should be ``seen'' as 1 for dt=6,7,8,...maxdt
    // but I cannot change the mInData2 vector, so I create this temporary vector vProductToBeSure which indicates for which values of j
    // the product should be ``seen'' to have formed
    // the correlation is done FORWARDS in time, which is necessary for this thing.
    vector<int> vProductToBeSure(mNumItemsPerTimeStep, 0); 
    vector<bool> vDoIActuallyCare(mNumItemsPerTimeStep, true);
    for (int dt=getMinDT(); dt<=maxdt; dt++) {
        double val=0;
        long long int count=0;
        int newindex=oldindex+dt*mNumItemsPerTimeStep;
        //cout << dt << " " << maxdt << " " << oldindex << " " << newindex << endl;
        for (int j=0; j<mNumItemsPerTimeStep; j++) {
            if (mInData1[oldindex+j] == 1) { //"oldmustbemember"
                if (vProductToBeSure[j] == 0 && mInData2[newindex+j] == 1) {
                    vProductToBeSure[j]=1;
                    //cout << "product formed for j = " << j << " ; dt = " << dt << endl;
                }
                if (! mDoICare[oldindex+j]) vDoIActuallyCare[j]=false; //if you're told not to care about this dt (for this j), then do not care at any greater dt either. not caring means not manipulating the value of the correlation function
                if (vDoIActuallyCare[j]) {
                    //val+= 1 - mInData1[oldindex+j] * mInData2[newindex+j]; //this would include recrossings, i.e. R->P->R->P between the first R and the last P, but that is not what you want!
                    val+= 1 - mInData1[oldindex+j] * vProductToBeSure[j]; //mInData1[oldindex+j] == 1 here (see above if-statement, "oldmustbemember")
                    count++;
                }
            }
        }
        mOutData[dt].first+=val;
        mOutData[dt].second+=count;
    }
}

// internalCorrelateWithLatest, allowedIndices, and perTimestepAllowedIndices are not needed since the entire TCF::correlateWithLatest functions is overridden
// in this class
//std::pair<float,int> TCFResidenceTimeSSP::internalCorrelateWithLatest(unsigned int oldindex, unsigned int newindex) const {
//    float val=0;
//    int count=0;
//    for (int j=0; j<mNumItemsPerTimeStep; j++) {
//        if (perTimestepAllowedIndices(oldindex+j, newindex+j)) {
//            val+=1 - mInData1[oldindex+j] * mInData2[newindex+j];
//            count++;
//        }
//    }
//    return std::pair<float,int>(val,count);
//}
//
//bool TCFResidenceTimeSSP::allowedIndices(int oldindex, int newindex) {
//    //this function is called in the main loop in TCF::correlateWithLatest()
//    //but there only the first item per timestep is compared (oldindex, newindex)
//    //and with coordination shells you have several items per timestep
//    //so check if indices are allowed manually within internalCorrelateWithLatest
//    //in the function perTimestepAllowedIndices
//    return true;
//}
//
//bool TCFResidenceTimeSSP::perTimestepAllowedIndices(int oldindex, int newindex) const {
//    if (mOldMustBeMember) { 
//        //cout << "oldindex == " << oldindex << " mInData1[oldindex] == " << mInData1[oldindex] << endl;
//        return mInData1[oldindex] == 1;
//    }
//    return true;
//}
