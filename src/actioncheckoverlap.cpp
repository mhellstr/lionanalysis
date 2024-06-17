#include "actioncheckoverlap.h"
#include "groupsum.h"

ActionCheckOverlap::ActionCheckOverlap()
: Action(ActionType::CheckOverlap, 1, 1000, 1, 1)
{ }

void ActionCheckOverlap::addGroup(Group *g) {
    Action::addGroup(g);
    if (dynamic_cast<GroupSum*>(g) == nullptr) {
        throw string("Error! CheckOverlap requires group of group type GroupIntersection");
    }
}

void ActionCheckOverlap::internalAction(const vector<Atom> *allatoms, const Timestep *timestep) {
    const vector<Group*> *vgroups=mpGroup->getConstituentGroups();
    if (mCount == 1) {
        (*mpOut) << "tstep id type";
        for (int i=0; i<vgroups->size(); i++) {
            (*mpOut) << " " << (*vgroups)[i]->getId();
        }
        (*mpOut) << endl;
    }
    for (int i=0; i<mpGroup->size(); i++) { //for each atom in the sumgroup
        stringstream ss("");
        int hits=0;
        ss << timestep->iteration << " " << mpGroup->atom(i)->id << " " << mpGroup->atom(i)->type;
        for (int j=0; j<vgroups->size(); j++) { //for each constituent group
            if ( (*vgroups)[j]->isMember(mpGroup->atom(i)) ) {
                ss << " 1";
                hits++;
            }
            else {
                ss << " 0";
            }
        }
        if (hits >=2) {
            (*mpOut) << ss.str() << endl;
            if (mPrintWarning) {
                cout << "WARNING: CheckOverlap on SumGroup " << mpGroup->getId() << ": " << ss.str() << endl;
            }
        }
    }
}
