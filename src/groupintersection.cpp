#include "groupintersection.h"

GroupIntersection::GroupIntersection(string s) 
: GroupCoordination(GROUP_TYPE::INTERSECTIONGROUP, s), mCopyCoordinationTable(true)
{
}

void GroupIntersection::update() {
    Group::update(nullptr, nullptr);
}

void GroupIntersection::internalUpdate(const vector<Atom> *allatoms, const Timestep *t)
{
    clear();
    if (mConstituentGroups.size() == 0) return;
    for (int i=0; i<mConstituentGroups[0]->size(); i++) {
        const Atom* a=mConstituentGroups[0]->atom(i);
        bool inallgroups=true;
        for (int j=1; j<mConstituentGroups.size(); j++) {
            if ( ! mConstituentGroups[j]->isMember(a) ) {
                inallgroups=false;
                break;
            }
        }
        if (inallgroups) {
            int ingroupid=addAtom(a);
            if (dynamic_cast<GroupCoordination*>(mConstituentGroups[0]) != nullptr) {
                copyCoordination(ingroupid, static_cast<GroupCoordination*>(mConstituentGroups[0]));
            }
        }
    }
}
