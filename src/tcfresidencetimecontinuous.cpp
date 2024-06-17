#include "tcfresidencetimecontinuous.h"

/*
 * mInData1 contains 0001111000110111; 1 if ActionResidenceTime::mpSubGroup->isMember() is true, 0 if it is false. This is the "raw data" - the TCFResidenceTimeContinuous object then decides which of these data (oldindex, newindex, dt) should be used to calculate the correlation function stored in mOutData
 * allowedIndices(oldindex,newindex) is called from within TCF::correlateWithLatest(), returns true if call to internalCorrelateWithLatest() should be allowed (i.e. whether to add stuff to mOutData)
 * THE USER CAN SET A NUMBER OF OPTIONS FOR allowedIndices():
 *      * the default is to accept ANY oldindex. this means that mOutData[0] will be equal to the probability finding the atom in ActionResidenceTime::mpSubGroup
 *      * mOldMustBeMember - only if whatever atom this TCF belongs to qualifies as a "member" at time oldindex is (oldindex,newindex) considered. 
 *          * "Simple" membership: mInData1[oldindex] == 1, i.e. just check if the atom was a member at that timestep (this is equivalent to setting mOldMustBeMemberFor to 0)
 *          * "Complicated" membership: Go back mOldMustBeMemberFor timesteps, and check that the atom has been a member for at least (mOldMustBeMemberFor-mOldMaxTotalEscapeTime) timesteps, with no continuous amount of zeroes exceeding mOldMaxContinuousEscapeTime timesteps
 *              * if mOldMustBeMemberFor is greater than oldindex, return false;
 *          * regardless of membership critera, the vector<bool> mvOldIsMember saves whether mInData1[oldindex] qualifies, since it is unnecessary to recheck this all the time since it only depends on historic variables
 * SETTINGS TO DECIDE WHETHER THE VALUES LEADING UP TO mInData1[newindex] SHOULD ADD A 1 OR A 0 TO THE CORRELATION FUNCTION (mOutData), this is used within internalCorrelateWithLatest()
 *     * if mvOldIsMember[oldindex] == false, then return 0; (this should only ever happen for mOldMustBeMember==false, otherwise we won't enter internalCorrelateWithLatest() when mvOldIsMember[oldindex]==false)
 *     * mNewMustBeMember: if mInData1[newindex] != 1, then return 0;
       * "Simple" membership: mInData1[newindex] == 1, i.e. just check if the atom is a member at that timestep (this is the default: mNewMembershipType == Membership::Simple)
       * "Complicated" membership:  if mNewMaxContinuousEscapeTime or mNewMaxTotalEscapeTime is set (changed from default values: mNewMembershipType == Membership::Complicated), then:
 *          * check the entire range oldindex...newindex, and check that the atom has been a member for at least ( (newindex-oldindex)-mNewMaxTotalEscapeTime) timesteps, with no continuous amount of zeroes exceeding mNewMaxContinuousEscapeTime timesteps.
            * (UNIMPLEMENTED : perhaps doesn't make much sense) go back mNewMustBeMemberFor timesteps, and check that the atom has been a member for at least (mOldMustBeMemberFor-mOldMaxTotalEscapeTime) timesteps, with no continuous amount of zeroes exceeding mNewMaxContinuousEscape Time timesteps
 * There is an asymmetry in the variables mNewMustBeMember and mOldMustBeMember: mNewMustBeMember checks that mInData1[newindex] == 1, i.e. that the LAST element is member, while mOldMustBeMember calls isOldAMember(), i.e. checks if oldindex is a member for whatever critera were given.
 */

TCFResidenceTimeContinuous::TCFResidenceTimeContinuous() : 
    TCFResidenceTime(TCF_t::ResidenceTimeContinuous), 
    mOldMaxTotalEscapeTime(1000000), mOldMaxContinuousEscapeTime(1000000), 
    mNewMaxTotalEscapeTime(1000000), mNewMaxContinuousEscapeTime(1000000), 
    mOldMustBeMemberFor(0), 
    mNewMembershipType(Membership::Simple), mNewMustBeMember(false)
{ 
    mNumItemsPerTimeStep = 1; 
}

