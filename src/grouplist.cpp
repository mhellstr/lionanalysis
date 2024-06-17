#include "grouplist.h"

GroupList::GroupList(string s)
: Group(GROUP_TYPE::LIST, s)
{ }

GroupList::GroupList(GROUP_TYPE gt, string s)
: Group(gt, s)
{ }

void GroupList::addAtomId(int a) {
    mvAtomIds.push_back(a);
}

void GroupList::internalUpdate(const vector<Atom> *allatoms, const Timestep *t) {
    clear();
    for (int i=0; i<mvAtomIds.size(); i++) {
        for (int j=0; j<(*allatoms).size(); j++) {
            if ( (*allatoms)[j].id == mvAtomIds[i] ) {
                addAtom(&((*allatoms)[j]));
            }
        }
    }
}
