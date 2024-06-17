#include "actiontmsdfollow.h"
#include "ttcf.h"
#include "ttcfmsdfollow.h"
#include "utility.h"
#ifndef NO_DLIB
#include <dlib/optimization/max_cost_assignment.h>
#endif


ActionTMSDFollow::ActionTMSDFollow() 
: ActionTMSD() //, mHungarian(false)
{
    mType = ActionType::MSDFollow;
    mSimpleAddToTCF = false;
    mShiftBy = 1;
    mExternalGroup = nullptr; 
}

void ActionTMSDFollow::addPrintIdsFilename(const string &s) {
    setOption(ActionTTCFOptions::DoPrintIds, true);
    addOut(s);
    mPrintIdsOut = mvpOuts[mvpOuts.size()-1];
}

void ActionTMSDFollow::addPrintPositionsFilename(const string &s) {
    setOption(ActionTTCFOptions::DoPrintPositions, true);
    addOut(s);
    mPrintPositionsOut = mvpOuts[mvpOuts.size()-1];
}

float ActionTMSDFollow::getShortDistance(int atomindex, const Timestep *t, int& switchindex, const vector<int> &vIgnore) {
    int i=atomindex;
    float mindist=10000;
    for (int j=0; j<mOldPositions.size(); j+=3) {
        bool allowed=true;
        for (int k=0; k<vIgnore.size(); k++) {
            if (vIgnore[k] == j/3) {
                allowed=false;
                break;
            }
        }
        if (allowed) {
            float d=distance(mOldPositions[j], mOldPositions[j+1], mOldPositions[j+2], mpGroup->atom(i)->xu, mpGroup->atom(i)->yu, mpGroup->atom(i)->zu, t);
            if (d<mindist) {
                mindist=d;
                switchindex=j/3;
            }
        }
    }
    return mindist;
}

void ActionTMSDFollow::getOccupiedSwitchIndicesHungarian(vector<int>& vOccupiedSwitchIndices, const Timestep *t) {
#ifdef NO_DLIB
    cout << "ERROR! To use TMSDFollow you need to compile with dlib. See Makefile." << std::endl;
#endif
#ifndef NO_DLIB
    //calculate distances between mOldPositions and mpGroup->atom etc.
    //cout << " HUNGARIAN " << endl;
    dlib::matrix<float> dmatrix(mpGroup->size(), mpGroup->size());
    //vOccupiedSwitchIndices should contain the atom indices (ingroup) of the NEW group
    for (int i=0; i<mpGroup->size(); i++) {
        for (int j=0; j<mOldPositions.size(); j+=3) {
            float d = distance(mOldPositions[j], mOldPositions[j+1], mOldPositions[j+2], mpGroup->atom(i)->xu, mpGroup->atom(i)->yu, mpGroup->atom(i)->zu, t);
            int jj=j/3;
            dmatrix(jj, i) = d;
            if (getOption(ActionTTCFOptions::HungarianSquaredPenalty)) {
                dmatrix(jj, i) = d*d;
            }
        }
    }
    float maxdist = dlib::max(dmatrix);
    dlib::matrix<int> submatrix = dlib::matrix_cast<int>( dlib::round((maxdist - dmatrix)*100) );
    std::vector<long> assignment = dlib::max_cost_assignment(submatrix);
    //std::copy(assignment.begin(), assignment.end(), 
    for (int i=0; i<assignment.size(); i++) {
        vOccupiedSwitchIndices[i] = (int)assignment[i];
    }
#endif
}

