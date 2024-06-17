#include "actiontmsd.h"
#include "ttcf.h"
#include "ttcfmsd.h"

ActionTMSD::ActionTMSD() 
: ActionTTCF<INTYPE,OUTTYPE>(ActionType::MSD,2,3,1,5, TCF_t::MSD)
{
    mNumItemsPerTimeStep=3;
}
ActionTMSD::ActionTMSD(ActionType actiontype, TCF_t tcftype)
: ActionTTCF<INTYPE,OUTTYPE>(actiontype, 2, 3, 1, 5, tcftype)
{ 
    mNumItemsPerTimeStep=3;
}

ActionTMSD::~ActionTMSD()
{ }


void ActionTMSD::internalMoreAtomsThanExpected(int numatoms) {
}

void ActionTMSD::internalAtomLeftEnteredVector(int zerobasedid) {
    //mTCF[zerobasedid]->clear(); important to NOT clear this thing for ResidenceTime
}
void ActionTMSD::internalAddToTCF(int zerobasedid, const Atom* a) {
    //cout << mpSubGroup->getId() << endl;
    //cout << mpGroup->getId() << endl;
    if (mDimX) {
        //if (zerobasedid==1) cout << "adding x " << a->xu << endl;
        mTCF[zerobasedid]->add(a->xu);
        //if (mvBoolsToSave[BoolInfoIndex::IsMember1]) mTCF[zerobasedid]->addBoolInfo(mpSubGroup->isMember(a), BoolInfoIndex::IsMember1);
    }
    if (mDimY) {
        //if (zerobasedid==1) cout << "adding y " << a->yu << endl;
        mTCF[zerobasedid]->add(a->yu);
        //if (mvBoolsToSave[BoolInfoIndex::IsMember1]) mTCF[zerobasedid]->addBoolInfo(mpSubGroup->isMember(a), BoolInfoIndex::IsMember1);
    }
    if (mDimZ) {
        //if (zerobasedid==1) cout << "adding z " << a->zu << endl;
        mTCF[zerobasedid]->add(a->zu);
        //if (mvBoolsToSave[BoolInfoIndex::IsMember1]) mTCF[zerobasedid]->addBoolInfo(mpSubGroup->isMember(a), BoolInfoIndex::IsMember1);
    }
}

TTCF<ActionTMSD::INTYPE, ActionTMSD::OUTTYPE>* ActionTMSD::newTCF() const {
    TTCF<INTYPE, OUTTYPE>* a = new TTCFMSD(this);
    //a->setParentAction(this);
    //setTTCFConditions(a);
    //static_cast<TTCFResidenceTime*>(a)->setOldMustBeMember(mOldMustBeMember);
    //static_cast<TTCFResidenceTime*>(a)->setTidyOld(mTidyOld);
    return a;
}


void ActionTMSD::internalFinalAction(const vector<Atom>* allatoms, const Timestep *t) {
    ActionTTCF::internalFinalAction(allatoms, t);
}
