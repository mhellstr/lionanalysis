#ifndef TCFRESIDENCETIMECONTINUOUS_H
#define TCFRESIDENCETIMECONTINUOUS_H
#include "tcfresidencetime.h"

#define TCFRESIDENCETIMECONTINUOUS_NEWMEMBERFORISDT -1

enum class Membership {
    Simple,
    Complicated
};


class TCFResidenceTimeContinuous : public TCFResidenceTime {
public:
    TCFResidenceTimeContinuous();
    void setOldMaxContinuousEscapeTime(int a) {mOldMaxContinuousEscapeTime=a;}
    void setOldMaxTotalEscapeTime(int a) {mOldMaxTotalEscapeTime=a;}
    void setOldMustBeMemberFor(int a) {mOldMustBeMemberFor=a;}
    void setNewMaxContinuousEscapeTime(int a) {mNewMaxContinuousEscapeTime=a; }
    void setNewMaxTotalEscapeTime(int a) {mNewMaxTotalEscapeTime=a; }
    void setNewMustBeMember(bool a) {mNewMustBeMember = a; }
    void setNewMembershipType(Membership a) {mNewMembershipType = a; }
    //void setAllowRecrossings(bool a) {mAllowRecrossings=a; }
    void  correlateWithLatest(int externalmaxdt=-1) override;
    //void setNewMustBeMemberFor(int a) {mNewMustBeMemberFor=a;}
protected:
    //std::pair<float,int> internalCorrelateWithLatest(unsigned int, unsigned int) const override;
    std::pair<double, long long int> singleInternalCorrelateWithLatest(unsigned int, unsigned int) const;
    bool isOldAMember(int oldindex, int oldmustbememberfor) const;
    bool allowedIndices(int, int) override;
    bool perTimestepAllowedIndices(int, int) const;
    int mOldMustBeMemberFor;
    int mOldMaxContinuousEscapeTime; //maximum *continuous* escape time
    int mOldMaxTotalEscapeTime; 
    int mNewMaxContinuousEscapeTime; //maximum *continuous* escape time
    int mNewMaxTotalEscapeTime; 
    bool mNewMustBeMember;
    //bool mAllowRecrossings;
    //void resizeOldIsMemberVector(int oldindex, int oldmustbememberfor);
    Membership mNewMembershipType;
    //vector<bool> mvOldIsMember;
};
#endif
