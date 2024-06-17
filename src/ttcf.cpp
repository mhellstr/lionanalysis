#include "ttcf.h"
template<typename Tin, typename Tout>
//TTCF<Tin,Tout>::TTCF(TCF_t pType) 
TTCF<Tin,Tout>::TTCF(const ActionTTCFParent* a) 
:mUncareWhenZero(false), mZeroAllWhenZero(false), mUncareWhenUncare(false), mParentAction(a), mJoinedCorrelation(false)
{
    //mFuncAllowedIndices = [](float a, float b) {return true;};
    mBoolInfo.resize(8, nullptr);
    for (int i=0; i<mBoolInfo.size(); i++) {
        mBoolInfo[i] = new vector<bool>;
    }
}

template<typename Tin, typename Tout>
TTCF<Tin,Tout>::~TTCF() { 
    for (auto &x : mBoolInfo) {
        delete x;
    }
    for (auto &x : mIntInfo) {
        delete x;
    }
    for (auto &x : mDoubleInfo) {
        delete x;
    }
}

template<typename Tin, typename Tout>
void TTCF<Tin,Tout>::clear() {
    mInData1.clear();
    mInData2.clear();
    for (int i=0; i<mBoolInfo.size(); i++) {
        mBoolInfo[i]->clear();
    }
}

template<typename Tin, typename Tout>
int TTCF<Tin,Tout>::getMaxDT(int oldindex) const {
    //cout << "inside getmaxdt mpInData1 is " << mpInData1 << endl;
    int numstoredsteps = (mInData1.size()-(oldindex)) / mNumItemsPerTimeStep;
    // 0, 1, 2, 3, 4, 5 ==> numstoredsteps = 2. maximum dt should be 1
    // 0, 1, 2, 3, 4, 5, ... , 29 ==> numstoredsteps = 10. mMaxTime = 10, so maximum dt should be 9
    if (numstoredsteps < mMaxTime - 1) { return numstoredsteps - 1; }
    else  { return mMaxTime -1 ; }
}

template<typename Tin, typename Tout>
int TTCF<Tin,Tout>::getMinDT() const {
    return 0;
}

template<typename Tin, typename Tout>
void TTCF<Tin,Tout>::add(const Tin& a) {
    mInData1.push_back(a);
    //cout << "adding " << a << endl;
}
template<typename Tin, typename Tout>
void TTCF<Tin,Tout>::add2(const Tin& a) {
    mInData2.push_back(a);
    //cout << "adding " << a << endl;
}

template<typename Tin, typename Tout>
void TTCF<Tin,Tout>::add(const Tin& a, const Tin& b) {
    mInData1.push_back(a);
    mInData2.push_back(b);
    //cout << "adding " << a << endl;
}

template<typename Tin, typename Tout>
void TTCF<Tin,Tout>::tidyInData() {
    //called from TCFResidenceTimeSSP::corrleateWithLatest()
    //removes the beginning of the mInData1 and mInData2 vectors that are no longer needed
    //mTidyOld is the multiple of mNumItemsPerTimeStep*mMaxTime that you allow to be stored
    //in the inData vectors before you need to clean them.
    //If you do not to this you will run out of memory with e.g. TCFResidenceTimeSSP or TCFResidenceTimeSwitchBond since these store a LOT of in data for reasonably large systems
    if (getTidyOld() > 0) {
        long long int maxsize=mNumItemsPerTimeStep*getTidyOld()*mMaxTime;
        long long int deletesize=(getMaxDT()+1)*mNumItemsPerTimeStep;
        if (mInData1.size() > maxsize) {
            mInData1.erase(mInData1.begin(),mInData1.end()-deletesize);
        }
        if (mInData2.size() > maxsize) {
            mInData2.erase(mInData2.begin(),mInData2.end()-deletesize);
        }
        for (auto &x : mBoolInfo) {
            if (x->size() > maxsize) x->erase(x->begin(), x->end()-deletesize);
        }
        for (auto &x : mIntInfo) {
            if (x->size() > maxsize) x->erase(x->begin(), x->end()-deletesize);
        }
        for (auto &x : mDoubleInfo) {
            if (x->size() > maxsize) x->erase(x->begin(), x->end()-deletesize);
        }
    }
}




