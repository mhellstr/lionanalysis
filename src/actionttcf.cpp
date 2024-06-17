#include "actionttcf.h"
template <typename Tin, typename Tout>
ActionTTCF<Tin,Tout>::ActionTTCF(ActionType type, int ming, int maxg, int mino, int maxo, TCF_t tcftype) 
:ActionTTCFParent(type, ming, maxg, mino, maxo) ,
mTCFType(tcftype)
{
}

template <typename Tin, typename Tout>
ActionTTCF<Tin,Tout>::~ActionTTCF() { 
    //cout << "I am action " << getDescription() << endl;
    for (int i=0; i<mTCF.size(); i++) {
        //cout << "delete " << mTCF[i] << endl;
        delete mTCF[i];
    }
}

template <typename Tin, typename Tout>
void ActionTTCF<Tin,Tout>::correlateAll(int numtimeorigins) {
    if (mManipulateIndividualTCFs) {
        if (numtimeorigins > 0) {
            for (int i=0; i<mpGroup->size(); i++) {
                vector<std::pair<Tout, long long int>> myOutData(mTCF[i]->getTotalOut());
                mTCF[i]->correlate(numtimeorigins);
                bool out_was_updated=false;
                for (int j=0; j<myOutData.size(); j++) {
                    if ( (myOutData[j].first != mTCF[i]->getOutDataValue(j)) || (myOutData[j].second != mTCF[i]->getOutDataCount(j)) ) {
                        out_was_updated=true;
                    }
                    myOutData[j].first-=mTCF[i]->getOutDataValue(j);
                    myOutData[j].second-=mTCF[i]->getOutDataCount(j);
                }
                manipulateIndividualTCF(myOutData, i, out_was_updated);
            }
        }
    }
    else {
        if (numtimeorigins > 0) {
            //cout << "correlateAll mCount " << mCount << getDescription() << endl;
            for (int i=0; i<mpGroup->size(); i++) {
                mTCF[i]->correlate(numtimeorigins);
            }
        }
    }
}

template <typename Tin, typename Tout>
void ActionTTCF<Tin,Tout>::createTCFs(int n) {
    for (int i=0; i<n; i++) {
        mTCF.push_back(newTCF());
        //cout << i << " " << mTCF.back()->capacity() << " before " << 4*mNumItemsPerTimeStep*mMaxTime << endl;
        mTCF.back()->reserve(4*mNumItemsPerTimeStep*mMaxTime); //this is not enough for ResidenceTimeCoordinationShell
        //cout << i << " " << mTCF.back()->capacity() << " after " << endl;
        mTCF.back()->setMaxTime(mMaxTime);
        mTCF.back()->setNumItemsPerTimeStep(mNumItemsPerTimeStep);
    }
}

