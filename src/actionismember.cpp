#include "actionismember.h"

ActionIsMember::ActionIsMember() :Action() {
}

void ActionIsMember::internal_action(const vector<Atom> *allatoms, const Timestep *timestep) {
    for (const auto &x : (*allatoms)) {
        if (mpGroup->isMember(&x)) {
            (*mpOut) << 1 << " ";
        }
        else {
            (*mpOut) << 0 << " ";
        }
    }
}
