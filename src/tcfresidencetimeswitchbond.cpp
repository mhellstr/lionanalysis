#include "tcfresidencetimeswitchbond.h"

TCFResidenceTimeSwitchBond::TCFResidenceTimeSwitchBond()
: TCFResidenceTime(TCF_t::ResidenceTimeSwitchBond), mOverwriteInData(false)
{
    //mCorrelateForward=true;
    mOldMustBeMember = true;
    mAllowRecrossings = false;
}

void TCFResidenceTimeSwitchBond::correlateWithLatest(int externalmaxdt) { //overrides TCF::correlateWithLatest()
    //NOTE: this function ALWAYS acts as if mOldMustBeMember is true and mAllowRecrossings is false... the variables are NOT checked!
    tidyInData();
    int maxdt=getMaxDT();
    int lastindex=mInData1.size()-mNumItemsPerTimeStep;
    int oldindex=lastindex-maxdt*mNumItemsPerTimeStep;
    //cout << "in the right place" << endl;
    if (oldindex < 0) return;
    vector<bool> vRememberBroken(mNumItemsPerTimeStep, false); 
    //here I can choose to modify mInData1[newindex] if I think of the bond as "intact", so that it is counted as that specific bond when, at a later step, this index would be the oldindex.
    //in fact I should probably do this
    for (int dt=getMinDT(); dt<=maxdt; dt++) {
        //vector<float> vDelta;
        vector<int> vBroken;
        //vector<int> vFormed;
        vector<int> vOldBonds;
        for (int j=0; j<mNumItemsPerTimeStep; j++) {
            if (mInData1[oldindex+j] == 1) vOldBonds.push_back(j);
        }
        float val=0; int count=0;
        int newindex=oldindex+dt*mNumItemsPerTimeStep;
        int numnewbonds=0;
        for (int j=0; j<mNumItemsPerTimeStep; j++) {
            if (vRememberBroken[j] == true) continue;
            numnewbonds+=mInData1[newindex+j];
            float delta = mInData1[newindex+j]-mInData1[oldindex+j];
            //if (delta > 0) vFormed.push_back(j);
            if (delta < 0) vBroken.push_back(j);
        }
        for (int i=0; i<vOldBonds.size(); i++) {
            if (vRememberBroken[vOldBonds[i]]) {
                val+=0;
                count++;
            }
            else if (numnewbonds > 0 && mInData1[newindex+vOldBonds[i]] == 0) { //definitely broken bond. add 0 to correlation function
                vRememberBroken[vOldBonds[i]]=true;
                val+=0;
                count++;
            }
            else { //the bond is still "intact" since no new bond was formed
                val++;
                count++;
                if (mOverwriteInData) {
                    mInData1[newindex+vOldBonds[i]]=1; //modify the in data to reflect that this is now a ``proper'' bond
                }
            }
        }
        mOutData[dt].first+=val;
        mOutData[dt].second+=count;
    }
}