template<typename Tin, typename Tout>
void TTCF<Tin,Tout>::correlate(int numtimeorigins, int externalmaxdt) {
//externalmaxdt default value is -1
    int maxdt=getMaxDT();
    if (externalmaxdt >=0) maxdt=externalmaxdt;
    if (maxdt < mMaxTime-1 && externalmaxdt < 0) return;
    for (int i=numtimeorigins; i>=1; i--) { // you don't want both i and maxdt to go to 0, since then the calculation of oldindex isn't going to work
        //int lastindex=mInData1.size()-mNumItemsPerTimeStep;
        //int oldindex=lastindex-(maxdt+i)*mNumItemsPerTimeStep;
        //int oldindex=mInData1.size() - 1 - (maxdt+i)*mNumItemsPerTimeStep;
        int oldindex=mInData1.size() - (maxdt+i)*mNumItemsPerTimeStep;
        if (oldindex < 0) continue;
        //cout << "i is " << i << " oldindex " << oldindex << " maxdt " << maxdt << " size " << mInData1.size() << endl;
        if (mJoinedCorrelation) {
            correlateFromOldIndex(oldindex);
        }
        else {
            for (int j=0; j<mNumItemsPerTimeStep; j++) {
                //if (oldindex+j >= mInData1.size()) {
                //    cout << "trying to call correlateFromOldIndex " << numtimeorigins << " " << externalmaxdt << " " << oldindex << " " << oldindex+j << " " << mInData1.size() << " " << mNumItemsPerTimeStep << endl;
                //}
                correlateFromOldIndex(oldindex+j);
            }
        }
    }
    tidyInData();
}

template<typename Tin, typename Tout>
void TTCF<Tin,Tout>::correlateFromOldIndex(int oldindex) {
    bool resultisknown=false; //thres ult of the CORRELATION might change this
    Consequence doicare=Consequence::NoConsequence; //the result of the BOOLINFO vectors might change this
    Tout knownresult(0);
    std::vector< std::pair<Tout, long long int> > tempOutData(mOutData.size(), std::pair<Tout, long long int>(0,0)); //modify this vector, then added changes to mOutData at the END after the for-loop, so that you can choose to not modify mOutData if mUncareAllWhenUncareRemaining is set
    //if (oldindex >= mInData1.size()) {
        //throw string("oldindex greater than mIndata1.size()");
    //}
    //if ((*mBoolInfo[0])[oldindex] != mInData1[oldindex]) {
        //cout << "FATAL: " << oldindex << " " << (*mBoolInfo[0])[oldindex] << " " << mInData1[oldindex] << " " << mBoolInfo[0]->size() << " " << mInData1.size() << endl;
    //}
    // if an UncareAll condition is not fulfilled for (oldindex, MAXIMUM newindex), then return
    //int finalnewindex=oldindex + getMaxDT(oldindex)*mNumItemsPerTimeStep;
    //if (!checkTCFCondition(oldindex, finalnewindex, Consequence::UncareAll)) return;

    for (int dt=getMinDT(); dt<=getMaxDT(oldindex); dt++) {
        int newindex=oldindex+dt*mNumItemsPerTimeStep;
        if (newindex >= mInData1.size()) break;
        //cout << "dt = " << dt << " oldindex = " << oldindex << " newindex = " << newindex << " getmaxdt " << getMaxDT(oldindex) << endl;
        //if (resultisknown && valuePrecedenceOverUncare()) {//valuremaining should have precedence over uncareremaining!
        //    tempOutData[dt].first+=knownresult;
        //    tempOutData[dt].second++;
        //}
        //else {
            if (doicare == Consequence::UncareRemaining) break;
            //else if (doicare == Consequence::UncareThis && mUncareWhenUncare) break;
            doicare=doICare(oldindex, newindex); //doICare checks TCFConditions of type UncareRemaining (and then) UncareThis
            if (doicare == Consequence::UncareRemaining) break;
            if (doicare == Consequence::UncareThis) continue;

            if (resultisknown) {
                tempOutData[dt].first+=knownresult;
                tempOutData[dt].second++;
            }
            else {
                Consequence c=Consequence::NoConsequence;
                std::pair<Tout, long long int> p=quickCorrelate(oldindex, newindex, c); //quickCorrelate checks TCFConditions of type ValueRemaining an ValueThis, sets c to the consequence
                if (c == Consequence::NoConsequence) { //c didn't change in quickCorrelate
                    p=internalCorrelateWithLatest(oldindex, newindex); //this is called ONCE per dt (per mNumItemsPerTimeStep)
                }
                else if (c == Consequence::ValueRemaining) {
                    resultisknown=true;
                    knownresult=0;
                }
                tempOutData[dt].first+=p.first;
                tempOutData[dt].second+=p.second;
                if (c == Consequence::ValueRemaining && valuePrecedenceOverUncare()) {
                    for (int dt2=dt+1; dt2<=getMaxDT(oldindex); dt2++) {
                        tempOutData[dt2].first+=knownresult;
                        tempOutData[dt2].second++;
                    }
                    break;
                }
            }
        //}
    }

    if (doicare == Consequence::UncareRemaining && uncareAllWhenUncareRemaining()) {
        return;
    }
    for (int dt=0; dt<mOutData.size(); dt++) {
        mOutData[dt].first+=tempOutData[dt].first;
        mOutData[dt].second+=tempOutData[dt].second;
    }
}

