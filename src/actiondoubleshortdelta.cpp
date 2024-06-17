#include "actiondoubleshortdelta.h"
#include "actionshortdelta.h"
#include "anglerdf.h"
#include <algorithm>

ActionDoubleShortDelta::ActionDoubleShortDelta()
:Action(ActionType::DoubleShortDelta, 0, 0, 1, 1), mpMiddle1(nullptr), mpEnd1(nullptr), mpMiddle2(nullptr), mpEnd2(nullptr), mResolution(0.1)
{
}


void ActionDoubleShortDelta::internalAction(const vector<Atom>* allatoms, const Timestep *t) {
    if (mCount == 1) {
        mOut.resize(int(mMaxDelta/mResolution)+1);
        for (int i=0; i<mOut.size(); i++) {
            mOut[i].resize(int(mMaxDelta/mResolution)+1, 0);
        }
    }
    //if (mNewGroup1 != nullptr) { mNewGroup1->clear(); }
    //if (mNewGroup2 != nullptr) { mNewGroup2->clear(); }
    //if (mNewGroup3 != nullptr) { mNewGroup3->clear(); }
    //cout << mOut.size() << endl;
    for (int i=0; i<mpGroup->size(); i++) {
        //cout << "i = " << i << " ";
        const Atom* const atom1=mpGroup->atom(i);
        const Atom* atom2=nullptr;
        const Atom* atom3=nullptr;
        double delta1=0, delta2=0;
        bool success=false;
        findShortDelta(atom1, atom2, atom3, mpGroup, mpMiddle1, mpEnd1, nullptr, false, delta1, success);
        if (!success) continue;
        findShortDelta(atom1, atom2, atom3, mpGroup, mpMiddle2, mpEnd2, nullptr, false, delta2, success);

        if (!success) continue;
        if (delta1 < mMaxDelta && delta2 < mMaxDelta) {
            int box1=int(delta1/mResolution);
            int box2=int(delta2/mResolution);
            mOut[box1][box2]++;
            //if (mNewGroup1CoordinationGroup == ANGLERDF_NEWGROUPCOORDINATION_BOTH) push_into_new_group(mNewGroup1, atom1, atom2, atom3);
            //else if (mNewGroup1CoordinationGroup == 2) push_into_new_group(mNewGroup1, atom1, atom2);
            //else if (mNewGroup1CoordinationGroup == 3) push_into_new_group(mNewGroup1, atom1, atom3);
            //if (mNewGroup2CoordinationGroup == ANGLERDF_NEWGROUPCOORDINATION_BOTH) push_into_new_group(mNewGroup2, atom2, atom1, atom3);
            //else if (mNewGroup2CoordinationGroup == 1) push_into_new_group(mNewGroup2, atom2, atom1);
            //else if (mNewGroup2CoordinationGroup == 3) push_into_new_group(mNewGroup2, atom2, atom3);
            //if (mNewGroup3CoordinationGroup == ANGLERDF_NEWGROUPCOORDINATION_BOTH) push_into_new_group(mNewGroup3, atom3, atom1, atom2);
            //else if (mNewGroup3CoordinationGroup == 1) push_into_new_group(mNewGroup3, atom3, atom1);
            //else if (mNewGroup3CoordinationGroup == 2) push_into_new_group(mNewGroup3, atom3, atom2);
        }
    }
    //if (mNewGroup1 != nullptr) {
    //    mNewGroup1->update(nullptr, nullptr);
    //}
    //if (mNewGroup2 != nullptr) {
    //    mNewGroup2->update(nullptr, nullptr);
    //}
    //if (mNewGroup3 != nullptr) {
    //    mNewGroup3->update(nullptr, nullptr);
    //}
}


void ActionDoubleShortDelta::internalPrint(const vector<Atom>* allatoms, const Timestep *t) {
    mpOut->close();
    mpOut->open(mvOutFilenames[0]);
    for (int i=0; i<mOut.size(); i++) {
        for (int j=0; j<mOut[i].size(); j++) {
            (*mpOut) << i*mResolution << " " << j*mResolution << " " << mOut[i][j] << endl;
        }
    }
    mpOut->close();
}
void ActionDoubleShortDelta::internalFinalAction(const vector<Atom>* allatoms, const Timestep *t) {
    internalPrint(allatoms, t);
}


