#include "actiontresidencetimesspcoordinationshell.h"
#include "ttcfresidencetimessp.h"
ActionTResidenceTimeSSPCoordinationShell::ActionTResidenceTimeSSPCoordinationShell() 
: ActionTResidenceTimeSSP()
{
    mNumItemsPerTimeStep=1;
    setActionType(ActionType::ResidenceTimeSSPCoordinationShell);
    mMaxGroups=5;
}
ActionTResidenceTimeSSPCoordinationShell::ActionTResidenceTimeSSPCoordinationShell(ActionType actiontype, TCF_t tcftype)
: ActionTResidenceTimeSSP()
{ 
    mNumItemsPerTimeStep=1;
    setActionType(ActionType::ResidenceTimeSSPCoordinationShell);
    mTCFType = TCF_t::ResidenceTimeSSPCoordinationShell;
    mMaxGroups=5;
}

ActionTResidenceTimeSSPCoordinationShell::~ActionTResidenceTimeSSPCoordinationShell()
{ }


void ActionTResidenceTimeSSPCoordinationShell::internalMoreAtomsThanExpected(int numatoms) {
    mNumItemsPerTimeStep = mpShellGroup->size();
}

void ActionTResidenceTimeSSPCoordinationShell::internalAddToTCF(int zerobasedid, const Atom* a, const Timestep *t) {
    bool isreactant = mpReactants->isMember(a);
    bool isproduct = mpProducts->isMember(a);
    if (isreactant) {
        const Group* coordinationtable = static_cast<GroupCoordination*>(mpReactants)->getCoordinationTable(mpReactants->getMemberId(a));
        for (int j=0; j<mpShellGroup->size(); j++) { 
            if (coordinationtable->isMember(mpShellGroup->atom(j))) {
                mTCF[zerobasedid]->add(true);
                addValueBoolInfo(zerobasedid, true, 1);
            }
            else {
                mTCF[zerobasedid]->add(false);
                addValueBoolInfo(zerobasedid, false, 1);
            }
        }
    }
    else {
        for (int j=0; j<mpShellGroup->size(); j++) {
            mTCF[zerobasedid]->add(false);
            addValueBoolInfo(zerobasedid, false, 1);
        }
    }
    if (isproduct) {
        const Group* coordinationtable = static_cast<GroupCoordination*>(mpProducts)->getCoordinationTable(mpProducts->getMemberId(a));
        for (int j=0; j<mpShellGroup->size(); j++) { 
            if (coordinationtable->isMember(mpShellGroup->atom(j))) {
                mTCF[zerobasedid]->add2(true);
                addValueBoolInfo(zerobasedid, true, 2);
                //the idea here is to have an "addNewChange" when the coordination table changes in an ActionWhenGroupChanged
                //but you can't really do it, since you need to know that you've been in the REACTANT state some time before
                //and I don't want to implement the whole changegrouptime functionality here. So it's better to change ActionChangeGroupTime to include the possiblities of coordination table changes
                //if (mTCF[zerobasedid]->inData2Size() > mpShellGroup->size() && mTCF[zerobasedid]->getInData2(mTCF[zerobasedid]->inData2Size()-1-mpShellGroup->size()) == false) { 
                //    cout << "t->iteration: " << t->iteration << " zerobasedid: " << zerobasedid << " inData2Size: " << mTCF[zerobasedid]->inData2Size() << " mpShellGroup->size(): " << mpShellGroup->size() << " central atom: " << a->id << " shell atom: " << mpShellGroup->atom(j)->id << endl;
                //    for (int k=0; k<mvActionWhenGroupChanged.size(); k++) {
                //        if (mvActionWhenGroupChanged[k] != nullptr) {
                //            mvActionWhenGroupChanged[k]->addNewChange(mpShellGroup->atom(j), 0, 0, t->iteration);
                //        }
                //    }
                //}
            }
            else {
                mTCF[zerobasedid]->add2(false);
                addValueBoolInfo(zerobasedid, false, 2);
            }
        }
    }
    else {
        for (int j=0; j<mpShellGroup->size(); j++) {
            mTCF[zerobasedid]->add2(false);
            addValueBoolInfo(zerobasedid, false, 2);
        }
    }


}
void ActionTResidenceTimeSSPCoordinationShell::addGroup(Group *g) {
    ActionTResidenceTimeSSP::addGroup(g);
    if (mvpGroups.size() > 3) {
        mpShellGroup = mvpGroups[3];
    }
}

void ActionTResidenceTimeSSPCoordinationShell::addActionWhenGroupChanged(Action *a) {
    if (a == nullptr) return;
    mvActionWhenGroupChanged.push_back(dynamic_cast<ActionWhenGroupChangedParent*>(a));
    if (mvActionWhenGroupChanged.back() == nullptr) {
        throw string("ERROR! Need action of type ActionWhenGroupChanged in ActionChangeGroupTime");
    }
}