template<typename Tin, typename Tout>
bool TTCF<Tin, Tout>::checkIfMember(int index, int bii) {
    return (*mBoolInfo[static_cast<int>(bii)])[index];
}


template<typename Tin, typename Tout>
bool TTCF<Tin, Tout>::checkIfMemberFor(int index, int bii, int maxhistory, int maxcontinuousescape, int maxtotalescape, bool softstart) {
    //cout << "check if member for " << index << " " << bii << " " << maxhistory << " " << maxcontinuousescape << " " << maxtotalescape << endl;
    if (maxhistory == 0) {
        //cout << index << " " << bii << " " << (*mBoolInfo[bii]).size() << endl;
        return (*mBoolInfo[bii])[index];
    }
    int start=index-maxhistory; //if you're going to the index EXCLUSIVE, which you're not...
    //int start=index-maxhistory+1; //you're going up to index INCLUSIVE, see below
    if (start < 0) {
        if (softstart) start=0;
        else return false;
    }
    int numcontinuouszeroes=0;
    int numtotalzeroes=0;
    //cout << "check if member for between " << start << " and " << index << endl;
    for (int i=start; i<=index; i+=mNumItemsPerTimeStep) { //you're going to the index INCLUSIVE
        //cout << (*mBoolInfo[static_cast<int>(bii)])[i];
        if (! (*mBoolInfo[static_cast<int>(bii)])[i]) {
            numcontinuouszeroes++;
            numtotalzeroes++;
            if (maxcontinuousescape >= 0 && numcontinuouszeroes > maxcontinuousescape) {
                //cout << maxcontinuousescape << " " << numcontinuouszeroes << endl;
                //cout << endl;
                return false;
            }
            if (maxtotalescape >= 0 && numtotalzeroes > maxtotalescape) return false;
        }
        else {
            numcontinuouszeroes=0;
        }
    }
    //cout << endl;
    return true;
}

