#include "actionresidencetimessp.h"
#include "tcfresidencetimessp.h"

ActionResidenceTimeSSP::ActionResidenceTimeSSP()
: ActionResidenceTime(ActionType::ResidenceTimeSSP, TCF_t::ResidenceTimeSSP)
{
    mNumItemsPerTimeStep=1;
    mMinGroups=3; mMaxGroups=3;
}

TCF* ActionResidenceTimeSSP::newTCF() const {
    TCF* a = new TCFResidenceTimeSSP;
    static_cast<TCFResidenceTimeSSP*>(a)->setOldMustBeMember(mOldMustBeMember);
    static_cast<TCFResidenceTimeSSP*>(a)->setTidyOld(mTidyOld);
    static_cast<TCFResidenceTimeSSP*>(a)->reserve(mNumItemsPerTimeStep*mMaxTime*1000);
    return a;
}



void ActionResidenceTimeSSP::internalAddToTCF(int zerobasedid, const Atom* a) {
    const bool reacmem=mpReactants->isMember(a);
    const bool prodmem=mpProducts->isMember(a);
    //add two numbers simultaneously to ensure that the mInData1 and mInData2 vectors in mTCF[zerobasedid] always have the same size
    static_cast<TCFResidenceTimeSSP*>(mTCF[zerobasedid])->addCare(true);
    if      (    reacmem && (!prodmem) ) mTCF[zerobasedid]->add(1,0); //most frequent first
    else if ( (!reacmem) && (!prodmem) ) mTCF[zerobasedid]->add(0,0);
    else if ( (!reacmem) && prodmem    ) {
        mTCF[zerobasedid]->add(0,1);
        //if (mpReactants->isMember(zerobasedid+1)) {
        //    cout << zerobasedid << " is product" << endl;
        //}
    }
    else if (    reacmem && prodmem    ) {
        mTCF[zerobasedid]->add(1,1); //this should NEVER happen if input is sensible
        cout << "WARNING: Atom " << a->type << " " << a->id << " is a member of BOTH reactants " << mpReactants->getId() << " and products " << mpProducts->getId() << " for action " << getDescription() << " - check your input!" << endl;
    }
}

void ActionResidenceTimeSSP::addGroup(Group *g) {
    mvpGroups.push_back(g);
    mpGroup=mvpGroups[0];
    if (mvpGroups.size() > 1) {
        mpReactants=mvpGroups[1];
        if (mvpGroups.size() > 2) {
            mpProducts = mvpGroups[2];
        }
    }
}


void ActionResidenceTimeSSP::internalFinalAction(const vector<Atom>* allatoms, const Timestep *t) {
    for (int j=0; j<mpGroup->size(); j++) {
        int startdt=mMaxTime-2;
        if (mMaxTime > mCount) startdt=mMaxTime-1;
        for (int i=startdt; i>=0; i--) {
            //cout << "final correlation for atom " << j << " with maxdt " << i << endl;
            mTCF[j]->correlateWithLatest(i);
        }
    }
    ActionResidenceTime::internalFinalAction(allatoms, t);
}
