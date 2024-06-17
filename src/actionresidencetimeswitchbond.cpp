#include "actionresidencetimeswitchbond.h"
#include "tcfresidencetimeswitchbond.h"

ActionResidenceTimeSwitchBond::ActionResidenceTimeSwitchBond()
: ActionResidenceTimeCoordinationShell(), mOverwriteInData(false)
{
    mNumItemsPerTimeStep=1;
    mMinGroups=3; mMaxGroups=3;
    setActionType(ActionType::ResidenceTimeSwitchBond);
    setOldMustBeMember(true);
    setAllowRecrossings(false);
}

TCF* ActionResidenceTimeSwitchBond::newTCF() const {
    TCF* a = new TCFResidenceTimeSwitchBond;
    static_cast<TCFResidenceTimeSwitchBond*>(a)->setOldMustBeMember(mOldMustBeMember);
    static_cast<TCFResidenceTimeSwitchBond*>(a)->setAllowRecrossings(mAllowRecrossings);
    static_cast<TCFResidenceTimeSwitchBond*>(a)->setOverwriteInData(mOverwriteInData);
    static_cast<TCFResidenceTimeSwitchBond*>(a)->reserve(mNumItemsPerTimeStep*mMaxTime*1000);
    return a;
}


