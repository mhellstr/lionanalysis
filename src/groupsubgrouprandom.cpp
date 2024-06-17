#include "groupsubgrouprandom.h"
#include <cstdlib>
#include <vector>

GroupSubGroupRandom::GroupSubGroupRandom(string s) 
: GroupSubGroup(GROUP_TYPE::SUBGROUPRANDOM, s), mNumElements(1)
{
}


void GroupSubGroupRandom::internalUpdate(const vector<Atom> *allatoms, const Timestep *t) {
    clear();
    if (mNumElements >= mParentGroup->size()) {
        for (int i=0; i<mParentGroup->size(); i++) {
            addAtom(mParentGroup->atom(i));
        }
    }
    else {
        vector<int> v(mNumElements, -1);
        for (int i=0; i<mNumElements; i++) {
            bool success=false;
            int numtries=0;
            while (!success) {
                numtries++;
                if (numtries > 100) break;
                int a = rand() % mParentGroup->size();
                bool found=false;
                for (int j=0; j<i; j++) {
                    found = (a == v[j]);
                    if (found) break;
                }
                success = !found;
                if (success) {
                    v[i]=a;
                    addAtom(mParentGroup->atom(a));
                }
            }
        }
    }
}

