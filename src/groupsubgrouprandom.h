#ifndef GROUPSUBGROUPRANDOM_H
#define GROUPSUBGROUPRANDOM_H

#include "group.h"
#include "groupsubgroup.h"

class GroupSubGroupRandom : public GroupSubGroup {
public:
    GroupSubGroupRandom(string s);
    void setNumElements(int a) { mNumElements = a; }
protected:
    void internalUpdate(const vector<Atom> *, const Timestep *) override;
    int mNumElements;
};



#endif
