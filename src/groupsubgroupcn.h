#ifndef GROUPSUBGROUPCN_H
#define GROUPSUBGROUPCN_H

#include "groupcoordination.h"

class GroupSubGroupCN : public GroupCoordination {
public:
    GroupSubGroupCN(string s="");
    ~GroupSubGroupCN();
    void setParentGroup(GroupCoordination *g) { mParentGroup = g; }
protected:
    virtual void internalUpdate(const vector<Atom> *, const Timestep *) override;
    GroupCoordination *mParentGroup;
};

#endif
