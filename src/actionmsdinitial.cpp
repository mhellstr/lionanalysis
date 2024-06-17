
#include "actionmsdinitial.h"
#include "tcf.h"
#include "tcfmsd.h"

ActionMSDInitial::ActionMSDInitial() 
: ActionMSD()
{
    mNumItemsPerTimeStep=3;
}


void ActionMSDInitial::internalAddToTCF(int zerobasedid, const Atom* a) {
    ActionMSD::internalAddToTCF(zerobasedid, a);
    if (mpSubGroup->isMember(a)) {
        mTCF[zerobasedid]->addCare(true);
        //cout << "addcare true " << zerobasedid << endl;
    }
    else {
        mTCF[zerobasedid]->addCare(false);
        //cout << "addcare false " << zerobasedid << endl;
    }
}

TCF* ActionMSDInitial::newTCF() const {
    TCF *a = new TCFMSD;
    a->setCorrelateForward(true);
    a->setCorrelateForward(false);
    return a;
    //return new TCFMSD;
}

void ActionMSDInitial::addGroup(Group *g) {
    mvpGroups.push_back(g);
    mpGroup=mvpGroups[0];
    if (mvpGroups.size() > 1) {
        mpSubGroup = mvpGroups[1];
    }
}


//void ActionMSD::push_back_in_data(const Timestep *t) {
//    ActionTCF::push_back_in_data(t);
//}
//
//
