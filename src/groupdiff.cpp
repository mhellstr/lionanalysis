#include "groupdiff.h"

GroupDiff::GroupDiff(string s) 
: GroupCoordination(GROUP_TYPE::DIFFGROUP, s)
{
}


void GroupDiff::internalUpdate(const vector<Atom> *allatoms, const Timestep *t)
{
    if (mConstituentGroups.size() == 0) return;
    clear();
    for (int i=0; i<mConstituentGroups[0]->size(); i++) { //these are atoms in the parent group
        const Atom* a=mConstituentGroups[0]->atom(i);
        bool foundmatch=false;
        for (int j=1; j<mConstituentGroups.size(); j++) { //remaining groups
            const Group *g = mConstituentGroups[j];
            if (g->isMember(a)) {
                foundmatch=true;
                break;
            }
        }
        if (!foundmatch) {
            int ingroupid=addAtom(a);
            if (dynamic_cast<GroupCoordination*>(mConstituentGroups[0]) != nullptr) {
                copyCoordination(ingroupid, static_cast<GroupCoordination*>(mConstituentGroups[0]));
            }
        }
    }
}
