#include "actiontresidencetimecoordinationshell.h"
#include "ttcfresidencetime.h"
ActionTResidenceTimeCoordinationShell::ActionTResidenceTimeCoordinationShell() 
: ActionTResidenceTime()
{
    mNumItemsPerTimeStep=1;
    setActionType(ActionType::ResidenceTimeCoordinationShell);
    mMaxGroups=5;
}
ActionTResidenceTimeCoordinationShell::ActionTResidenceTimeCoordinationShell(ActionType actiontype, TCF_t tcftype)
: ActionTResidenceTime(actiontype, tcftype)
{ 
    mNumItemsPerTimeStep=1;
    setActionType(ActionType::ResidenceTimeCoordinationShell);
    mMaxGroups=5;
}

ActionTResidenceTimeCoordinationShell::~ActionTResidenceTimeCoordinationShell()
{ }


void ActionTResidenceTimeCoordinationShell::internalMoreAtomsThanExpected(int numatoms) {
    //this function is called from ActionTCF::internalAction when mpGroup->size() is greater than before
    //in case of ResidenceTimeCoordinationShell, this should only happen the first time the action is called
    //since the size of mpGroup should be constant
    //the number of "items" per timestep should be the number of atoms, so this is initialized HERE
    //cout << "mpShellGroup->getId() : " << mpShellGroup->getId() << " mpShellGroup->size()=" << mpShellGroup->size()  << endl;
    mNumItemsPerTimeStep = mpShellGroup->size();
}

void ActionTResidenceTimeCoordinationShell::internalAddToTCF(int zerobasedid, const Atom* a) {
    //there is no guarantee that the members of coordinationtable are sorted in ascending order
    //with respect to atomic id, so at the moment use the below slow way of finding out which
    //zeroes and ones to add
    //cout << getDescription() << "time unit is " << mTimeUnit << endl;
    
    //the internalAddToTCF functions is called for ALL atoms in ParentGroup, so if (!ismember1) then the coordinationtable WILL BE UNDEFINED
    const Group* coordinationtable = static_cast<GroupCoordination*>(mpSubGroup)->getCoordinationTable(mpSubGroup->getMemberId(a));
    bool ismember1=mpSubGroup->isMember(a); //is the CENTER atom a member of the correlation group?
    for (int j=0; j<mpShellGroup->size(); j++) { //loop over ALL possible shell atoms
        const Atom* shellatom=mpShellGroup->atom(j);
        //vector<bool> vAddedBoolInfo(64, false);
        //for (int k=0; k<mvTCFConditions.size(); k++) {
        //    const TCFCondition *t = &mvTCFConditions[k];
        //    if (!vAddedBoolInfo[t->bii]) {
        //        switch (t->bii) {
        //            case BoolInfoIndex::IsMember1: //central atom
        //                mTCF[zerobasedid]->addBoolInfo(t->g1->isMember(a), t->bii);
        //                vAddedBoolInfo[t->bii]=true;
        //                break;
        //            case BoolInfoIndex::IsMember2: // only for SSP, central atom for "products"
        //                break;
        //            case BoolInfoIndex::IsMember3: //coordination shell
        //                mTCF[zerobasedid]->addBoolInfo(t->g1->isMember(shellatom), t->bii);
        //                vAddedBoolInfo[t->bii]=true;
        //                break;
        //            case BoolInfoIndex::IsMember4: // only for SSP, coordination shell for "products"
        //                break;
        //        }
        //    }
        //}
        /*
        if (mvBoolsToSave[BoolInfoIndex::IsMember1]) { //center, ismember1
            //cout << "adding boolinfo" << zerobasedid << endl;
            mTCF[zerobasedid]->addBoolInfo(ismember1, BoolInfoIndex::IsMember1);
        }
        if (mvBoolsToSave[BoolInfoIndex::IsMember3]) { //shell (of "reactant"), ismember3
            //mpShellGroup1 is the "condition" group for the shell
            mTCF[zerobasedid]->addBoolInfo(mpShellGroup1->isMember(shellatom), BoolInfoIndex::IsMember3);
        }
        */

        //cout << zerobasedid << " " << j << " " << a->id << " " << shellatom->id <<  " " << coordinationtable->size() << " " << coordinationtable->atom(0) << endl;
        bool value;
        if (ismember1) {
            value=coordinationtable->isMember(shellatom);
        }
        else {
            value=false;
        }
        //cout << "out" << endl;
        //vector<bool> vAddedBoolInfo(16,false);
        //for (int i=0; i<mvTCFConditions.size(); i++) {
        //    switch (mvTCFConditions[i].type)  {
        //        case TCFConditionType::OldValueIsTrueFor1:
        //        case TCFConditionType::OldValueIsFalseFor1:
        //        case TCFConditionType::OldValueIsTrueFor2:
        //        case TCFConditionType::OldValueIsFalseFor2:
        //        case TCFConditionType::OldValueIsTrueFor3:
        //        case TCFConditionType::OldValueIsFalseFor3:
        //        case TCFConditionType::OldValueIsTrueFor4:
        //        case TCFConditionType::OldValueIsFalseFor4:
        //        case TCFConditionType::NewValueIsTrueFor1:
        //        case TCFConditionType::NewValueIsFalseFor1:
        //        case TCFConditionType::NewValueIsTrueFor2:
        //        case TCFConditionType::NewValueIsFalseFor2:
        //        case TCFConditionType::NewValueIsTrueFor3:
        //        case TCFConditionType::NewValueIsFalseFor3:
        //        case TCFConditionType::NewValueIsTrueFor4:
        //        case TCFConditionType::NewValueIsFalseFor4:
        //            if (!vAddedBoolInfo[mvTCFConditions[i].bii]) {
        //                mTCF[zerobasedid]->addBoolInfo(value, mvTCFConditions[i].bii);
        //            }
        //            vAddedBoolInfo[mvTCFConditions[i].bii]=true;
        //            break;
        //    }
        //}
        //if (mvBoolsToSave[BoolInfoIndex::Value1]) {
            //mTCF[zerobasedid]->addBoolInfo(value, BoolInfoIndex::Value1);
        //}
        mTCF[zerobasedid]->add(value);
        addValueBoolInfo(zerobasedid, value, 1);
        //if (mvTCFConditions.size() > 0 && mBoolInfo.size() > 0) {
            //cout << mTCF[zerobasedid]->size() << " " << mBoolInfo
        //}
    }
}

void ActionTResidenceTimeCoordinationShell::addGroup(Group *g) {
    mvpGroups.push_back(g);
    mpGroup=mvpGroups[0]; //should be a CONST SIZE group (static)
    if (mvpGroups.size() > 1) {
        mpSubGroup = dynamic_cast<GroupCoordination*>(mvpGroups[1]); //can be variably sized, no problem
        if (mpSubGroup == nullptr) {
            throw string("Error in ActionResidenceTimeCoordinationShell::addGroup(); second group to be added needs to be of type GroupCoordination - " + mvpGroups[1]->getId());
        }
        if (mvpGroups.size() > 2) {
            mpShellGroup = mvpGroups[2]; //should be a CONST SIZE group (static)
            if (mvpGroups.size() > 3) {
                mpShellGroup1 = mvpGroups[3]; //condition group for the shell
            }
        }
    }
}
