#include "actionttcfparent.h"


ActionTTCFParent::ActionTTCFParent(ActionType type, int ming, int maxg, int mino, int maxo) 
:Action(type, ming, maxg, mino, maxo), mDoNormalize0(false), mDoPrintRawNumbers(true), mDoPrintHeader(true), mpShellGroup(nullptr), mMaxTime(10), mNumItemsPerTimeStep(1), mAverage(false), mDimX(true), mDimY(true), mDimZ(true), mTidyOld(2), mCorrelationFrequency(1), mpTCFFrom(nullptr), mDoPrintAllMembers(false), mUseAllTimeOrigins(true), mUncareAllWhenUncareRemaining(false), mValuePrecedenceOverUncare(false), mManipulateIndividualTCFs(false), mSimpleAddToTCF(true), mShiftBy(0)
{
    mvBoolsToSave.resize(64, false);
    setOption(ActionTTCFOptions::DoNormalize0, false);
    setOption(ActionTTCFOptions::DoPrintRawNumbers, true);
    setOption(ActionTTCFOptions::DoPrintHeader, true);
    setOption(ActionTTCFOptions::Average, true);
    setOption(ActionTTCFOptions::DoPrintAllMembers, false);
    setOption(ActionTTCFOptions::UseAllTimeOrigins, true);
    setOption(ActionTTCFOptions::UncareAllWhenUncareRemaining, false);
    setOption(ActionTTCFOptions::ValuePrecedenceOverUncare, false);
    setOption(ActionTTCFOptions::ManipulateIndividualTCFs, false);
    setOption(ActionTTCFOptions::Discretize, false);
    setOption(ActionTTCFOptions::DiscretizeContinuous, false);
    setOption(ActionTTCFOptions::DiscretizeDiscrete, false);
    setOption(ActionTTCFOptions::DoPrintVectorOut, false);
    setOption(ActionTTCFOptions::Hungarian, false);
    setOption(ActionTTCFOptions::IgnoreConflicts, false);
    setOption(ActionTTCFOptions::DoPrintIds, false);
    setOption(ActionTTCFOptions::HungarianSquaredPenalty, false);
}

void ActionTTCFParent::setOption(int option, bool value) {
    mOptions[option]=value;
}

bool ActionTTCFParent::getOption(int option) const {
    return mOptions[option];
}

void ActionTTCFParent::setMaxTime(float a, bool realtime) {
    if (!realtime) {
        mMaxTime = int(a);
    }
    else {
        mMaxTime = int( round(a / mTimeUnit) );
    }
}

void ActionTTCFParent::setCorrelationFrequency(float a, bool realtime) {
    if (!realtime) {
        mCorrelationFrequency = int(a);
    }
    else {
        mCorrelationFrequency = int( round(a / mTimeUnit) );
    }
}


//template <typename Tin, typename Tout>
//void ActionTTCF<Tin,Tout>::setRealTime(float realtime) {
void ActionTTCFParent::setRealTime(float realtime) {
    int maxtime = int( realtime / mTimeUnit );
    setMaxTime(maxtime);
}

void ActionTTCFParent::addGroup(Group *g) {
    mvpGroups.push_back(g);
    mpGroup=mvpGroups[0];
    if (mvpGroups.size() > 1) {
        mpSubGroup = mvpGroups[1];
    }
}


//template <typename Tin, typename Tout>
//void ActionTTCF<Tin,Tout>::addCondition(TCFCondition a) { 
void ActionTTCFParent::addCondition(TCFCondition a, bool realtime) { 
    if (a.type == TCFConditionType::OldMustBeMemberFor1 ||
        a.type == TCFConditionType::OldMustBeMemberFor2 ||
        a.type == TCFConditionType::OldMustBeMemberFor3 ||
        a.type == TCFConditionType::OldMustBeMemberFor4 ||
        a.type == TCFConditionType::NewMustBeMemberFor1 ||
        a.type == TCFConditionType::NewMustBeMemberFor2 ||
        a.type == TCFConditionType::NewMustBeMemberFor3 ||
        a.type == TCFConditionType::NewMustBeMemberFor4 ||
        a.type == TCFConditionType::OldValueIsTrueFor1  ||
        a.type == TCFConditionType::OldValueIsFalseFor1 ||
        a.type == TCFConditionType::NewValueIsTrueFor1  ||
        a.type == TCFConditionType::NewValueIsFalseFor1 
        ) {
            if (realtime) {
                a.p1 = int( round(a.d1 / mTimeUnit) );
                a.p2 = int( round(a.d2 / mTimeUnit) );
                a.p3 = int( round(a.d3 / mTimeUnit) );
                //cout << "RealTime: a.p1,p2,p3 = " << a.p1 << " " << a.p2 << " " << a.p3 << " " << mTimeUnit << " " << a.d1 << " " << a.d2 << " " << a.d3 << " ff " << a.d2/mTimeUnit << " " << int(a.d2/mTimeUnit) << " " << endl;
            }
            else {
                a.p1 = int(a.d1);
                a.p2 = int(a.d2);
                a.p3 = int(a.d3);
            }
    }
    mvTCFConditions.push_back(a); 
    //cout << mvTCFConditions.size() <<  " " << getDescription() << endl;
    switch (a.type) {
        case TCFConditionType::OldIsMember1:
        case TCFConditionType::OldMustBeMemberFor1:
        case TCFConditionType::NewIsMember1:
        case TCFConditionType::NewMustBeMemberFor1:
        case TCFConditionType::NewMustBeContinuousMember1:
        case TCFConditionType::NewIsNotMember1:
            mvBoolsToSave[static_cast<int>(BoolInfoIndex::IsMember1)]=true;
            break;
        case TCFConditionType::OldIsMember2:
        case TCFConditionType::OldMustBeMemberFor2:
        case TCFConditionType::NewIsMember2:
        case TCFConditionType::NewMustBeMemberFor2:
        case TCFConditionType::NewMustBeContinuousMember2:
        case TCFConditionType::NewIsNotMember2:
            mvBoolsToSave[static_cast<int>(BoolInfoIndex::IsMember2)]=true;
            break;
        case TCFConditionType::OldIsMember3:
        case TCFConditionType::OldMustBeMemberFor3:
        case TCFConditionType::NewIsMember3:
        case TCFConditionType::NewMustBeMemberFor3:
        case TCFConditionType::NewMustBeContinuousMember3:
        case TCFConditionType::NewIsNotMember3:
            mvBoolsToSave[static_cast<int>(BoolInfoIndex::IsMember3)]=true;
            break;
        case TCFConditionType::OldIsMember4:
        case TCFConditionType::OldMustBeMemberFor4:
        case TCFConditionType::NewIsMember4:
        case TCFConditionType::NewMustBeMemberFor4:
        case TCFConditionType::NewMustBeContinuousMember4:
        case TCFConditionType::NewIsNotMember4:
            mvBoolsToSave[static_cast<int>(BoolInfoIndex::IsMember4)]=true;
            break;
        case TCFConditionType::OldValueIsTrueFor1:
        case TCFConditionType::OldValueIsFalseFor1:
        case TCFConditionType::NewValueIsTrueFor1:
        case TCFConditionType::NewValueIsFalseFor1:
            mvBoolsToSave[static_cast<int>(BoolInfoIndex::Value1)]=true;
            break;
    }
}

