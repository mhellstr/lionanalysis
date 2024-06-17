#ifndef GROUPADDCOORDINATION_H
#define GROUPADDCOORDINATION_H

#include "groupcoordination.h"

class GroupAddCoordination : public GroupCoordination {
public:
    GroupAddCoordination(string s);
protected:
    void internalUpdate(const vector<Atom> *, const Timestep *) override;
};



#endif