template <typename Tin, typename Tout>
void ActionTTCF<Tin,Tout>::internalAction(const vector<Atom>* allatoms, const Timestep *t)
{
    int desiredTCFSize=0;
    desiredTCFSize=mpGroup->size();
    //cout << desiredTCFSize << " " << mpGroup->size() << " " << mTCF.size() << endl;
    if (desiredTCFSize > mTCF.size()) {
        //cout << mNumItemsPerTimeStep << endl;
        internalMoreAtomsThanExpected(desiredTCFSize); //for ResidenceTimeCoordinationShell, set mNumItemsPerTimeStep
        //cout << mNumItemsPerTimeStep << endl;
        int oldsize=mTCF.size();
        //cout << "mShellGroup->getId(): " << mpShellGroup->getId() << " " << mpShellGroup->size() << endl;
        createTCFs(desiredTCFSize-oldsize);
        //for (int i=oldsize; i<desiredTCFSize; i++) {
        //    //cout << "desired size = " << desiredTCFSize << " i = " << i << endl;
        //    mTCF.push_back(newTCF()); //these are deleted in ActionTTCF::~ActionTTCF()
        //    mTCF[i]->reserve(4*mNumItemsPerTimeStep*mMaxTime); //this is not enough for ResidenceTimeCoordinationShell
        //    mTCF[i]->setMaxTime(mMaxTime);
        //    mTCF[i]->setNumItemsPerTimeStep(mNumItemsPerTimeStep);
        //    //cout << "maxtime " << mMaxTime << " numitemspertimestep " << mNumItemsPerTimeStep << endl;
        //}
        //mEntered.resize(desiredTCFSize, 0);
    }

    //if (mContinuousMembers) {
        //updateEnteredVector(t);
    //}
    //#pragma omp parallel for 
    for (int i=0; i<mpGroup->size(); i++) { //mpGroup is ALL atoms for type ResidenceTime
        const Atom *a=mpGroup->atom(i);

        vector<bool> vAddedBoolInfo(16, false);
        for (int j=0; j<mvTCFConditions.size(); j++) {
            int bii=mvTCFConditions[j].bii;
            if (vAddedBoolInfo[bii]) continue;
            //switch (bii) {
            switch (mvTCFConditions[j].type) {
                case TCFConditionType::OldMustBeMemberFor1:
                case TCFConditionType::NewMustBeMemberFor1:
                case TCFConditionType::OldMustBeMemberFor2: //not supposed to be used?
                case TCFConditionType::NewMustBeMemberFor2: //not supposed to be used?
                //case BoolInfoIndex::IsMember1: //central atom
                //case BoolInfoIndex::IsMember2: //central atom "product" (only ssp)
                    //bool b=mvTCFConditions[j].g1->isMember(a);
                    //cout << a->id << " " << mvTCFConditions[j].g1->isMember(a) << " " << bii << " " << mNumItemsPerTimeStep << endl;
                    mTCF[i]->addBoolInfo(mvTCFConditions[j].g1->isMember(a), bii, mNumItemsPerTimeStep);
                    //for (int k=0; k<mNumItemsPerTimeStep; k++) {
                    //    mTCF[i]->addBoolInfo(mvTCFConditions[j].g1->isMember(a), bii);
                    //}
                    break;
                case TCFConditionType::OldMustBeMemberFor3:
                case TCFConditionType::NewMustBeMemberFor3:
                //case TCFConditionType::OldMustBeMemberFor4: //not suppsoed to be used?
                //case TCFConditionType::NewMustBeMemberFor4: //not suppsoed to be used?
                //case BoolInfoIndex::IsMember3: //coordination shell
                //case BoolInfoIndex::IsMember4: //coordination shell "product" (only ssp)
                    for (int k=0; k<mpShellGroup->size(); k++) {
                        const Atom* shellatom=mpShellGroup->atom(k);
                        mTCF[i]->addBoolInfo(mvTCFConditions[j].g1->isMember(shellatom), bii);
                    }
                    break;
                case TCFConditionType::OldCoordinationFor1:
                case TCFConditionType::NewCoordinationFor1:
                    for (int k=0; k<mpShellGroup->size(); k++) {
                        const Atom* shellatom=mpShellGroup->atom(k);
                        int ing1id = mvTCFConditions[j].g1->getMemberId(a);
                        if (ing1id >= 0) {
                            mTCF[i]->addBoolInfo(dynamic_cast<GroupCoordination*>(mvTCFConditions[j].g1)->getCoordinationTable(ing1id)->isMember(shellatom), bii);
                        }
                        else {
                            mTCF[i]->addBoolInfo(false, bii);
                        }
                    }
                    break;
            }
            vAddedBoolInfo[bii]=true;
        }

        if (mSimpleAddToTCF) {
            internalAddToTCF(i, a, t);
        }
    } 
    if (!mSimpleAddToTCF) internalComplicatedAddToTCF(t); //only happens for MSDFollow
    if (mCount % mCorrelationFrequency == 0) {
        //if (mCount < 30) cout << "mCount " << mCount << " mCorrelationFrequency " << mCorrelationFrequency << " CORRELATING " << endl;
        correlateAll(mCorrelationFrequency);
    }
}

