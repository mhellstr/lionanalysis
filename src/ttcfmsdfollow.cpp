#include "ttcfmsdfollow.h"
#include "actiontmsdfollow.h"

std::pair<double, long long int> TTCFMSDFollow::internalCorrelateWithLatest(unsigned int oldindex, unsigned int newindex) const {
    //cout << "correlation returning " << mInData1[oldindex]*mInData1[newindex] << endl;
    double val=0;
    //int j=0;
    for (int j=0; j<mNumItemsPerTimeStep; j++) {
        val+=(mInData1[newindex+j]-mInData1[oldindex+j])*(mInData1[newindex+j]-mInData1[oldindex+j]);
    }
    return std::pair<double, long long int>(val,1);
}


void TTCFMSDFollow::discretizeLastInterval(int numelements) {
    //int startindex = mInData1.size() - numelements*mNumItemsPerTimeStep;
    int startindex = mInData1.size() - (numelements+1)*mNumItemsPerTimeStep;
    int endindex = mInData1.size() - 1*mNumItemsPerTimeStep;
    //cout << "startindex IS " << startindex << " enndindex is " << endindex << endl;
    if (startindex < 0) return; //TODO
    for (int i=0; i<mNumItemsPerTimeStep; i++) {
        mDiscretizedInData.push_back(mInData1[endindex+i] - mInData1[startindex+i]);
        //cout << " I just added " << mInData1[endindex+i] << " - " << mInData1[startindex+i] << " = " << mDiscretizedInData.back() << " to mDiscretizedInData at position " << mDiscretizedInData.size()-1 << " (k = " << i << endl;
        //arr[i] = mInData1[endindex+i] - mInData1[startindex+i];
    }
}

