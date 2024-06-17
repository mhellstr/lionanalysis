#ifndef GROUPCOORDINATIONTABLEINTERSECTION_H
#define GROUPCOORDINATIONTABLEINTERSECTION_H

#include "groupcoordination.h"

class GroupCoordinationTableIntersection : public GroupCoordination {
public:
    GroupCoordinationTableIntersection(string s);
protected:
    //GroupCoordinationTableIntersection(GROUP_TYPE, string);
    void internalUpdate(const vector<Atom> *, const Timestep *) override;
};



#endif