template <typename Tin, typename Tout>
void ActionTTCF<Tin,Tout>::internalPrint(const vector<Atom>* allatoms, const Timestep *t) {
    mpOut->close();
    mpOut->open(mvOutFilenames.front());
    if (mDoPrintHeader) {
        (*mpOut) << "### DESCR " << getDescription() << endl;
        (*mpOut) << "### count " << mCount << " timeunit " << mTimeUnit << endl;
        (*mpOut) << "#t(ps) value";
        if (mDoPrintRawNumbers) (*mpOut) << " sum count";
        if (mDoPrintAllMembers) {
            for (int i=0; i<mTCF.size(); i++) {
                (*mpOut) << " mem" << i << "id" << mpGroup->atom(i)->id;
            }
        }
        (*mpOut) << "\n";
    }
    std::vector< std::pair<Tout, long long int> > myout(mMaxTime, std::pair<Tout, long long int>(0, 0));
    float internal_normalization=1; //you want to "postnormalize" the VACF so that it is 1 for dt==0
    int cutoff_end=2;
    double shiftby=0;
    double scaletimeby=mTimeUnit;
    if (mType == ActionType::MSDFollow && getOption(ActionTTCFOptions::Discretize)) {
        cutoff_end=0;
        //shiftby=mTimeUnit*mCorrelationFrequency;
        //shiftby=0;
        shiftby=(mCorrelationFrequency-0)*mTimeUnit;
        scaletimeby=mTimeUnit;
    }
    for (int dt=0; dt<mMaxTime-cutoff_end; dt++) {
        for (int i=0; i<mTCF.size(); i++) {
            myout[dt].first+=mTCF[i]->getOutDataValue(dt);
            myout[dt].second+=mTCF[i]->getOutDataCount(dt);
            //cout << dt << " " << mTCF[i]->getOutDataValue(dt) << " " << mTCF[i]->getOutDataCount(dt) << " " << i << " " << mpGroup->atom(i)->id << endl;
        }
        if (getOption(ActionTTCFOptions::Discretize) && myout[dt].second==0 && dt!=0) continue;
        double val=0;
        //if (0==dt && (mTCFType==TCF_t::VACF || mTCFType==TCF_t::ResidenceTime)) internal_normalization= myout[dt].first / myout[dt].second;
        if (0==dt && mDoNormalize0) internal_normalization= myout[dt].first / myout[dt].second;
        if (myout[dt].second > 0) val = myout[dt].first *1.0/ myout[dt].second;
        val /= internal_normalization;
        //cout << "mTimeUnit is " << mTimeUnit << endl;
        //(*o) << dt << " " << val << "\n";

        if (dt==0 && mType==ActionType::MSDFollow && !mDoPrintAllMembers && getOption(ActionTTCFOptions::Discretize)) (*mpOut) << 0 << " " << 0 << "\n";
        (*mpOut) << dt*scaletimeby+shiftby << " " << val;
        if (mDoPrintRawNumbers) {
            (*mpOut) << " " << myout[dt].first << " " << myout[dt].second;
        }
        if (mDoPrintAllMembers) {
            for (int i=0; i<mTCF.size(); i++) {
                long long int c=mTCF[i]->getOutDataCount(dt);
                if (c==0) (*mpOut) << " " << 0;
                else (*mpOut) << " " << mTCF[i]->getOutDataValue(dt) * 1.0 / c;
            }
        }
        (*mpOut) << "\n";
    }
    //}
    //else {
    //    if (mDoPrintHeader) {
    //        (*mpOut) << "#t(ps) ";
    //        for (int i=0; i<mTCF.size(); i++) {
    //            (*mpOut) << "mem" << i << " ";
    //        }
    //        (*mpOut) << "\n";
    //    }
    //    for (int dt=0; dt<mMaxTime; dt++) {
    //        float t=dt;
    //        if (mTimeUnit != 0 && mTCFType!=TCF_t::VACF) t=dt*mTimeUnit;
    //        (*mpOut) << t;
    //        for (int i=0; i<mTCF.size(); i++) {
    //            int c=mTCF[i]->getOutDataCount(dt);
    //            if (c==0) (*mpOut) << " " << 0;
    //            else (*mpOut) << " " << mTCF[i]->getOutDataValue(dt) *1.0/ mTCF[i]->getOutDataCount(dt);
    //        }
    //        (*mpOut) << "\n";
    //    }
    //}
    mpOut->close();

    writeRestart(t);
}

template <typename Tin, typename Tout>
void ActionTTCF<Tin,Tout>::writeRestart(const Timestep *t)
{
    if (mpWriteRestart != nullptr) {
        mpWriteRestart->close();
        mpWriteRestart->open(mWriteRestartFilename);
        (*mpWriteRestart) << t->number << " " << mTCF.size() << " " << mMaxTime << " " << mNumItemsPerTimeStep << "\n";
        for (int i=0; i<mTCF.size(); i++) {
            for (int j=0; j<mTCF[i]->getMaxTime(); j++) {
                (*mpWriteRestart)  << mTCF[i]->getOutDataValue(j) << " " << mTCF[i]->getOutDataCount(j) << " ";
            }
            (*mpWriteRestart) << "\n";
        }
        mpWriteRestart->close();
    }
}

template <typename Tin, typename Tout>
void ActionTTCF<Tin,Tout>::internalInitialize(const vector<Atom>* allatoms, const Timestep* t) {
    if (mpReadRestart != nullptr) {
        int numtcfs=0;
        int maxtime=0;
        int numitemspertimestep=0;
        int mintimestepnumber=0;
        (*mpReadRestart) >> mintimestepnumber >> numtcfs >> maxtime >> numitemspertimestep;
        if (maxtime != mMaxTime) {
            throw string("internalinitialize in ActionTTCF, read the wrong MaxHistory from restart file");
        }
        mNumItemsPerTimeStep = numitemspertimestep;
        mMinTimestepNumber = mintimestepnumber+1;
        createTCFs(numtcfs);
        for (int i=0; i<mTCF.size(); i++) {
            for (int j=0; j<mMaxTime; j++) {
                Tout x;
                long long int count;
                (*mpReadRestart) >> x >> count;
                mTCF[i]->setOutDataValue(j, x);
                mTCF[i]->setOutDataCount(j, count);
            }
        }
    }
}

template <typename Tin, typename Tout>
void ActionTTCF<Tin,Tout>::internalFinalAction(const vector<Atom>* allatoms, const Timestep *t)
{ //there should be calls to virtual functions instead of "if mTCFType==??", etc., I will do that... soon...
    //cout << "before correlatem count % mCorrelation ftrquency " << mCount << " " << mCorrelationFrequency << endl;
    if (mType != ActionType::MSDFollow) {
        correlateAll(mCount % mCorrelationFrequency);
        if (mUseAllTimeOrigins) {
            for (int i=0; i<mpGroup->size(); i++) {
                for (int j=mMaxTime-2; j>=0; j--) {
                    //cout << "correlating mtcf " << i << " for j " << j << endl;
                    mTCF[i]->correlate(1, j);
                }
            }
        }
    }

    internalPrint(allatoms, t);
}



template class ActionTTCF<bool, long long int>;
template class ActionTTCF<double, double>;