void TCFResidenceTimeContinuous::correlateWithLatest(int externalmaxdt) { //overrides TCF::correlateWithLatest()
    tidyInData();
    //cout << mMaxTime << " " << mOldMustBeMember << " " << mNewMustBeMember << " " << mOldMaxTotalEscapeTime << " " << mOldMaxContinuousEscapeTime << " " << mNewMaxContinuousEscapeTime << " " << mNewMaxTotalEscapeTime << " " << mOldMustBeMemberFor << endl;
    int maxdt=getMaxDT();
    int lastindex=mInData1.size()-mNumItemsPerTimeStep;
    //int oldindex=lastindex-maxdt*mNumItemsPerTimeStep;
    int oldindex=lastindex-mMaxTime*mNumItemsPerTimeStep;
    //cout << "in the right place" << endl;
    if (oldindex < 0) return;
    //cout << oldindex << " " << mNewMustBeMember << mAllowRecrossings << mOldMustBeMember << endl;
    vector<bool> vBroken(mNumItemsPerTimeStep, false); 
    for (int dt=getMinDT(); dt<=maxdt; dt++) {
        double val=0;
        long long int count=0;
        int newindex=oldindex+dt*mNumItemsPerTimeStep;
        for (int j=0; j<mNumItemsPerTimeStep; j++) {
            bool oldisamember=isOldAMember(oldindex+j, mOldMustBeMemberFor);
            bool newisamember=(mInData1[newindex+j]==1);
            //cout << oldisamember << " " << endl;
            if (! mOldMustBeMember || oldisamember) { 
                //if (mOldMustBeMember && oldisamember && vBroken[j] && ! mAllowRecrossings) { //the logic here was that it mostly makes sense to check for broken bonds if oldmustbemember, which makes good sense
                if (mNewMustBeMember && ! newisamember) {
                    val+=0;
                    count++;
                    //if (oldisamember) {
                        //cout << "doing it for dt = " << dt << endl;
                    //}
                    continue;
                }
                if (vBroken[j] && ! mAllowRecrossings) {
                    val+=0;
                    count++;
                }
                else {
                    std::pair<double, long long int> p=singleInternalCorrelateWithLatest(oldindex+j, newindex+j); //returns <value, count>
            //cout << "mInData1[" << oldindex+i << "] = " << mInData1[oldindex+i];
            //cout << " mInData1[" << newindex+i << "] = " << mInData1[newindex+i];
            //cout << " pushing back: " << p.first << " count: " << p.second << endl;
                    //if (dt==1) {
                    //    cout << oldindex << " " << newindex << " " << p.first << " " << p.second << endl;
                    //}
                    if (p.first == 0) vBroken[j]=true;
                    val+= p.first;
                    count++;
                }
            }
        }
        mOutData[dt].first+=val;
        mOutData[dt].second+=count;
    }
}


/*
std::pair<float,int> TCFResidenceTimeContinuous::internalCorrelateWithLatest(unsigned int oldindex, unsigned int newindex) const {
    std::pair<float,int> res(0.0,0);
    //cout << "HELLO" << endl;
    for (int i=0; i<mNumItemsPerTimeStep; i++) {
        if (perTimestepAllowedIndices(oldindex+i, newindex+i)) {
            //cout << " allowed " << endl;
            std::pair<float,int> p=singleInternalCorrelateWithLatest(oldindex+i, newindex+i);
            res.first+=p.first;
            res.second+=p.second;
        }
    }
    return res;
}
*/
std::pair<double, long long int> TCFResidenceTimeContinuous::singleInternalCorrelateWithLatest(unsigned int oldindex, unsigned int newindex) const {
    //float val=0;
    //val+=mInData1[oldindex]*mInData1[newindex];
    //float sum=0, maxsum=0;
    //cout << " mOldMustBeMember == " << (int)mOldMustBeMember << 
    //        " mNewMustBeMember == " << (int)mNewMustBeMember <<
    //        " mNewMembershipType == " << (int)mNewMembershipType <<
    //        " mOldMaxTotalEscapeTime == " << mOldMaxTotalEscapeTime <<
    //        " mNewMaxTotalEscapeTime == " << mNewMaxTotalEscapeTime <<
    //        " mOldMaxContinuousEscapeTime == " << mOldMaxContinuousEscapeTime <<
    //        " mNewMaxContinuousEscapeTime == " << mNewMaxContinuousEscapeTime <<
    //        endl;
    int returncount=1;
    //if (mOldMustBeMember == true) {
        //if (mvOldIsMember[oldindex] == false) return std::pair<float,int>(0,returncount);
    //}
    //if (mNewMustBeMember == true && mInData1[newindex] == 0) { return std::pair<float,int>(0,returncount); } //this is now checked in correlateWithLatest() instead
    if (mNewMembershipType == Membership::Simple) {
        //cout << newindex << " " << mInData1[newindex] << endl;
        if (mOldMustBeMember == true) {
            //cout << "returning " << mInData1[newindex] <<  " oldindex = " << oldindex << " " << " newindex = " << newindex << endl;
            return std::pair<double, long long int>(mInData1[newindex],returncount);
            //if (mInData1[newindex] == 1) return 1;
            //else return 0;
        }
        else {
            
            //if (mvOldIsMember[oldindex] == true) cout << "true ";
            //else cout << "false ";
            //cout << "mInData1[" << newindex << "*int(mvOldIsMember[" << oldindex << "]) = " << mInData1[newindex] << "*" << int(mvOldIsMember[oldindex]) << " = " << mInData1[newindex]*int(mvOldIsMember[oldindex]) << endl;
            return std::pair<double, long long int>(mInData1[newindex]*int(isOldAMember(oldindex, mOldMustBeMember)),returncount);
        }
    }
    else if (mNewMembershipType == Membership::Complicated) {
        long long int numzeroes=0, numcontinuouszeroes=0;
        //if (mNewMustBeMember) {
            //cout << "newmustbemember!" << oldindex << " " << newindex;
        //}
        for (int i=oldindex; i<=newindex; i+=mNumItemsPerTimeStep) {
                //cout << "i = " << i << " numzeroes = " << numzeroes << " numcontinuouszeroes = " << numcontinuouszeroes << " mNew " << mNewMaxTotalEscapeTime << " " << mNewMaxContinuousEscapeTime << endl;
                //cout << oldindex << " " << i << " " << newindex << " " << mNumItemsPerTimeStep << " " << mInData1[i] << " " << mOldMustBeMember << " " << mNewMustBeMember << endl;
            if (mInData1[i] == 0) {
                numzeroes++;
                numcontinuouszeroes++;
                //cout << "i = " << i << " numzeroes = " << numzeroes << " numcontinuouszeroes = " << numcontinuouszeroes << " mNew " << mNewMaxTotalEscapeTime << " " << mNewMaxContinuousEscapeTime << endl;
                if (numzeroes > mNewMaxTotalEscapeTime) return std::pair<double, long long int>(0,returncount);
                if (numcontinuouszeroes > mNewMaxContinuousEscapeTime) return std::pair<double, long long int>(0,returncount);
            }
            else {
                numcontinuouszeroes=0;
            }
        }
        return std::pair<double, long long int>(1,returncount);
    }
    cout << "THIS SHOULD NEVER HAPPEN: TCFResidenceTimeContinouous::internalCorrelateWithLatest - unknown NewMembershipType" << endl;
    return std::pair<double, long long int>(0,0);
}

