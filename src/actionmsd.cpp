#include "actionmsd.h"
#include "tcf.h"
#include "tcfmsd.h"

ActionMSD::ActionMSD() 
: ActionTCF(ActionType::MSD,0,1000,1,1, TCF_t::MSD)
{
    mNumItemsPerTimeStep=3;
}

void ActionMSD::internalMoreAtomsThanExpected(int numatoms) {
    //int oldsize=mTCF.size();
    //for (int i=oldsize; i<numatoms; i++) {
    //    mTCF.push_back(newTCF()); //these are deleted in ActionTCF::~ActionTCF()
    //    mTCF[i]->reserve(mNumItemsPerTimeStep*mMaxTime);
    //    mTCF[i]->setMaxTime(mMaxTime);
    //    mTCF[i]->setNumItemsPerTimeStep(mNumItemsPerTimeStep);
    //}
    //mEntered.resize(numatoms, 0);
}

void ActionMSD::internalAtomLeftEnteredVector(int zerobasedid) {
    mTCF[zerobasedid]->clear();
}
void ActionMSD::internalAddToTCF(int zerobasedid, const Atom* a) {
    if (mDimX) mTCF[zerobasedid]->add(a->xu);
    if (mDimY) mTCF[zerobasedid]->add(a->yu);
    if (mDimZ) mTCF[zerobasedid]->add(a->zu);
}

TCF* ActionMSD::newTCF() const {
    return new TCFMSD;
}

//void ActionMSD::push_back_in_data(const Timestep *t) {
//    ActionTCF::push_back_in_data(t);
//}
