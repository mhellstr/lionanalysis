#include "actionmovecenter.h"
#include "utility.h"
#include <cstdlib>
#include <ctime>


ActionMoveCenter::ActionMoveCenter()
: Action(ActionType::MoveCenter, 1, 1, 0, 0), mMinVal(0), mProbabilityToMove(0.1), mProbabilityToChangeFrame(1.0), mMaxFrac(0.5)
{
}

void ActionMoveCenter::internalInitialize(const vector<Atom> *atoms, const Timestep *timestep) {
    //srand(time(nullptr));
}


void ActionMoveCenter::internalAction(const vector<Atom> *atoms, const Timestep *timestep) {
    float rr = rand()*1.0/RAND_MAX;
    if (rr >= mProbabilityToChangeFrame) return;
    GroupCoordination *g=dynamic_cast<GroupCoordination*>(mpGroup);
    for (int i=0; i<g->size(); i++) {
        const Group *ct = g->getCoordinationTable(i);
        if (ct->size() == 2) { //the first atom in the ct should be the COVALENTLY BOUND atom, and the second the HYDROGENBONDED atom
            float r=rand()*1.0/RAND_MAX;
            if (r >= mProbabilityToMove) continue;
            const Atom *near=ct->atom(0);
            const Atom *far=ct->atom(1);
            const Atom *center = g->atom(i);
            if (near->bonds[center->id] < mMinVal) continue;
            float maxval=near->bonds[far->id]*mMaxFrac;
            float drange=maxval-mMinVal;
            float newval=mMinVal + drange*rand()*1.0/RAND_MAX;
            float oldval=near->bonds[center->id];
            float scalefactor=newval/oldval;
            //float scalefactor=1;
            float newx=center->x, newy=center->y, newz=center->z;
            TranslateAtomCoordinatesToVicinity(newx, newy, newz, near->x, near->y, near->z, timestep);
            newx= near->x + (newx-near->x)*scalefactor;
            newy= near->y + (newy-near->y)*scalefactor;
            newz= near->z + (newz-near->z)*scalefactor;
            if (distance( newx, newy, newz, far->x, far->y, far->z, timestep)  < mMinVal) continue;
            vector<Atom>* ccatoms = const_cast<vector<Atom>*>(atoms);
            //cout << "OLD POSITION: " << center->x << " " << center->y << " " << center->z << endl;
            (*ccatoms)[center->id].x = newx;
            (*ccatoms)[center->id].y = newy;
            (*ccatoms)[center->id].z = newz;
                    int s=center->id;
                        for (int j=1; j<ccatoms->size(); j++) {
                            if (j==s) continue;
                            float dist=distance((*ccatoms)[s].x,(*ccatoms)[s].y,(*ccatoms)[s].z,(*ccatoms)[j].x,(*ccatoms)[j].y,(*ccatoms)[j].z, timestep);
                            (*ccatoms)[s].bonds[j] = dist;
                            (*ccatoms)[j].bonds[s] = dist;
                        }
            //cout << "NEW POSITION: " << center->x << " " << center->y << " " << center->z << endl;
            //ccatoms[center->id].x = 
        }
    }
}
