#include "actionsumvelocities.h"
//this is more or less equivalent to CalculateMomentum

ActionSumVelocities::ActionSumVelocities() :Action(ActionType::SumVelocities, 0, 1000, 1, 1) { }

void ActionSumVelocities::internalAction(const vector<Atom> *allatoms, const Timestep *t) {
    double sumx=0,sumy=0,sumz=0;
    for (int i=0; i<mpGroup->size(); i++) {
        sumx+=mpGroup->atom(i)->mass*mpGroup->atom(i)->vx;
        sumy+=mpGroup->atom(i)->mass*mpGroup->atom(i)->vy;
        sumz+=mpGroup->atom(i)->mass*mpGroup->atom(i)->vz;
    }
    (*mpOut) << sumx << " " << sumy << " " << sumz << "\n";
}

