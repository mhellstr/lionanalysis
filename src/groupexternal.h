#ifndef GROUPEXTERNAL_H
#define GROUPEXTERNAL_H
#include "groupcoordination.h"

/***
 * When using GroupExternal, don't forget to call update(nullptr, nullptr) to update its internal size counters!
 */
class GroupExternal : public GroupCoordination {
public:
    GroupExternal(string s="");
protected:
    void internalUpdate(const vector<Atom> *, const Timestep *); 
};

#endif
