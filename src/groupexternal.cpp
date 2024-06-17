#include "groupexternal.h"
GroupExternal::GroupExternal(string s)
: GroupCoordination(GROUP_TYPE::EXTERNAL, s)
{
}
void GroupExternal::internalUpdate(const vector<Atom> *, const Timestep *) {
}
