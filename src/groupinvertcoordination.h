#ifndef GROUPINVERTCOORDINATION_H
#define GROUPINVERTCOORDINATION_H

#include "groupcoordination.h"

class GroupInvertCoordination : public GroupCoordination {
public:
    GroupInvertCoordination(string s);
protected:
    void internalUpdate(const vector<Atom> *, const Timestep *) override;
};



#endif
