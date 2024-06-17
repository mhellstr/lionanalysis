#ifndef GROUPDIFF_H
#define GROUPDIFF_H

#include "groupcoordination.h"

class GroupDiff : public GroupCoordination {
public:
    GroupDiff(string s);
protected:
    void internalUpdate(const vector<Atom> *, const Timestep *) override;
};



#endif
