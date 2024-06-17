#include "groupatomicnumber.h"
GroupAtomicNumber::GroupAtomicNumber(string id) 
: Group(GROUP_TYPE::ATOMICNUMBER, id), mTargetAtomType(""), mTargetAtomMass(1.0), mDoSetMasses(false)
{
    mRequiresDistances = false;
}

void GroupAtomicNumber::internalUpdate(const vector<Atom> *allatoms, const Timestep *t) {
    //cout << "atomicnumber internalupdate groupid " << getId() <<  " allatoms.size() " << allatoms->size() << endl;
    clear();
    for (int i=1; i<allatoms->size(); i++) { //there is a dummy atom in the beginning of the allatoms vector.
        if ((*allatoms)[i].type == mTargetAtomType || mTargetAtomType == GROUP_CONSTANT_ALLATOMS) {
            addAtom(&((*allatoms)[i]));
            if (mDoSetMasses) {
                vector<Atom>* nonconst = const_cast<vector<Atom>*>(allatoms);
                (*nonconst)[i].mass = mTargetAtomMass;
                //cout << " set mass of atom " << (*allatoms)[i].id << " to " << mTargetAtomMass << endl;
            }
        }
    }
}
