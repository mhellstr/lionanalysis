#include "actionfindvacuum.h"
#include "utility.h"

ActionFindVacuum::ActionFindVacuum() 
: Action(ActionType::FindVacuum, 1, 1, 1, 1)
{
}

ActionFindVacuum::~ActionFindVacuum() { }


void ActionFindVacuum::updateDescription() {
    checkNumGroups();
    setDescription("FindVacuum inside group " + mvpGroups[0]->getId());
}


void ActionFindVacuum::internalAction(const vector<Atom> *atoms, const Timestep *timestep) {
    float d=0.0;
    float X,Y,Z,mindist;
    help_findVacuum(timestep, 0.0, timestep->cellx, 0.0, timestep->celly, 0.0, timestep->cellz, mResolution, X, Y, Z, mindist);
    help_findVacuum(timestep, X-mResolution, X+mResolution, Y-mResolution, Y+mResolution, Z-mResolution, Z+mResolution, mResolution/10., X, Y, Z, mindist);
    //help_findVacuum(atoms, timestep, X-3*mResolution, X+3*mResolution, Y-3*mResolution, Y+3*mResolution, Z-3*mResolution, Z+3*mResolution, mResolution/10., X, Y, Z, mindist);
    //help_findVacuum(atoms, timestep, X-mResolution/10., X+mResolution/10., Y-mResolution/10., Y+mResolution/10., Z-mResolution/10., Z+mResolution/10., mResolution/50., X, Y, Z, mindist);
    (*mpOut) << timestep->iteration << " " << X << " " << Y << " " << Z << " " << mindist << endl;
}

void ActionFindVacuum::help_findVacuum(const Timestep *timestep, float minx, float maxx, float miny, float maxy, float minz, float maxz, float resolution, float &storex, float &storey, float &storez, float &stored) {
    float mindist=10000.0;
    float X=0.0,Y=0.0,Z=0.0; // these are the coordinates where the distance to any other atom is the greatest
    float d=0.0;
    int xsteps=int( (maxx-minx) / resolution) + 1;
    int ysteps=int( (maxx-minx) / resolution) + 1;
    int zsteps=int( (maxx-minx) / resolution) + 1;
    #pragma omp parallel
    {
        float X_local=0,Y_local=0,Z_local=0,min_local=100000,max_local=0.0;
        float x=0.0, y=0.0, z=0.0;
        #pragma omp for 
        for (int ix=0; ix<xsteps; ix++) { 
            x=minx+ix*resolution;
            for (int iy=0; iy<ysteps; iy++) { 
                y=miny+iy*resolution;
                for (int iz=0; iz<zsteps; iz++) { 
                    z=minz+iz*resolution;
                    min_local=10000;
                    for (int i=0; i<mvpGroups[0]->size(); i++) {
                        d=distance(x,y,z, mvpGroups[0]->atom(i)->x, mvpGroups[0]->atom(i)->y, mvpGroups[0]->atom(i)->z, timestep);
                        if (d<min_local) {
                            min_local=d; //this is the minimum distance within this frame
                        }
                    }
                    if (min_local > max_local) {
                        max_local=min_local; //if the minimum distance within this frame is greater than the previous minimum distances, store this minimum distance instead
                        X_local=x; Y_local=y; Z_local=z;
                    }
                }
            }
        }
        #pragma omp critical
        {
            if (max_local < mindist) {
                mindist = max_local;
                X=X_local; Y=Y_local; Z=Z_local;
            }
        }
    }
    storex=X; storey=Y; storez=Z; stored=mindist;
}

