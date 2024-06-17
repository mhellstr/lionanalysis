#include "groupinvertcoordination.h"

GroupInvertCoordination::GroupInvertCoordination(string s) 
: GroupCoordination(GROUP_TYPE::INVERTCOORDINATION, s)
{
}


void GroupInvertCoordination::internalUpdate(const vector<Atom> *allatoms, const Timestep *t)
{
    clear();
    if (mConstituentGroups.size() != 1) throw string("Error! GroupInvertCoordination NEEDS 2 constituentgroups!");
    const GroupCoordination *g=dynamic_cast<GroupCoordination*>(mConstituentGroups[0]);
    if (g == nullptr) throw string("invertcoordination needs gurp of type GroupCoordination!");
    for (int i=0; i<g->size(); i++) {
        const Group *coordinationtable = g->getCoordinationTable(i);
        for (int j=0; j<coordinationtable->size(); j++) {
            int ingroupid=addAtom(coordinationtable->atom(j));
            addToCoordinationTable(ingroupid, g->atom(i), true);
        }
    }
}
