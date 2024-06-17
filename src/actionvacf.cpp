#include "actionvacf.h"
#include "tcf.h"
#include "tcfvacf.h"

ActionVACF::ActionVACF() 
: ActionTCF(ActionType::VACF,0,1000,1,1, TCF_t::VACF)
{
    mNumItemsPerTimeStep=3;
}

void ActionVACF::internalMoreAtomsThanExpected(int numatoms) {
}

void ActionVACF::internalAtomLeftEnteredVector(int zerobasedid) {
    mTCF[zerobasedid]->clear();
}
void ActionVACF::internalAddToTCF(int zerobasedid, const Atom* a) {
    if (mDimX) mTCF[zerobasedid]->add(a->vx);
    if (mDimY) mTCF[zerobasedid]->add(a->vy);
    if (mDimZ) mTCF[zerobasedid]->add(a->vz);
}

TCF* ActionVACF::newTCF() const {
    return new TCFVACF;
}
