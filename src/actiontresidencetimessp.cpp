#include "actiontresidencetimessp.h"
#include "ttcfresidencetimessp.h"

ActionTResidenceTimeSSP::ActionTResidenceTimeSSP()
: ActionTResidenceTime(ActionType::ResidenceTimeSSP, TCF_t::ResidenceTimeSSP)
{
    mNumItemsPerTimeStep=1;
    mMinGroups=3; mMaxGroups=3;
}

TTCF<bool, long long int>* ActionTResidenceTimeSSP::newTCF() const {
    TTCF<bool, long long int>* a = new TTCFResidenceTimeSSP(this);
    a->reserve(mNumItemsPerTimeStep*mMaxTime*1000);
    return a;
}

void ActionTResidenceTimeSSP::internalAddToTCF(int zerobasedid, const Atom* a) {
    const bool reacmem=mpReactants->isMember(a);
    const bool prodmem=mpProducts->isMember(a);
    //add two numbers simultaneously to ensure that the mInData1 and mInData2 vectors in mTCF[zerobasedid] always have the same size
    mTCF[zerobasedid]->add(reacmem, prodmem);
    addValueBoolInfo(zerobasedid, reacmem, 1);
    addValueBoolInfo(zerobasedid, prodmem, 2);
    if (    reacmem && prodmem    ) {
        cout << "WARNING: Atom " << a->type << " " << a->id << " is a member of BOTH reactants " << mpReactants->getId() << " and products " << mpProducts->getId() << " for action " << getDescription() << " - check your input!" << endl;
    }
    //if (mvBoolsToSave[BoolInfoIndex::IsMember1]) {
    //    mTCF[zerobasedid]->addBoolInfo(reacmem, BoolInfoIndex::IsMember1);
    //}
    //if (mvBoolsToSave[BoolInfoIndex::IsMember2]) {
    //    mTCF[zerobasedid]->addBoolInfo(prodmem, BoolInfoIndex::IsMember2);
    //}
}

void ActionTResidenceTimeSSP::addGroup(Group *g) {
    mvpGroups.push_back(g);
    mpGroup=mvpGroups[0];
    if (mvpGroups.size() > 1) {
        mpReactants=mvpGroups[1];
        if (mvpGroups.size() > 2) {
            mpProducts = mvpGroups[2];
        }
    }
}


void ActionTResidenceTimeSSP::internalFinalAction(const vector<Atom>* allatoms, const Timestep *t) {
    ActionTResidenceTime::internalFinalAction(allatoms, t);
}
