#include "groupaddcoordination.h"

GroupAddCoordination::GroupAddCoordination(string s) 
: GroupCoordination(GROUP_TYPE::ADDCOORDINATION, s)
{
}


void GroupAddCoordination::internalUpdate(const vector<Atom> *allatoms, const Timestep *t)
{
    clear();
    if (mConstituentGroups.size() != 2) throw string("Error! GroupAddCoordination NEEDS 2 constituentgroups!");
    const GroupCoordination *g=dynamic_cast<GroupCoordination*>(mConstituentGroups[0]);
    if (g == nullptr) throw string("addcoordination needs gurp of type GroupCoordination!");
    const GroupCoordination *h=dynamic_cast<GroupCoordination*>(mConstituentGroups[1]);
    if (h == nullptr) throw string("addcoordination needs gurp of type GroupCoordination!");
    for (int i=0; i<g->size(); i++) {
        int originalingroupid=addAtom(g->atom(i));
        const Group *coordinationtable = g->getCoordinationTable(i);
        for (int j=0; j<coordinationtable->size(); j++) { //if an atom in the first group's coordination table is a member of the second group, add the corresponding atom's coordination tbale in the SECOND group to the coordination tbale of the ORIGINAL atom in the FIRST group
            addToCoordinationTable(originalingroupid, coordinationtable->atom(j), true);
            int newingroupid=h->getMemberId(coordinationtable->atom(j));
            if (newingroupid >= 0) {
                for (int k=0; k<h->getCoordinationTable(newingroupid)->size(); k++) {
                    addToCoordinationTable(originalingroupid, h->getCoordinationTable(newingroupid)->atom(k), true);
                }
            }
        }
    }
}
