#include "groupsubgroupcn.h"
#include "groupcoordination.h"

GroupSubGroupCN::~GroupSubGroupCN()
{ }

GroupSubGroupCN::GroupSubGroupCN(string s)
: GroupCoordination(GROUP_TYPE::SUBGROUPCN, s)
{
    //mConstituentGroups.resize(1, nullptr);
}


void GroupSubGroupCN::internalUpdate(const vector<Atom> *allatoms, const Timestep *t)
{
    clear();
    //cout << "GroupSubGroupCN::internalUpdate : id = " << getId() << " parentGroup = " << mParentGroup->getId() << endl;
    for (int i=0; i < mParentGroup->size(); i++) { 
        int cn = mParentGroup->getCoordinationNumber(i);
        if (cn >= mMinCoord && cn <= mMaxCoord)
        {
            int ingroupid = addAtom(mParentGroup->atom(i));
            mCoordinationNumbers[ingroupid] = cn;
            mCoordinationTable[ingroupid] = *mParentGroup->getCoordinationTable(i); //this copies the vector and is pretty slow, but mCoordinationTable is a vector and not a pointer so there is no simple way around it (other than to to mCoordinationTable = new vector, but I don't want to do that at this point because of complexity)
        }
    }
}
