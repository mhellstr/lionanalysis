#ifndef ACTIONRESIDENCETIMECONTINUOUS_H
#define ACTIONRESIDENCETIMECONTINUOUS_H


#include "actionresidencetime.h"
#include "tcfresidencetimecontinuous.h"

class ActionResidenceTimeContinuous : public ActionResidenceTime {
public:
    ActionResidenceTimeContinuous();
    void setOldMaxContinuousEscapeTime(float a, bool realtime=false);
    void setOldMaxTotalEscapeTime(float a, bool realtime=false);
    void setOldMustBeMemberFor(float a, bool realtime=false);
    void setNewMaxContinuousEscapeTime(float a, bool realtime=false);
    void setNewMaxTotalEscapeTime(float a, bool realtime=false);
    void setNewMustBeMember(bool a) {mNewMustBeMember = a; }
    void setNewMembershipType(Membership a) {mNewMembershipType=a;}
    //void setAllowRecrossings(bool a) {mAllowRecrossings = a;}
    //void setNewMustBeMemberFor(int a) {mNewMustBeMemberFor=a;}
    //void setNewMustBeMemberForCurrentDT() { mNewMustBeMemberFor=TCFRESIDENCETIMECONTINUOUS_NEWMEMBERFORISDT; }
protected:
    TCF* newTCF() const override;
    int mOldMustBeMemberFor;
    int mOldMaxContinuousEscapeTime; //maximum *continuous* escape time
    int mOldMaxTotalEscapeTime; 
    int mNewMaxContinuousEscapeTime; //maximum *continuous* escape time
    int mNewMaxTotalEscapeTime; 
    bool mNewMustBeMember;
    //bool mAllowRecrossings;
    Membership mNewMembershipType;
    float getGnuplotFitOffset() const override;
};



#endif
