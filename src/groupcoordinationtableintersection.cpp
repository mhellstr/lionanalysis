#include "group.h"
#include "groupcoordinationtableintersection.h"

//GroupCoordinationTableIntersection::~GroupCoordinationTableIntersection()
//{ }

GroupCoordinationTableIntersection::GroupCoordinationTableIntersection(string s)
: GroupCoordination(GROUP_TYPE::COORDINATIONTABLEINTERSECTION, s)
{
    //mConstituentGroups.resize(1, nullptr);
}


void GroupCoordinationTableIntersection::internalUpdate(const vector<Atom> *allatoms, const Timestep *t)
{
    clear();
    //cout << "GroupCoordinationTableIntersection::internalUpdate : id = " << getId() << " parentGroup = " << mParentGroup->getId() << endl;
    if (mConstituentGroups.empty()) throw string("GroupCoordinationTableIntersection::internalUpdate, but mConstituentGroups is empty!");
    const GroupCoordination *mParentGroup = dynamic_cast<GroupCoordination*>(mConstituentGroups[0]);
    if (mParentGroup == nullptr) throw string ("ERROR! GroupCoordinationTableIntersection::internalUpdate, but mParentGroup is not of type GroupCoordination!");
    for (int i=0; i < mParentGroup->size(); i++) { 
        int cn = mParentGroup->getCoordinationNumber(i);
        int ingroupid=addAtom(mParentGroup->atom(i));
        //cout << "atom " << i << " in parent group " << mParentGroup->getId() << " has cn = " << cn << endl;
        for (int j=0; j<cn; j++) {
            const Atom *parentcoordinationtableatom = mParentGroup->getCoordinationTable(i)->atom(j);
            bool success=true;
            for (int k=1; k<mConstituentGroups.size(); k++) { //mConstituentGroups[0] is the parent group
                if (!mConstituentGroups[k]->isMember(parentcoordinationtableatom)) {
                    //cout << " atom " << parentcoordinationtableatom->id << " of type " << parentcoordinationtableatom->type << " was NOT a member of group " << mConstituentGroups[k]->getId() << endl;
                    success=false;
                    break;
                }
            }
            if (success) {
                addToCoordinationTable(ingroupid, parentcoordinationtableatom, true);
            }
        }
        int newcn=getCoordinationNumber(ingroupid);
        //if (newcn < mMinCoord || newcn > mMaxCoord) {
        //    delAtom(mParentGroup->atom(i));
        //}
    }
}
