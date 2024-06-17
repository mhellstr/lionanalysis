#include "groupregion.h"
#include <functional>

GroupRegion::GroupRegion(string s) 
: Group(GROUP_TYPE::REGION, s), mMinX(-100000), mMaxX(100000), mMinY(-1000000), mMaxY(1000000), mMinZ(-1000000), mMaxZ(1000000),mMinDelta(-1000), mMaxDelta(10000), mNormalCoordinateBounds(false)
{
}

bool GroupRegion::normalCoordinateBounds(float val, float min, float max, float cell) {
    return (val >=min && val < max);
}

bool GroupRegion::negativeLowerBound(float val, float min, float max, float cell) {
    //min is < 0 here, so return true if the coordinate is near "either edge" of the cell
    return (val >= cell+min || val < max);
}


void GroupRegion::internalUpdate(const vector<Atom> *allatoms, const Timestep *t)
{
    clear();
    // the mMinX > -t->cellx check is there since the defualt value for mMinX is -100000.... if it's been manually specified in the 
    // configuration file then it is sure to be a number > -t->cellx
    std::function<bool(float,float,float,float)> x_check = (mMinX < 0 && mMinX > -t->cellx ? &GroupRegion::negativeLowerBound : &GroupRegion::normalCoordinateBounds);
    std::function<bool(float,float,float,float)> y_check = (mMinY < 0 && mMinY > -t->celly ? &GroupRegion::negativeLowerBound : &GroupRegion::normalCoordinateBounds);
    std::function<bool(float,float,float,float)> z_check = (mMinZ < 0 && mMinZ > -t->cellz ? &GroupRegion::negativeLowerBound : &GroupRegion::normalCoordinateBounds);
    if (mNormalCoordinateBounds) {
        x_check = &GroupRegion::normalCoordinateBounds;
        y_check = &GroupRegion::normalCoordinateBounds;
        z_check = &GroupRegion::normalCoordinateBounds;
    }
    //std::function<bool(float,float,float,float)> delta_check = &groupRegion::normalCoordinateBounds;
    //(mMinX < 0 ? &GroupRegion::negativeLowerBound : &GroupRegion::normalCoordinateBounds);
    for (int i=1; i<(*allatoms).size(); i++) {
        const float x=(*allatoms)[i].x;
        const float y=(*allatoms)[i].y;
        const float z=(*allatoms)[i].z;
        const float delta=(*allatoms)[i].delta;
        if (x_check(x,mMinX,mMaxX,t->cellx) && y_check(y,mMinY,mMaxY,t->celly) && z_check(z,mMinZ,mMaxZ,t->cellz) && (delta >= mMinDelta && delta<mMaxDelta)) {
        //if (x >= mMinX && x < mMaxX && y >= mMinY && y < mMaxY && z >= mMinZ && z < mMaxZ) {
            addAtom(&((*allatoms)[i]));
        }
    }
}

float GroupRegion::getVolume(const Timestep *t) const {
    float dx=mMaxX-mMinX, dy=mMaxY-mMinY, dz=mMaxZ-mMinZ;
    if (t != nullptr) {
        if (t->cellx < dx) dx=t->cellx;
        if (t->celly < dy) dy=t->celly;
        if (t->cellz < dz) dz=t->cellz;
    }
    return dx*dy*dz;
}
