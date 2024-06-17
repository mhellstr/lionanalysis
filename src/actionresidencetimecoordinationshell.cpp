#include "actionresidencetimecoordinationshell.h"
#include "tcf.h"
#include "tcfresidencetimecoordinationshell.h"
#include "tcfresidencetimecontinuous.h"

ActionResidenceTimeCoordinationShell::ActionResidenceTimeCoordinationShell() 
//: ActionTCF(ActionType::ResidenceTimeCoordinationShell,3,3,1,1, TCF_t::ResidenceTimeCoordinationShell)
: ActionResidenceTimeContinuous()
{
    setActionType(ActionType::ResidenceTimeCoordinationShell);
}

void ActionResidenceTimeCoordinationShell::internalMoreAtomsThanExpected(int numatoms) {
    //this function is called from ActionTCF::internalAction when mpGroup->size() is greater than before
    //in case of ResidenceTimeCoordinationShell, this should only happen the first time the action is called
    //since the size of mpGroup should be constant
    //the number of "items" per timestep should be the number of atoms, so this is initialized HERE
    //cout << "mpShellGroup->getId() : " << mpShellGroup->getId() << " mpShellGroup->size()=" << mpShellGroup->size()  << endl;
    mNumItemsPerTimeStep = mpShellGroup->size();
}

void ActionResidenceTimeCoordinationShell::internalAtomLeftEnteredVector(int zerobasedid) {
    //mTCF[zerobasedid]->clear(); important to NOT clear this thing for ResidenceTimeCoordinationShell
}
void ActionResidenceTimeCoordinationShell::internalAddToTCF(int zerobasedid, const Atom* a) {
    //there is no guarantee that the members of coordinationtable are sorted in ascending order
    //with respect to atomic id, so at the moment use the below slow way of finding out which
    //zeroes and ones to add
    //cout << getDescription() << "time unit is " << mTimeUnit << endl;
    if (mpSubGroup->isMember(a)) {
        const Group* coordinationtable = static_cast<GroupCoordination*>(mpSubGroup)->getCoordinationTable(mpSubGroup->getMemberId(a));
        for (int j=0; j<mpShellGroup->size(); j++) { 
            if (coordinationtable->isMember(mpShellGroup->atom(j))) mTCF[zerobasedid]->add(1);
            else mTCF[zerobasedid]->add(0);
        }
    }
    else {
        for (int j=0; j<mpShellGroup->size(); j++) {
            mTCF[zerobasedid]->add(0);
        }
    }
}

//TCF* ActionResidenceTimeCoordinationShell::newTCF() const {
    //return new TCFResidenceTimeCoordinationShell;
    //return new TCFResidenceTimeContinuous;
    //TCF* a = new TCFResidenceTimeContinuous;
    //static_cast<TCFResidenceTimeContinuous*>(a)->setOldMustBeMember(true); //shouldnt really be used for continuous
    //static_cast<TCFResidenceTimeContinuous*>(a)->setOldMaxTotalEscapeTime(mOldMaxTotalEscapeTime);
    //static_cast<TCFResidenceTimeContinuous*>(a)->setOldMaxContinuousEscapeTime(mOldMaxContinuousEscapeTime);
    //static_cast<TCFResidenceTimeContinuous*>(a)->setOldMustBeMemberFor(mOldMustBeMemberFor);
    //static_cast<TCFResidenceTimeContinuous*>(a)->setNewMaxTotalEscapeTime(mNewMaxTotalEscapeTime);
    //static_cast<TCFResidenceTimeContinuous*>(a)->setNewMaxContinuousEscapeTime(mNewMaxContinuousEscapeTime);
    //static_cast<TCFResidenceTimeContinuous*>(a)->setNewMustBeMember(mNewMustBeMember);
    //static_cast<TCFResidenceTimeContinuous*>(a)->setNewMembershipType(mNewMembershipType);
    //return a;
//}

void ActionResidenceTimeCoordinationShell::addGroup(Group *g) {
    mvpGroups.push_back(g);
    mpGroup=mvpGroups[0]; //should be a CONST SIZE group (static)
    if (mvpGroups.size() > 1) {
        mpSubGroup = dynamic_cast<GroupCoordination*>(mvpGroups[1]); //can be variably sized, no problem
        if (mpSubGroup == nullptr) {
            throw string("Error in ActionResidenceTimeCoordinationShell::addGroup(); second group to be added needs to be of type GroupCoordination - " + mvpGroups[1]->getId());
        }
        if (mvpGroups.size() > 2) {
            mpShellGroup = mvpGroups[2]; //should be a CONST SIZE group (static)
        }
    }
}
