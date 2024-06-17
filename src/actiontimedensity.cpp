#include "actiontimedensity.h"
#include "groupregion.h"
#include "timestep.h"
#include "constants.h"

ActionTimeDensity::ActionTimeDensity()
: Action(ActionType::TimeDensity,2,2,1,1), mTimeUnit(1), mpRegionGroup(nullptr), mIntersectionGroup("dummy")
{ }


void ActionTimeDensity::internalAction(const vector<Atom>* allatoms, const Timestep *timestep) {
    if (mCount == 1) {
        mIntersectionGroup.addConstituentGroup(mpGroup);
        mIntersectionGroup.addConstituentGroup(mpRegionGroup);
    }
    mIntersectionGroup.update();
    GroupRegion *region = dynamic_cast<GroupRegion*>(mpRegionGroup);
    if (region == nullptr) throw string("regiongroup is not GroupRegion in ActionTimeDensity::internalAction!");
    float vol=region->getVolume(timestep); //A^3
    float mass=mIntersectionGroup.getMass();
    float rawnumber=1.0*mIntersectionGroup.size();
    float numberdensity=rawnumber*1.0/vol;
    //mvNumberDensity.push_back(numberdensity);
    //
    double massdensity=mass*ATOMICMASSUNIT*1e3/(vol*1e-24); //1e-24 converts from A^3 to cm^3, 1e3 converts from kg to g
    //mvMassDensity.push_back(massdensity);
    // cout << "vol = " << vol << " mass = " << mass << " rawnumber = " << rawnumber <<  " size1 = " << mpGroup->size() << " size2 = " << mpRegionGroup->size() << " size3= " << mIntersectionGroup.size() << endl; //" cellx = " << timestep->cellx << " celly = " << timestep->celly << " cellz = " << timestep->cellz << endl;
    if (mCount == 0) {
        (*mpOut) << "#t numberdensity massdensity \n";
    }
    (*mpOut) << mCount*mTimeUnit << " " << numberdensity << " " << massdensity << "\n";

}
void ActionTimeDensity::internalFinalAction(const vector<Atom>* allatoms, const Timestep *timestep) {
    //for (int i=0; i<mvNumberDensity.size(); i++) {
        //(*mpOut) << i*mTimeUnit << " " << mvNumberDensity[i] << " " << mvMassDensity[i] << "\n";
    //}
}


void ActionTimeDensity::addGroup(Group *g) {
    mvpGroups.push_back(g);
    mpGroup=mvpGroups[0];
    if (mvpGroups.size() > 1) {
        mpRegionGroup=mvpGroups[1];
    }
}