void ActionTMSDFollow::internalComplicatedAddToTCF(const Timestep *t) {
    //mpGroup is specified as ParentGroup in the configuration input! So that's a little misleading.
    bool found_conflict=false;
    bool found_serious_conflict=false;
    vector<int> vOccupiedSwitchIndices(mpGroup->size(), -1);
    if (mOldPositions.empty()) {
        found_conflict=true;
        found_serious_conflict=true; //this one requires the reset
    }
    else if (mOldPositions.size() == mpGroup->size()*3) {
        if (getOption(ActionTTCFOptions::Hungarian)) {
            getOccupiedSwitchIndicesHungarian(vOccupiedSwitchIndices, t);
        }
        else {
            vector<int> vSwitchIndices(mpGroup->size(), -1);
            vector<float> vMinDists(mpGroup->size(), -1);
            vector<int> vIgnore;
            vIgnore.clear();
            for (int i=0; i<mpGroup->size(); i++) { //i is new index
                float mindist=1000;
                int switchindex=0; //for the i:th element in mpGroup, what is the "position(/3)" that is the corresponding one in mOldPositions?
                mindist=getShortDistance(i, t, switchindex, vIgnore);
                vMinDists[i]=mindist;
                //for (int j=0; j<mOldPositions.size(); j+=3) {
                //    float d=distance(mOldPositions[j], mOldPositions[j+1], mOldPositions[j+2], mpGroup->atom(i)->xu, mpGroup->atom(i)->yu, mpGroup->atom(i)->zu, t);
                //    if (d < mindist) {
                //        mindist=d;
                //        switchindex=j/3;
                //        vMinDists[i]=mindist;
                //    }
                //}
                if (vOccupiedSwitchIndices[switchindex] >= 0) { //already found this one before
                    //cout << "FUNNY! " << endl;
                    //assume that not more than two would go to the same place, so check if the present one or the previous one is closer
                    //mOldPositions contain many coordinates: 1x, 1y, 1z, 2x, 2y, 2z, 3x, 3y, 3z...
                    //vOccupiedSwitchIndices contains 1..n elements that is analogous to mOldPositions, that is,
                    //vOccupiedSwitchIndices[1] is the groupindex (inside mpGroup), that the coordinates 2x, 2y, and 2z refer to
                    //vSwitchIndices is the reverse, i.e.
                    //vSwitchIndices[1] gives the position inside mOldPositions that the second atom in mpGroup "is"
                    //
                    //if vOccupiedSwitchIndices[switchindex] >=0, this means that a previous atom wanted to refer to the same position (switchindex) in mOldPositions
                    //that we want to refer to now
                    //
                    //we need to solve this conflict, so find out what the "second best" options for the previous (previous in the sense it appears in mpGroup at a lower groupindex) atom is
                    //and find out what the second best option for "this" atom i is
                    // if the second best option cannot be used (because it is already taken by a THIRD (unknown) atom) for the previous one, use the second best option for this one
                    // if the second best option cannot be used (because it is already taken by a THIRD (unknown) atom) for this one, use the second best option for the previous one
                    // if both second best options can be used, determine which gives the *smaller* movement
                    int previndex=vOccupiedSwitchIndices[switchindex];
                    float prevdist=vMinDists[previndex];
                    int altprevswitchindex=-1;
                    float altprevdist=getShortDistance(previndex, t, altprevswitchindex, vector<int>(1, vSwitchIndices[previndex]));
                    bool altprevallowed=true;
                    if (vOccupiedSwitchIndices[altprevswitchindex] >=0) {
                        altprevallowed=false;
                    }
                    int altthisswitchindex=-1;
                    float altthisdist=getShortDistance(i, t, altthisswitchindex, vector<int>(1, switchindex));
                    bool altthisallowed=true;
                    if (vOccupiedSwitchIndices[altthisswitchindex] >=0) {
                        altthisallowed=false;
                    }

                    if (!altprevallowed && !altthisallowed) {
                        if (!getOption(ActionTTCFOptions::IgnoreConflicts)) {
                            cerr << "CONFLICT (iteration " << t->iteration << "): I couldn't switch previous " << previndex << " from " << vSwitchIndices[previndex] << " to " << altprevswitchindex;
                            cerr << ", nor could I switch the current " << i << " from " << switchindex << " to " << altthisswitchindex << endl;
                        }
                        found_conflict=true;
                        break;
                    }
                    else if (!altprevallowed && altthisallowed) {
                        //cout << "newallwoed" << endl;
                        vOccupiedSwitchIndices[altthisswitchindex]=i;
                        vSwitchIndices[i]=altthisswitchindex;
                    }
                    else if (altprevallowed && ! altthisallowed) {
                        //cout << "olallowed" << endl;
                        //change the previous one
                        vOccupiedSwitchIndices[altprevswitchindex]=previndex;
                        vSwitchIndices[previndex]=altprevswitchindex;
                        //set the new one
                        vOccupiedSwitchIndices[switchindex]=i;
                        vSwitchIndices[i]=switchindex;
                    }
                    else if (altprevallowed && altthisallowed) {
                        //cout << "bothallowed" << endl;
                        float prevdelta=altprevdist - prevdist; if (prevdelta < 0) prevdelta=-prevdelta;
                        float thisdelta=altthisdist - mindist; if (thisdelta < 0) thisdelta=-thisdelta;
                        if (prevdelta < thisdelta) {
                            //cout << "prev < this: " << prevdelta << " < " << thisdelta << " " << i << " " << previndex << endl;
                            //change the previous one
                            vOccupiedSwitchIndices[altprevswitchindex]=previndex;
                            vSwitchIndices[previndex]=altprevswitchindex;
                            //set the new one
                            vOccupiedSwitchIndices[switchindex]=i;
                            vSwitchIndices[i]=switchindex;
                        }
                        else {
                            //cout << "this < prev " << thisdelta << " < " << prevdelta << " " << i << " " << previndex << endl;
                            vOccupiedSwitchIndices[altthisswitchindex]=i;
                            vSwitchIndices[i]=altthisswitchindex;
                        }
                    }



                    //cerr << "vOccupiedSwitchindices[switchindex] already set to " << vOccupiedSwitchIndices[switchindex] << " whikle attempting to set it to " << i << endl;
                    //found_conflict=true;
                    //break;
                }
                else {
                    //cout << "NOT funny! ";
                    //there was no conflict, so simply store which position in mOldPositions (switchindex) that this atom correpsonds to
                    vOccupiedSwitchIndices[switchindex]=i;
                    vSwitchIndices[i]=switchindex;
                }
                //vOccupiedSwitchIndices[switchindex]=i;
                //vSwitchIndices[i]=switchindex;
            } // for int i=0; i<mpGroup->size(); i++
        } //else mHungarian
    } 
    else { //size changed from one timestep to the next
        if (!getOption(ActionTTCFOptions::IgnoreConflicts)) {
            cerr << "CONFLICT (iteration " << t->iteration << "): size changed from " << mOldPositions.size()/3 << " to " << mpGroup->size() << endl;
        }
        found_conflict=true;
    }
    if (!found_serious_conflict && (!found_conflict || getOption(ActionTTCFOptions::IgnoreConflicts))) {
        for (int i=0; i<mOldPositions.size(); i+=3) {
            int groupindex=vOccupiedSwitchIndices[i/3];
            Atom a;
            //important to set the unwrapped coordinates here, since internalAddtoTCF will use the unwrapped onces
            if (!found_conflict) { //there was no conflict
                a.xu=mpGroup->atom(groupindex)->xu;
                a.yu=mpGroup->atom(groupindex)->yu;
                a.zu=mpGroup->atom(groupindex)->zu;
                TranslateAtomCoordinatesToVicinity(a.xu, a.yu, a.zu, mOldPositions[i], mOldPositions[i+1], mOldPositions[i+2], t);
                //mOldPositions[i+0]=(mpGroup->atom(groupindex)->xu);
                //mOldPositions[i+1]=(mpGroup->atom(groupindex)->yu);
                //mOldPositions[i+2]=(mpGroup->atom(groupindex)->zu);
                mOldPositions[i+0]=a.xu;
                mOldPositions[i+1]=a.yu;
                mOldPositions[i+2]=a.zu;
                //cout << "GROUPINDEX " << groupindex << " " << (mpGroup->atom(groupindex)->id == mOldIds[groupindex]) << endl;
                //addJumpBoolInfo(groupindex, mpGroup->atom(groupindex)->id == mOldIds[groupindex], mNumItemsPerTimeStep);
                addJumpBoolInfo(i/3, mpGroup->atom(groupindex)->id == mOldIds[i/3], mNumItemsPerTimeStep);
                //if (mpGroup->atom(groupindex)->id != mOldIds[i/3]) {
                //    cout << t->iteration << " i/3 " << i/3 << " gi " << groupindex << " o " << mOldIds[i/3] << " n " << mpGroup->atom(groupindex)->id << endl;
                //}
                //mOldIds[groupindex] = mpGroup->atom(groupindex)->id;
                mOldIds[i/3] = mpGroup->atom(groupindex)->id;
                //cout << mpGroup->atom(groupindex)->id << " " << mOldPositions[i] << " " << mOldPositions[i+1] << " " << mOldPositions[i+2] << " ";
                internalAddToTCF(i/3, &a);
                //if (mCount < 30) {
                //    cout << "adding " << a.xu << " " << a.yu << " " << a.zu << " (id = " << mpGroup->atom(groupindex)->id << ") at TCF number " << i/3 << " mCount " << mCount << endl;
                //}
            }
            else { //there was a conflict but we ignore it and just keep the old positions.
                //cout << "there was a conflict but I am ignoring it!" << endl;
                a.xu=mOldPositions[i+0];
                a.yu=mOldPositions[i+1];
                a.zu=mOldPositions[i+2];
                addJumpBoolInfo(i/3, true, mNumItemsPerTimeStep);
                internalAddToTCF(i/3, &a);
            }
        }
        //cout << endl;
    }

    if (found_serious_conflict || (found_conflict && !getOption(ActionTTCFOptions::IgnoreConflicts))) {
        for (int i=0; i<mTCF.size(); i++) {
            mTCF[i]->clear();
        }
        mOldPositions.clear();
        mOldIds.clear();

        for (int i=0; i<mpGroup->size(); i++) {
            mOldPositions.push_back(mpGroup->atom(i)->xu);
            mOldPositions.push_back(mpGroup->atom(i)->yu);
            mOldPositions.push_back(mpGroup->atom(i)->zu);
            mOldIds.push_back(mpGroup->atom(i)->id);

            addJumpBoolInfo(i, true, mNumItemsPerTimeStep);
            internalAddToTCF(i, mpGroup->atom(i));
        }
    }

    if (getOption(ActionTTCFOptions::DoPrintIds) && mPrintIdsOut != nullptr) {
        //(*mvpOuts[1]) << t->iteration;
        for (int i=0; i<mOldIds.size(); i++) {
            (*mPrintIdsOut) << " " << mOldIds[i];
        }
        (*mPrintIdsOut) << "\n";
    }

    if (getOption(ActionTTCFOptions::DoPrintPositions) && mPrintPositionsOut != nullptr) {
        //(*mvpOuts[1]) << t->iteration;
        for (int i=0; i<mOldPositions.size(); i++) {
            (*mPrintPositionsOut) << " " << mOldPositions[i];
        }
        (*mPrintPositionsOut) << "\n";
    }

    if (mExternalGroup != nullptr) {
        mExternalGroup->clear();
        for (int i=0; i<mOldIds.size(); i++) {
            int ingroupid = mpGroup->getMemberId(mOldIds[i]);
            mExternalGroup->addAtom(mpGroup->atom(ingroupid));
        }
        mExternalGroup->update(nullptr, nullptr);
    }

}

