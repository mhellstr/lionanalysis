#include "actiontchargeflux.h"

ActionTChargeFlux::ActionTChargeFlux() :ActionTVACF()
{
}



void ActionTChargeFlux::internalAddToTCF(int zerobasedid, const Atom* a) {
    double sumx=0,sumy=0,sumz=0;
    for (int i=0; i<mvpGroups[1]->size(); i++) {
        const Atom *a = mvpGroups[1]->atom(i);
        sumx+=a->mass*a->vx;
        sumy+=a->mass*a->vy;
        sumz+=a->mass*a->vz;
        //cout << a->id << " " << a->type << " " << a->mass << " " << sumx << " " << sumy << " " << sumz << endl;
    }
    if (mDimX) {
        mTCF[zerobasedid]->add(sumx);
    }
    if (mDimY) {
        mTCF[zerobasedid]->add(sumy);
    }
    if (mDimZ) {
        mTCF[zerobasedid]->add(sumz);
    }
}

