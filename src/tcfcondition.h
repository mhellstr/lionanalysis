#ifndef TCFCONDITION_H
#define TCFCONDITION_H
#include "consequence.h"
#include "group.h"
//enum class TCF_t { VACF, MSD, MRT, ResidenceTime, ResidenceTimeContinuous, ResidenceTimeCoordinationShell, ResidenceTimeSSP, ResidenceTimeSwitchBond };
//enum class CorrelationResult { Default, UncareWhatIsLeft, ZeroWhatIsLeft }
//
//enum class Consequence { NoConsequence, Uncare, Value };
//
namespace BoolInfoIndex {
enum {
    IsMember1,
    IsMember2,
    IsMember3,
    IsMember4,
    Value1,
    Value2,
    Value3,
    Value4,
    Unknown
};
}

enum class TCFConditionType {
    // these all check the "boolinfo" variables, one could also implement something that checks the values of the correlation funciton
    OldIsMember1,
    OldMustBeMemberFor1,
    OldValueIsTrueFor1,
    OldValueIsFalseFor1,
    NewIsMember1,
    NewMustBeMemberFor1,
    NewMustBeContinuousMember1,
    NewIsNotMember1,
    NewValueIsTrueFor1,
    NewValueIsFalseFor1,
    OldIsMember2,
    OldMustBeMemberFor2,
    OldValueIsTrueFor2,
    OldValueIsFalseFor2,
    NewIsMember2,
    NewMustBeMemberFor2,
    NewMustBeContinuousMember2,
    NewIsNotMember2,
    NewValueIsTrueFor2,
    NewValueIsFalseFor2,
    OldIsMember3,
    OldMustBeMemberFor3,
    OldValueIsTrueFor3,
    OldValueIsFalseFor3,
    NewIsMember3,
    NewMustBeMemberFor3,
    NewMustBeContinuousMember3,
    NewIsNotMember3,
    NewValueIsTrueFor3,
    NewValueIsFalseFor3,
    OldIsMember4,
    OldMustBeMemberFor4,
    OldValueIsTrueFor4,
    OldValueIsFalseFor4,
    NewIsMember4,
    NewMustBeMemberFor4,
    NewMustBeContinuousMember4,
    NewIsNotMember4,
    NewValueIsTrueFor4,
    NewValueIsFalseFor4,
    OldCoordinationFor1,
    NewCoordinationFor1,
    NewJumpMemberFor1 // for  TMSDFollow ONLY
//    UncareWhenUncare, //these three are specially implemented
//    UncareWhenValue,
//    ValueWhenValue,
};

int TCFConditionTypeToBoolInfoIndex(const TCFConditionType);
class TCFCondition {
public:
    TCFCondition(TCFConditionType ptype) :type(ptype), p1(0), p2(0), p3(0), returnvalue(0), result(true), bii(0), d1(0), d2(0), d3(0), g1(nullptr), g2(nullptr), g3(nullptr), negate(false), softhistory(false) { bii=TCFConditionTypeToBoolInfoIndex(ptype); };
    TCFConditionType type;
    int bii; //boolinfoindex
    Consequence consequence;
    int p1;
    int p2;
    int p3;
    int returnvalue;
    void setResult(bool a) { result = a; }
    bool result;
    Group *g1;
    Group *g2;
    Group *g3;
    double d1;
    double d2;
    double d3;
    bool negate;
    bool softhistory;
};


#endif