void ActionTMSDFollow::addJumpBoolInfo(int zerobasedid, bool value, int n) {
    vector<bool> vAddedBoolInfo(16,false);
    for (int i=0; i<mvTCFConditions.size(); i++) {
        switch (mvTCFConditions[i].type)  {
            case TCFConditionType::NewJumpMemberFor1:
                if (!vAddedBoolInfo[mvTCFConditions[i].bii]) {
                    mTCF[zerobasedid]->addBoolInfo(value, mvTCFConditions[i].bii, n);
                }
                vAddedBoolInfo[mvTCFConditions[i].bii]=true;
                break; //break froms witch
        }
    }
}

TTCF<ActionTMSDFollow::INTYPE, ActionTMSDFollow::OUTTYPE>* ActionTMSDFollow::newTCF() const {
    TTCF<INTYPE, OUTTYPE>* a = new TTCFMSDFollow(this);
    return a;
}


void ActionTMSDFollow::printVectorOut(const vector<double>& toadd, const vector<double>& tonotadd, int counttoadd, int counttonotadd, int oldindex, int newindex) {
    if (getOption(ActionTTCFOptions::DoPrintVectorOut) && mvpOuts.size() >=2) {
        ofstream *o(mvpOuts[1]);
        double sq1=0, sq2=0, scalprod=0;
        double totsq=0;
        for (int i=0; i<toadd.size(); i++) {
            sq1+=toadd[i]*toadd[i];
            sq2+=tonotadd[i]*tonotadd[i];
            totsq+=(toadd[i]+tonotadd[i])*(toadd[i]+tonotadd[i]);
            scalprod+=toadd[i]*tonotadd[i];
        }
        double normal=sqrt(sq1*sq2);
        double ang=500;
        if (normal >0) {
            ang = acos(scalprod/normal)*180/3.14159;
        }
        //the angle is the angle between the discrete and continuous vectors
        //if all segments belong to the same "kind" (either discrete or continuous), the angle 500 is printed
        //the correlation time is maxhistory, and the correlationfrequency (newjumpescape1) equals the segment size. This means there are (maxhistory/segmentsize) segments per line that is printed, i.e. (maxhistory/segmentsize) = counttoadd+counttonotadd
        (*o) << ang << " " << scalprod << " " << normal << " ; " 
         << counttoadd << " " << counttonotadd << " " << double(counttoadd)*1.0/(counttoadd+counttonotadd) << " ; " 
         << toadd[0] << " " << toadd[1] << " " << toadd[2] << " ; " 
         << tonotadd[0] << " " << tonotadd[1] << " " << tonotadd[2] << " ; " 
         << toadd[0]+tonotadd[0] << " " << toadd[1]+tonotadd[1] << " " << toadd[2]+tonotadd[2] << " ; " 
         << sq1 << " " << sq2 << " " << totsq << " " << totsq - 2 * scalprod << " ; "
         << oldindex << " " << newindex << endl;
    }
}


void ActionTMSDFollow::internalInitialize(const vector<Atom>*, const Timestep*) {
    if (getOption(ActionTTCFOptions::Discretize) && mMaxTime % mCorrelationFrequency != 0) throw string("Action TMSDFollow with Discretize option, you MUST have MaxHistory be evenly divisble by the CorrelationFrequency. Description: " + getDescription());
}
