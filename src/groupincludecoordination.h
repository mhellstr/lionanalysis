#ifndef GROUPINCLUDECOORDINATION_H
#define GROUPINCLUDECOORDINATION_H

#include "groupcoordination.h"

class GroupIncludeCoordination : public GroupCoordination {
public:
    GroupIncludeCoordination(string s);
protected:
    void internalUpdate(const vector<Atom> *, const Timestep *) override;
};



#endif
