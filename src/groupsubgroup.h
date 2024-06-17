#ifndef GROUPSUBGROUP_H
#define GROUPSUBGROUP_H

#include "group.h"
#include "grouplist.h"

class GroupSubGroup : public GroupList {
public:
    GroupSubGroup(string s);
    void setParentGroup(Group *g) { mConstituentGroups.clear(); addConstituentGroup(g); }
protected:
    GroupSubGroup(GROUP_TYPE, string);
    void internalUpdate(const vector<Atom> *, const Timestep *) override;
    void internalAddConstituentGroup(Group *) override;
    Group *mParentGroup;
};



#endif
