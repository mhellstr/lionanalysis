#ifndef GROUPMEMBERHISTORY_H
#define GROUPMEMBERHISTORY_H
#include "group.h"

class GroupMemberHistory : public Group {
public:
    GroupMemberHistory(string s="");
    GroupMemberHistory(GROUP_TYPE, string s);
    virtual void internalUpdate(const vector<Atom> *, const Timestep *) override;
    void setMaxHistory(int a) { mMaxHistory = a; }
    void setMaxMemberTime(int a) { mMaxMemberTime = a; }
    void setMinMemberTime(int a) { mMinMemberTime = a; }
    void setParentGroup(Group *g) { mParentGroup = g; }
    void setMemberOfGroup(Group *g) {mMemberOfGroup = g;}
    void setDrawFromGroup(Group *g) {mDrawFromGroup = g;}
protected:
    Group *mParentGroup; //all atoms that might potentially become members of mDrawFromGroup
    Group *mDrawFromGroup; //atoms must be member of this group
    Group *mMemberOfGroup; //check the history of the membership in this group
    int mMaxHistory;
    int mMaxMemberTime;
    int mMinMemberTime;
    vector<vector<bool>> mvIsMember;
};

#endif
