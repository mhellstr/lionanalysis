#include <stdexcept>
#include <algorithm>
#include "formatter.h"
#include "tcf.h"
#include "utility.h"
#include "groupcoordination.h"

TCF_t stringToTCF_t(const string& s) {
    if (s == "msd") return TCF_t::MSD;
    else if (s == "vac" || s == "vacf") return TCF_t::VACF;
    else if (s == "residencetime") return TCF_t::ResidenceTime;
    else if (s == "residencetimecoordinationshell") return TCF_t::ResidenceTimeCoordinationShell;
    else throw string("Unknown TCF type: " + s);
};

TCF::TCF(TCF_t pType) 
:mType(pType), mCorrelateForward(false), mTidyOld(0)
{
    //mFuncAllowedIndices = [](float a, float b) {return true;};
}

TCF::~TCF() { }

void TCF::setMaxTime(int a) {
    mMaxTime = a;
    mOutData.resize(mMaxTime, std::pair<double, long long int>(0.0,0));
}
void TCF::clear() {
    mInData1.clear();
    mInData2.clear();
}

float TCF::getOutDataValue(int a) const {
    return mOutData[a].first;
}
int TCF::getOutDataCount(int a) const {
    return mOutData[a].second;
}

int TCF::getMaxDT() const {
    //cout << "inside getmaxdt mpInData1 is " << mpInData1 << endl;
    int numstoredsteps = mInData1.size() / mNumItemsPerTimeStep;
    // 0, 1, 2, 3, 4, 5 ==> numstoredsteps = 2. maximum dt should be 1
    // 0, 1, 2, 3, 4, 5, ... , 29 ==> numstoredsteps = 10. mMaxTime = 10, so maximum dt should be 9
    if (numstoredsteps < mMaxTime) { return numstoredsteps - 1; }
    else  { return mMaxTime - 1; }
}

int TCF::getMinDT() const {
    return 0;
}

void TCF::correlateWithLatest(int externalmaxdt) {
    //cout << "before maxdt" << endl;
    if (mCorrelateForward) { //necessary for SSP?
        //if the present timestep is greater than getMaxDT(), then go back getMaxDT() timesteps and correlate *forwards* 0,1,2,...getMaxDT() timesteps
        int maxdt=getMaxDT();
        if (externalmaxdt > 0 && maxdt > externalmaxdt) maxdt=externalmaxdt; //i do not remember what the externalmaxdt parameter is doing.... from what I can tell it is never (!) set when calling correlateWithLatest()....
        int lastindex=mInData1.size()-mNumItemsPerTimeStep;
        int oldindex=lastindex-maxdt*mNumItemsPerTimeStep;
        //cout << "corrleating forward" << endl;
        if (oldindex < 0) return;
        for (int dt=getMinDT(); dt<=maxdt; dt++) {
            int newindex=oldindex+dt*mNumItemsPerTimeStep;
            if (allowedIndices(oldindex,newindex)) {
                std::pair<double, long long int> p=internalCorrelateWithLatest(oldindex, newindex);
                //cout << "adding value " << p.first << " (count " << p.second << " to dt = " << dt << endl;
                mOutData[dt].first+=p.first;
                mOutData[dt].second+=p.second;
            }
        }
    }
    else { //this is my original implementation:
        //at the present (latest timestep), go back 0, 1, 2, .... maxdt timesteps and perform the correlatiaon for each dt
        int maxdt=getMaxDT();
        if (externalmaxdt > 0 && maxdt > externalmaxdt) maxdt=externalmaxdt;
        //cout << "maxdt is " << maxdt << " " << mNumItemsPerTimeStep;
        int newindex=mInData1.size()-mNumItemsPerTimeStep;
        //cout << "correlating backward" << endl;
        for (int dt=getMinDT(); dt<=maxdt; dt++) {
            //cout << " dt = " << dt << endl;
            int oldindex=newindex-dt*mNumItemsPerTimeStep;
            if (allowedIndices(oldindex,newindex)) {
                std::pair<double, long long int> p=internalCorrelateWithLatest(oldindex, newindex);
                //cout << "adding value " << p.first << " (count " << p.second << " to dt = " << dt << endl;
                mOutData[dt].first+=p.first;
                mOutData[dt].second+=p.second;
            }
        }
    }
}

void TCF::add(const float a) {
    mInData1.push_back(a);
    //cout << "adding " << a << endl;
}
void TCF::add2(const float a) {
    mInData2.push_back(a);
    //cout << "adding " << a << endl;
}

void TCF::add(const float a, const float b) {
    mInData1.push_back(a);
    mInData2.push_back(b);
    //cout << "adding " << a << endl;
}

bool TCF::allowedIndices(int oldindex, int newindex) { //this function is overridden with e.g. residencetime, where you can set an option that some oldindices are not allowed 
    return true;
}



void TCF::tidyInData() {
    //called from TCFResidenceTimeSSP::corrleateWithLatest()
    //removes the beginning of the mInData1 and mInData2 vectors that are no longer needed
    //mTidyOld is the multiple of mNumItemsPerTimeStep*mMaxTime that you allow to be stored
    //in the inData vectors before you need to clean them.
    //If you do not to this you will run out of memory with e.g. TCFResidenceTimeSSP or TCFResidenceTimeSwitchBond since these store a LOT of in data for reasonably large systems
    if (mTidyOld > 0) {
        if (mInData1.size() > mNumItemsPerTimeStep*mTidyOld*mMaxTime) {
            //cout << "Erasing minData! from size = " << mInData1.size()  << " (threshold " << mNumItemsPerTimeStep*mTidyOld*mMaxTime << " mNumIt = " << mNumItemsPerTimeStep << " mTidyOld = " << mTidyOld << " mMaxTime = " << mMaxTime << ") to ";
            mInData1.erase(mInData1.begin(),mInData1.end()-(getMaxDT()+1)*mNumItemsPerTimeStep);
            //mInData1.erase(mInData1.begin(), mInData1.begin()+mNumItemsPerTimeStep*mTidyOld*mMaxTime);
            //cout << mInData1.size() << endl;
        }
        if (mInData2.size() > mNumItemsPerTimeStep*mTidyOld*mMaxTime) {
            //mInData2.erase(mInData2.begin(),mInData2.end()-getMaxDT()*mNumItemsPerTimeStep);
            mInData2.erase(mInData2.begin(),mInData2.end()-(getMaxDT()+1)*mNumItemsPerTimeStep);
            //mInData2.erase(mInData2.begin(), mInData2.begin()+mNumItemsPerTimeStep*mTidyOld*mMaxTime);
        }
    }
}
