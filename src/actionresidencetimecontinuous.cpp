#include "actionresidencetimecontinuous.h"

ActionResidenceTimeContinuous::ActionResidenceTimeContinuous()
: ActionResidenceTime(ActionType::ResidenceTimeContinuous, TCF_t::ResidenceTimeContinuous),
mOldMaxTotalEscapeTime(1000000), mOldMaxContinuousEscapeTime(1000000), mOldMustBeMemberFor(0), 
mNewMaxTotalEscapeTime(1000000), mNewMaxContinuousEscapeTime(1000000), mNewMustBeMember(false),
mNewMembershipType(Membership::Simple)
{ 
    mAllowRecrossings=false;
}


TCF* ActionResidenceTimeContinuous::newTCF() const {
    TCF* a = new TCFResidenceTimeContinuous;
    static_cast<TCFResidenceTimeContinuous*>(a)->setOldMustBeMember(mOldMustBeMember); //shouldnt really be used for continuous
    static_cast<TCFResidenceTimeContinuous*>(a)->setOldMaxTotalEscapeTime(mOldMaxTotalEscapeTime);
    static_cast<TCFResidenceTimeContinuous*>(a)->setOldMaxContinuousEscapeTime(mOldMaxContinuousEscapeTime);
    static_cast<TCFResidenceTimeContinuous*>(a)->setOldMustBeMemberFor(mOldMustBeMemberFor);
    static_cast<TCFResidenceTimeContinuous*>(a)->setNewMaxTotalEscapeTime(mNewMaxTotalEscapeTime);
    static_cast<TCFResidenceTimeContinuous*>(a)->setNewMaxContinuousEscapeTime(mNewMaxContinuousEscapeTime);
    static_cast<TCFResidenceTimeContinuous*>(a)->setNewMustBeMember(mNewMustBeMember);
    static_cast<TCFResidenceTimeContinuous*>(a)->setNewMembershipType(mNewMembershipType);
    static_cast<TCFResidenceTimeContinuous*>(a)->setAllowRecrossings(mAllowRecrossings);
    static_cast<TCFResidenceTimeContinuous*>(a)->setTidyOld(mTidyOld);
    return a;
}



float ActionResidenceTimeContinuous::getGnuplotFitOffset() const { 
    if (mNewMembershipType == Membership::Complicated) {
        if (mNewMustBeMember) return 0;
        else return mTimeUnit * mNewMaxContinuousEscapeTime; 
    }
    else {
        return 0; 
    }
};

void ActionResidenceTimeContinuous::setOldMaxContinuousEscapeTime(float a, bool realtime) {
    if (realtime) { a/=mTimeUnit; }
    mOldMaxContinuousEscapeTime=int(round(a));
}
void ActionResidenceTimeContinuous::setOldMaxTotalEscapeTime(float a, bool realtime) {
    if (realtime) { a/=mTimeUnit; }
    mOldMaxTotalEscapeTime=int(round(a));
}
void ActionResidenceTimeContinuous::setOldMustBeMemberFor(float a, bool realtime) {
    if (realtime) { a/=mTimeUnit; }
    if ( a<0) a=0;
    mOldMustBeMemberFor=int(round(a));
}
void ActionResidenceTimeContinuous::setNewMaxContinuousEscapeTime(float a, bool realtime) {
    if (realtime) { a/=mTimeUnit; }
    mNewMaxContinuousEscapeTime=int(round(a)); 
    if (a >= 0) setNewMembershipType(Membership::Complicated); 
}
void ActionResidenceTimeContinuous::setNewMaxTotalEscapeTime(float a, bool realtime) {
    if (realtime) { a/=mTimeUnit; }
    mNewMaxTotalEscapeTime=int(round(a));  
    if (a >= 0) setNewMembershipType(Membership::Complicated); 
}
