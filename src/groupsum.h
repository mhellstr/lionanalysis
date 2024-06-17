#ifndef GROUPSUM_H
#define GROUPSUM_H

#include "groupcoordination.h"

class GroupSum : public GroupCoordination {
public:
    GroupSum(string s);
protected:
    void internalUpdate(const vector<Atom> *, const Timestep *) override;
};



#endif
