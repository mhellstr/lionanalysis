#include "groupbond.h"

GroupBond::GroupBond(string s)
: GroupCoordination(GROUP_TYPE::BONDCRITERION, s), mBondMinDist(0.0), mBondMaxDist(2.0), mpFromGroup(nullptr), mpToGroup(nullptr), mpCorrespondingGroup(nullptr), mInheritDelta(false), mIncludeToGroup(false)
{
}

void GroupBond::internalUpdate(const vector<Atom> *allatoms, const Timestep *t) {
    clear();
    //cout << "beginning of GroupBond::internalUpdate: size() = " << size() << endl;
    if (mpCorrespondingGroup != nullptr) mpCorrespondingGroup->clear();
    if (mpFromGroup == nullptr || mpToGroup == nullptr) { return; }
    mCoordinationTable.clear();
    int res=0;
    vector<const Atom*> tolist;
    for (int i=0; i<mpFromGroup->size(); i++) {
        res=0;
        tolist.clear();
        const Atom* fromAtom = mpFromGroup->atom(i);
        for (int j=0; j<mpToGroup->size(); j++) {
            const Atom* toAtom = mpToGroup->atom(j);
            if (fromAtom->id == toAtom->id) continue;
            float dist=fromAtom->bonds[toAtom->id];
            if (dist > mBondMinDist && dist < mBondMaxDist) {
                res++;
                //cout << "timestep: " << t->iteration << " pushing back atom " << toAtom->id << " because bond to atom " << fromAtom->id << " is only " << dist << endl;
                tolist.push_back(toAtom);
            }
        }
        if ( (res >= mMinCoord && res <=mMaxCoord) ) {
            int ingroupid = addAtom(fromAtom);
            if (mInheritDelta && ! tolist.empty()) {
                const_cast<Atom*>(fromAtom)->delta = tolist[0]->delta; //this changes the ATOM itself, which might affect ANY OTHER GROUP or ACTION THAT USES the delta property
                //this also only makes sense if all atoms in the group tolist have the same delta (or, the more probable scenario, that tolist only contains one element)
            }
            //mAtoms.push_back(mpFromGroup->mAtoms[i]);
            //mCoordinationNumbers.push_back(res);
            setCoordinationNumber(ingroupid, res);
            for (int k=0; k<tolist.size(); k++) {
                addToCoordinationTable(ingroupid, tolist[k]);
                //cout << "adding to ingroupid " << ingroupid << " atom " << tolist[k]->id << endl;
                if (mpCorrespondingGroup != nullptr) {
                    int corringroupid=mpCorrespondingGroup->addAtom(tolist[k]);
                    mpCorrespondingGroup->addToCoordinationTable(corringroupid, fromAtom, true);
                }
                if (mIncludeToGroup) {
                    addAtom(tolist[k]);
                }
            } //for tolist
        } //if coordination number
    } //for fromGroup
    //cout << "end of GroupBond::internalUpdate for group " << getId() << " size: " << size() << endl;
    if (mpCorrespondingGroup != nullptr) mpCorrespondingGroup->update(allatoms, t); // this doesn't modify the atoms nor coordination tables but just updates the corresponding group's TotalSize, so that averagesize.dat etc. are given correctly
}//internalUpdate
