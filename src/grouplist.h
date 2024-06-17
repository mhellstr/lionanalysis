#ifndef GROUPLIST_H
#define GROUPLIST_H
#include "group.h"

class GroupList : public Group {
public:
    GroupList(string s="");
    GroupList(GROUP_TYPE, string s);
    void addAtomId(int);
    virtual void internalUpdate(const vector<Atom> *, const Timestep *) override;
protected:
    vector<int> mvAtomIds;
};

#endif