void TTCFMSDFollow::correlate(int numtimeorigins, int externalmaxdt) { //this is called from actionttcf::correlateAll and actionttcf::internalFinalAction
    //normally, numtimeorigins would be equal to the correlation frequency
    //however, because you might want to "discretize" the the movements into e.g. 100 fs blocks (to do a continuous/discrete contribution to the MSD, a la G. A. Voth)
    //check whether the "discretize" option is set. IF discretize is set, then the user has set some CorrelationFrequency that is equal to the discretization interval
    //which also means that this function is only called every numtimeorigins steps.
    // the sensible thing to do would be to save the results in some array that is used for the data instead of recomputing all the time
    //  this data is saved in mDiscretizedInData
    if (static_cast<const ActionTMSDFollow*>(mParentAction)->getOption(ActionTTCFOptions::Discretize)) {
        double arr[8];
        discretizeLastInterval(numtimeorigins);
        int reducedMaxTime = mMaxTime / numtimeorigins;
        int startindex = mDiscretizedInData.size() - reducedMaxTime*mNumItemsPerTimeStep;
        //cout << "xstartindex = " << startindex << endl;
        if (startindex < 0) return; //startindex refers to mDiscretizedInData
        int maxdt=getMaxDT();
        //cout << "xmaxdt = " << maxdt << " maxtime = " << mMaxTime  << endl;
        if (maxdt < mMaxTime-1 && externalmaxdt < 0) return;
        int oldindex=mInData1.size() - (maxdt+1)*mNumItemsPerTimeStep;
        //int oldindex=mInData1.size() - numtimeorigins*mNumItemsPerTimeStep;
        //cout << "oldindex " << oldindex << " " << mInData1.size() << " " << numtimeorigins << " " << mNumItemsPerTimeStep << endl;
        if (oldindex < 0) return; //oldindex refers to mInData1
        int bii=0; bool found_bii=false;
        for (auto &x : conditions()) {
            if (x.type == TCFConditionType::NewJumpMemberFor1) {
                bii = x.bii;        
                found_bii=true;
            }
        }
        if (!found_bii) throw("No NewJumpMember for discretized MSDFollow! What are you doing?!");
        vector<double> tempOut(mNumItemsPerTimeStep*reducedMaxTime, 0);
        vector<double> toadd(mNumItemsPerTimeStep, 0);
        vector<double> tonotadd(mNumItemsPerTimeStep, 0);
        int counttoadd=0;
        int counttonotadd=0;
        int newindex;
        for (int dt=0; dt<reducedMaxTime; dt++) {
            //newindex = oldindex + (dt+1)*numtimeorigins*mNumItemsPerTimeStep - 1;
            newindex = oldindex + (dt+1)*numtimeorigins*mNumItemsPerTimeStep;
            //cout << "oldindex = " << oldindex << " dt = " << dt << " calling checkIfMemberFor(" << newindex << " " << bii << " " << numtimeorigins*mNumItemsPerTimeStep << ") mboolsizse = " << mBoolInfo[0]->size() << " " << mNumItemsPerTimeStep << " indatasize " << mInData1.size() << endl;
            bool continuous = checkIfMemberFor(newindex-mNumItemsPerTimeStep, bii, numtimeorigins*mNumItemsPerTimeStep, 0, 0, false); //need to multiply the history parameter by mNumItemsPerTimeStep since IDENTICAL values are added to the BoolInfo array for each of the x, y, z components
            //you want the DISTANCE between oldindex and newindex, but the MEMBERSHIP HISTORY between oldindex and (newindex-1), i.e. the end point should be excluded.
            //for (int k=0; k<(*mBoolInfo[bii]).size(); k+=mNumItemsPerTimeStep) cout << (*mBoolInfo[bii])[k]; cout << endl;
            //cout << " bool continuous = checkIfMemberFor(" << newindex << " " << bii<< " " << numtimeorigins*mNumItemsPerTimeStep << " " <<  endl;
            //cout << "continuous IS " << continuous << endl;
            if (
                (continuous && mParentAction->getOption(ActionTTCFOptions::DiscretizeContinuous))  ||
                (!continuous && mParentAction->getOption(ActionTTCFOptions::DiscretizeDiscrete))
            ){
                //
                for (int k=0; k<mNumItemsPerTimeStep; k++) {
                    //cout << "tempOut[" << dt << "+" << k << "]+=mDiscretizedInData[" << startindex << "+" << mNumItemsPerTimeStep << "*" << dt << "+" << k << "]";
                    tempOut[dt*mNumItemsPerTimeStep+k]+=mDiscretizedInData[startindex+mNumItemsPerTimeStep*dt+k];
                }
                counttoadd++;
                //cout << "YYY " << dt << endl;
            }
            else {
                for (int k=0; k<mNumItemsPerTimeStep; k++) {
                    tonotadd[k]+=mDiscretizedInData[startindex+mNumItemsPerTimeStep*dt+k];
                }
                counttonotadd++;
                //cout << "XXX " << dt << endl;
            }
            for (int k=0; k<mNumItemsPerTimeStep; k++) {
                toadd[k]+=tempOut[dt*mNumItemsPerTimeStep+k];
            }
            // at dt = 0, you get the shift between 0 and numtimeorigins... so you should shift the output data to greater dt (this is done in ActionTTCF:internalPrint())
            for (int k=0; k<mNumItemsPerTimeStep; k++) {
                mOutData[dt*(numtimeorigins-0)].first+=toadd[k]*toadd[k];
                //mOutData[dt*reducedMaxTime].first+=toadd[k]*toadd[k];
                //cout << "mOutData[dt*numtimeorigins].second+=toadd[k]*toadd[k]; dt = " << dt << " numtimeorigins = " << numtimeorigins << " dt*numtimeorigins = " << dt*numtimeorigins << " k = " << k << " " << toadd[k]*toadd[k] << " " << endl;
            }
            mOutData[dt*(numtimeorigins-0)].second++;
            //mOutData[dt*reducedMaxTime].second++;
        }
        const_cast<ActionTMSDFollow*>(static_cast<const ActionTMSDFollow*>(mParentAction))->printVectorOut(toadd, tonotadd, counttoadd, counttonotadd, oldindex, newindex);
    }
    else {
        TTCF::correlate(numtimeorigins, externalmaxdt);
    }
}

void TTCFMSDFollow::clear() {
    TTCF::clear();
    mDiscretizedInData.clear();
}

