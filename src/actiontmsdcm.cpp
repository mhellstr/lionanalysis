#include "actiontmsdcm.h"

ActionTMSDCM::ActionTMSDCM() :ActionTMSD()
{
}



void ActionTMSDCM::internalAddToTCF(int zerobasedid, const Atom* a) {
    //calculate center of mass
    double centerx=0, centery=0, centerz=0, summass=0;
    for (int i=0; i<mpGroup->size(); i++) {
        const Atom* a=mpGroup->atom(i);
        centerx+= a->xu;
        centery+= a->yu;
        centerz+= a->zu;
        summass+=a->mass;
    }
    if (summass == 0) cout << "WARNING: SUM OF MASS IS 0. SHOULD NOT HAPPEN" << endl;
    centerx/=mpGroup->size();
    centery/=mpGroup->size();
    centerz/=mpGroup->size();
    //if (zerobasedid==0) {
    //    cout << "group " << mpGroup->getId() << "  center is " << centerx << " " << centery << " " << centerz << " summas = " << summass << endl;
    //}
    if (mDimX) {
        mTCF[zerobasedid]->add(centerx);
    }
    if (mDimY) {
        mTCF[zerobasedid]->add(centery);
    }
    if (mDimZ) {
        mTCF[zerobasedid]->add(centerz);
    }
}

