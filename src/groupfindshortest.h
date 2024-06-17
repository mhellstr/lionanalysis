#ifndef GROUPFINDSHORTEST_H
#define GROUPFINDSHORTEST_H

#include "groupcoordination.h"

class GroupFindShortest : public GroupCoordination {
public:
    GroupFindShortest(string s);
protected:
    void internalUpdate(const vector<Atom> *, const Timestep *) override;
};



#endif
