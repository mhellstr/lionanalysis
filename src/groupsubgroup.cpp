#include "groupsubgroup.h"

GroupSubGroup::GroupSubGroup(string s) 
: GroupList(GROUP_TYPE::SUBGROUP, s)
{
}

GroupSubGroup::GroupSubGroup(GROUP_TYPE gt, string s) 
: GroupList(gt, s)
{
}


void GroupSubGroup::internalUpdate(const vector<Atom> *allatoms, const Timestep *t) {
    clear();
    for (int id : mvAtomIds) {
        if (id < mParentGroup->size()) {
          addAtom(mParentGroup->atom(id));
        }
    }
}

void GroupSubGroup::internalAddConstituentGroup(Group *g) {
    mParentGroup = mConstituentGroups[0];
}
