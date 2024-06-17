#ifndef GROUPINTERSECTION_H
#define GROUPINTERSECTION_H

#include "groupcoordination.h"

class GroupIntersection : public GroupCoordination {
public:
    GroupIntersection(string s);
    void update();
protected:
    void internalUpdate(const vector<Atom> *, const Timestep *) override;
    bool mCopyCoordinationTable;
};



#endif
