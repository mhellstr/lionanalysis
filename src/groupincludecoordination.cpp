#include "groupincludecoordination.h"

GroupIncludeCoordination::GroupIncludeCoordination(string s) 
: GroupCoordination(GROUP_TYPE::INCLUDECOORDINATION, s)
{
}


void GroupIncludeCoordination::internalUpdate(const vector<Atom> *allatoms, const Timestep *t)
{
    clear();
    if (mConstituentGroups.size() != 1) throw string("Error! GroupIncludeCoordination NEEDS 2 constituentgroups!");
    const GroupCoordination *g=dynamic_cast<GroupCoordination*>(mConstituentGroups[0]);
    if (g == nullptr) throw string("includecoordination needs gurp of type GroupCoordination!");
    for (int i=0; i<g->size(); i++) {
        const Group *coordinationtable = g->getCoordinationTable(i);
        addAtom(g->atom(i));
        for (int j=0; j<coordinationtable->size(); j++) {
            int ingroupid=addAtom(coordinationtable->atom(j));
            //addToCoordinationTable(ingroupid, g->atom(i), true);
        }
    }
}
