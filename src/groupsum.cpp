#include "groupsum.h"

GroupSum::GroupSum(string s) 
: GroupCoordination(GROUP_TYPE::SUMGROUP, s)
{
}


void GroupSum::internalUpdate(const vector<Atom> *allatoms, const Timestep *t)
{
    clear();
    for (int i=0; i<mConstituentGroups.size(); i++) {
        GroupCoordination* groupcoordinationpointer=dynamic_cast<GroupCoordination*>(mConstituentGroups[i]);
        for (int j=0; j<mConstituentGroups[i]->size(); j++) {
            //cout << "adding atom " << mConstituentGroups[i]->atom(j)->id << " to group " << getId() << " it was part of group " << mConstituentGroups[i]->getId() << endl;
            int ingroupid=addAtom(mConstituentGroups[i]->atom(j));
            if (groupcoordinationpointer != nullptr) {
                addCoordinationFromOtherGroup(ingroupid, groupcoordinationpointer, j);
            }
        }
    }
}