bool TCFResidenceTimeContinuous::perTimestepAllowedIndices(int oldindex, int newindex) const {
    if (mOldMustBeMember) { 
        //cout << " oldindex " << oldindex << " has " << isOldAMember(oldindex, mOldMustBeMember) << " been a member for at lteast " << mOldMustBeMemberFor << " timesteps " << endl;
        return isOldAMember(oldindex, mOldMustBeMemberFor);
    }
    //cout << " returning true; ";
    return true;
}



bool TCFResidenceTimeContinuous::allowedIndices(int oldindex, int newindex) { //called from void TCF::correlateWithLatest(int externalmaxdt) 
    //resizeOldIsMemberVector(oldindex, mOldMustBeMemberFor);
    //if (mOldMustBeMember == true) { 
        //return isOldAMember(oldindex, mOldMustBeMemberFor);
    //}
    return true;
}

//void TCFResidenceTimeContinuous::resizeOldIsMemberVector(int oldindex, int oldmustbememberfor) {
//    if (oldmustbememberfor == 0) {
//        if (oldindex >= mvOldIsMember.size()) {
//            //cout << "RESIZING mvOldIsMember: old size: " << mvOldIsMember.size() << " oldindex: " << oldindex << " value : " << (mInData1[oldindex] == 1) << endl;
//            mvOldIsMember.resize(oldindex+1, (mInData1[oldindex] == 1)); //this should only increase the size by 1
//        }
//        //return mInData1[oldindex] == 1;
//    }
//    else {
//        if (oldindex >= mvOldIsMember.size()) {
//            mvOldIsMember.resize(oldindex+1, isOldAMember(oldindex, oldmustbememberfor));
//        }
//    }
//}


bool TCFResidenceTimeContinuous::isOldAMember(int oldindex, int oldmustbememberfor) const { //called by allowedIndices(), this method is not const since it resizes/changes mOldIsMember
    //if (mvOldIsMember.size()>oldindex) return mvOldIsMember[oldindex]; //this mvOldIsMember vector only works if mNumItemsPerTimeStep is 1 (i.e. if (*this) is part of ActionResidenceTimeContinuous and not part of an ActionResidenceTimeCoordinatioShell)
    if (oldmustbememberfor == 0) {
        //cout << " quick and easy " << mInData1[oldindex] << endl;
        return mInData1[oldindex] == 1;
    }
    int extraoldindex = oldindex - oldmustbememberfor*mNumItemsPerTimeStep; //mNumItemsPerTimeStep == 1
    if (extraoldindex < 0) return false;
    //float sum=0, maxsum=0;
    int numcontinuouszeroes=0, numzeroes=0;
    for (int i=extraoldindex; i<=oldindex; i+=mNumItemsPerTimeStep) {
        if (mInData1[i] == 0) {
            numcontinuouszeroes++;
            numzeroes++;
            if (numcontinuouszeroes > mOldMaxContinuousEscapeTime) return false;
            if (numzeroes > mOldMaxTotalEscapeTime) return false;
        }
        else {
            numcontinuouszeroes=0;
        }
        //sum+=mInData1[i];
    }
    return true;
}
