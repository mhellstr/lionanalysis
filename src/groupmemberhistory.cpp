#include "groupmemberhistory.h"
#include <algorithm>

GroupMemberHistory::GroupMemberHistory(string s)
: Group(GROUP_TYPE::MEMBERHISTORY, s), mMaxHistory(10), mMinMemberTime(0), mMaxMemberTime(10000000)
{ }

GroupMemberHistory::GroupMemberHistory(GROUP_TYPE gt, string s)
: Group(gt, s), mMaxHistory(10), mMinMemberTime(0), mMaxMemberTime(10000000)
{ }

void GroupMemberHistory::internalUpdate(const vector<Atom> *allatoms, const Timestep *t) {
    clear();
    if (mUpdateCount == 0) {
        mvIsMember.resize(mParentGroup->size());
        for (int i=0; i<mvIsMember.size(); i++) {
            mvIsMember[i].resize(mMaxHistory, false);
        }
    }
    int index = mUpdateCount % mMaxHistory;
    for (int i=0; i<mParentGroup->size(); i++) {
        mvIsMember[i][index] = mMemberOfGroup->isMember(mParentGroup->atom(i));
        if (mDrawFromGroup->isMember(mParentGroup->atom(i))) {
            int count = std::count(mvIsMember[i].begin(), mvIsMember[i].end(), true);
            if (count >= mMinMemberTime && count <= mMaxMemberTime) {
                addAtom(mParentGroup->atom(i));
            }
        }
    }
}