//template<typename Tin, typename Tout>
//void TTCF<Tin,Tout>::ConditionIsTrue(
template<typename Tin, typename Tout>
bool TTCF<Tin, Tout>::checkTCFCondition(int oldindex, int newindex, Consequence c) {
    //returns false if ANY of the conditions is false, otherwise true
    bool result=true;
    for (auto &x : conditions()) {
        if (x.consequence != c) continue;
        int myhistory=x.p1*mNumItemsPerTimeStep; //scale by mNumItemsPerTimeStep! in calculation of e.g. MSD, IDENTICAL values are writen to the BoolInfo arrays for each of the x, y, z components
        if (myhistory < 0) myhistory=newindex-oldindex; //newindex and oldindex are already separted by multiples of mNumItemsPerTimeStep
        switch (x.type) {
            case TCFConditionType::OldMustBeMemberFor1:
            case TCFConditionType::OldMustBeMemberFor2:
            case TCFConditionType::OldMustBeMemberFor3:
            case TCFConditionType::OldMustBeMemberFor4:
            case TCFConditionType::OldValueIsTrueFor1:
            case TCFConditionType::OldValueIsTrueFor2:
            case TCFConditionType::OldCoordinationFor1:
                result=checkIfMemberFor(oldindex, x.bii, myhistory, x.p2, x.p3, x.softhistory);
                //if (oldindex == newindex && x.bii == 0) cout << "result is " << result << " " << oldindex << " " << x.bii << " " << myhistory << " " << x.p2 << " " << x.p3 << " " << x.negate << endl;
                //if (result) cout << "old " << oldindex << endl;
                break;
            case TCFConditionType::NewMustBeMemberFor1:
            case TCFConditionType::NewMustBeMemberFor2:
            case TCFConditionType::NewMustBeMemberFor3:
            case TCFConditionType::NewMustBeMemberFor4:
            case TCFConditionType::NewJumpMemberFor1:
            case TCFConditionType::NewValueIsTrueFor1:
            case TCFConditionType::NewValueIsTrueFor2:
            case TCFConditionType::NewCoordinationFor1:
                result=checkIfMemberFor(newindex, x.bii, myhistory, x.p2, x.p3, x.softhistory);
                //if (result) cout << "new " << newindex << endl;
                break;
        }
        if (x.negate) {
            //cout << "negating " << int(x.type) << endl;
            result=!result;
        }
        else {
            //cout << "not negating " << int(x.type) << endl;
        }
        if (!result) return result;
    }
    //if (oldindex == newindex) cout << "YUYUYU is " << result << endl;
    return result;
}

template<typename Tin, typename Tout>
Consequence TTCF<Tin,Tout>::doICare(int oldindex, int newindex) {
    bool a;
    //if (oldindex == newindex )cout << "CHECKING FOR UncareRemaining " << conditions().size() << endl;
    a=checkTCFCondition(oldindex, newindex, Consequence::UncareRemaining);
    if (!a) {
        //if (oldindex == newindex) cout << "RETURNING UncareRemaining" << endl;
        return Consequence::UncareRemaining;
    }
    //if (oldindex == newindex) cout << "NOT returning UncareRemaining! for olindex = newindex = " << oldindex << endl;
    a=checkTCFCondition(oldindex, newindex, Consequence::UncareThis);
    if (!a) {
        return Consequence::UncareThis;
    }
    //cout << "RETURNING NoConsequence" << endl;
    return Consequence::NoConsequence;
}

template<typename Tin, typename Tout>
void TTCF<Tin,Tout>::addBoolInfo(bool value, int boolinfoindex) {
    //if (inData1Size() % 100 == 0) {
    //    cout << "mBoolInfo[i].size() = " << mBoolInfo[boolinfoindex]->size() << " " << inData1Size() << endl;
    //}
    mBoolInfo[boolinfoindex]->push_back(value);
}

template<typename Tin, typename Tout>
void TTCF<Tin,Tout>::addBoolInfo(bool value, int boolinfoindex, int n) {
    mBoolInfo[boolinfoindex]->insert(mBoolInfo[boolinfoindex]->end(), n, value);
    //cout << "mBoolInfo.size() = " << mBoolInfo.size() << " " << mBoolInfo[boolinfoindex]->size() << " " << boolinfoindex << " " << n << " " << value << endl;
    //mBoolInfo[boolinfoindex]->push_back(value);
}

template<typename Tin, typename Tout>
std::pair<Tout, long long int> TTCF<Tin,Tout>::quickCorrelate(int oldindex, int newindex, Consequence &c) {
    bool a;
    a=checkTCFCondition(oldindex, newindex, Consequence::ValueRemaining);
    if (!a) {
        c=Consequence::ValueRemaining;
        return std::pair<Tout, long long int>(Tout(0), 1);
    }
    a=checkTCFCondition(oldindex, newindex, Consequence::ValueThis);
    if (!a) {
        c=Consequence::ValueThis;
        return std::pair<Tout, long long int>(Tout(0), 1);
    }
    return std::pair<Tout, long long int>(Tout(0), 0);
}

template class TTCF<bool, long long int>;
template class TTCF<double, double>;
