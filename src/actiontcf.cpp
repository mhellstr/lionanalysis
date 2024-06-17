#include "actiontcf.h"

ActionTCF::ActionTCF(ActionType type, int ming, int maxg, int mino, int maxo, TCF_t tcftype) 
:Action(type, ming, maxg, mino, maxo), mMaxTime(10), mNumItemsPerTimeStep(1), mReserveMultiple(1),
mAverage(false), mContinuousMembers(false), mDimX(true), mDimY(true), mDimZ(true), mTCFType(tcftype), mTidyOld(0)
{

}
ActionTCF::~ActionTCF() { //not virtual - this is the destructor that gets called for all derived types
    //cout << "I am action " << getDescription() << endl;
    for (int i=0; i<mTCF.size(); i++) {
        //cout << "delete " << mTCF[i] << endl;
        delete mTCF[i];
    }
}

void ActionTCF::internalAction(const vector<Atom>* allatoms, const Timestep *t)
{
    int desiredTCFSize=0;
    if (mContinuousMembers) desiredTCFSize=t->numatoms;
    else desiredTCFSize=mpGroup->size();
    if (desiredTCFSize > mTCF.size()) {
        internalMoreAtomsThanExpected(desiredTCFSize); //for ResidenceTimeCoordinationShell, set mNumItemsPerTimeStep
        int oldsize=mTCF.size();
        //cout << "mShellGroup->getId(): " << mpShellGroup->getId() << endl;
        for (int i=oldsize; i<desiredTCFSize; i++) {
            //cout << "desired size = " << desiredTCFSize << " i = " << i << endl;
            mTCF.push_back(newTCF()); //these are deleted in ActionTCF::~ActionTCF()
            mTCF[i]->reserve(mNumItemsPerTimeStep*mMaxTime); //this is not enough for ResidenceTimeCoordinationShell
            mTCF[i]->setMaxTime(mMaxTime);
            mTCF[i]->setNumItemsPerTimeStep(mNumItemsPerTimeStep);
            //cout << "maxtime " << mMaxTime << " numitemspertimestep " << mNumItemsPerTimeStep << endl;
        }
        mEntered.resize(desiredTCFSize, 0);
    }

    if (mContinuousMembers) {
        updateEnteredVector(t);
    }
    push_back_in_data(t);
}

void ActionTCF::internalFinalAction(const vector<Atom>* allatoms, const Timestep *t)
{ //there should be calls to virtual functions instead of "if mTCFType==??", etc., I will do that... soon...
    (*mpOut) << "### DESCR " << getDescription() << endl;
    (*mpOut) << "### count " << mCount << " timeunit " << mTimeUnit << endl;
    if (mAverage) {
        (*mpOut) << "#t(ps) value" << endl;
        std::vector< std::pair<double, long long int> > myout(mMaxTime, std::pair<double, long long int>(0.0, 0));
        float internal_normalization=1; //you want to "postnormalize" the VACF so that it is 1 for dt==0
        for (int dt=0; dt<mMaxTime; dt++) {
            for (int i=0; i<mTCF.size(); i++) {
                myout[dt].first+=mTCF[i]->getOutDataValue(dt);
                myout[dt].second+=mTCF[i]->getOutDataCount(dt);
            }
            float val=0;
            //if (0==dt && (mTCFType==TCF_t::VACF || mTCFType==TCF_t::ResidenceTime)) internal_normalization= myout[dt].first / myout[dt].second;
            if (0==dt && (mTCFType==TCF_t::VACF)) internal_normalization= myout[dt].first / myout[dt].second;
            if (myout[dt].second > 0) val = myout[dt].first / myout[dt].second;
            val /= internal_normalization;
            //cout << "mTimeUnit is " << mTimeUnit << endl;
            //(*o) << dt << " " << val << "\n";
            if (mTimeUnit == 0 || mTCFType == TCF_t::VACF) {
                if (mTimeUnit == 0 || myout[dt].second > 0) {
                    (*mpOut) << dt << " " << val << "\n";
                }
            }
            else (*mpOut) << dt*mTimeUnit << " " << val <<  " " << myout[dt].first << " " << myout[dt].second << "\n"; 
        }
    }
    else {
        (*mpOut) << "#t(ps) ";
        for (int i=0; i<mTCF.size(); i++) {
            (*mpOut) << "mem" << i << " ";
        }
        (*mpOut) << "\n";
        for (int dt=0; dt<mMaxTime; dt++) {
            float t=dt;
            if (mTimeUnit != 0 && mTCFType!=TCF_t::VACF) t=dt*mTimeUnit;
            (*mpOut) << t;
            for (int i=0; i<mTCF.size(); i++) {
                int c=mTCF[i]->getOutDataCount(dt);
                if (c==0) (*mpOut) << " " << 0;
                else (*mpOut) << " " << mTCF[i]->getOutDataValue(dt) / mTCF[i]->getOutDataCount(dt);
            }
            (*mpOut) << "\n";
        }
    }

}



void ActionTCF::update_with_continuous_members(const Timestep *timestep) {

}

void ActionTCF::push_back_in_data(const Timestep *timestep) {
    int zerobasedid;
    const Atom *a;
    #pragma omp parallel for private(zerobasedid,a)
    for (int i=0; i<mpGroup->size(); i++) { //mpGroup is ALL atoms for type ResidenceTime
        a=mpGroup->atom(i);
        int zerobasedid = a->id -1;
        if (!mContinuousMembers) zerobasedid=i; //if not continuous members just use the group's atom id
        //cout << "internalAddToTCF(" << zerobasedid << "," << a << ")" << endl;
        internalAddToTCF(zerobasedid, a);
        //it might be better to call the correlateWithLatest() in internalFinalAction, that would most likely use the cache better
        if (mContinuousMembers) mTCF[zerobasedid]->correlateWithLatest();
        else mTCF[zerobasedid]->correlateWithLatest();
    } 
}


void ActionTCF::updateEnteredVector(const Timestep *timestep) {
    for (int i=1; i<=timestep->numatoms; i++) { //all atoms in the current timestep
        bool foundmatch=false;
        for (int j=0; j<mpGroup->size(); j++) { //all atoms in the group
            if (i==mpGroup->atom(j)->id) {
                if (mEntered[i-1] == 0) {
                    mEntered[i-1]=timestep->iteration; //atom id i entered the group on the CURRENT timestep
                    //cout << "mEntered[" << i-1 << " = " << timestep->iteration << endl;
                    //mEntered uses zero-based counting
                }
                else {
                    //do nothing --- keep the old value of mEntered[id]
                    ;
                }
                foundmatch=true;
                break;
            }
        }
        if (foundmatch==false) {
            mEntered[i-1]=0; //mEntered[id]==0 means that the atom currently is not in the group
            internalAtomLeftEnteredVector(i-1);
            //cout << "no match for atom " << i << endl;
            //mTCF[i-1].clear(); //this could save some memory for MSD and VACF.... but for ResidenceTime it would be stupid to clear this vector since you explicitly need data for then the atom is NOT a amember of the group
        }
    }
//    //now mEntered contains 0 if the atom is not in mGroup, and the last timestep it entered otherwise
}


void ActionTCF::setMaxTime(float a, bool realtime) {
    if (!realtime) {
        mMaxTime = int(a);
    }
    else {
        mMaxTime = int( round(a / mTimeUnit) );
    }
}


void ActionTCF::setRealTime(float realtime) {
    int maxtime = int( realtime / mTimeUnit );
    setMaxTime(maxtime);
}
