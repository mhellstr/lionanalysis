#ifndef GROUPREGION_H
#define GROUPREGION_H

#include "group.h"
#include "timestep.h"

class GroupRegion : public Group {
public:
    GroupRegion(string s="");
    void setMinX(float a) {mMinX=a;}
    void setMinY(float a) {mMinY=a;}
    void setMinZ(float a) {mMinZ=a;}
    void setMaxX(float a) {mMaxX=a;}
    void setMaxY(float a) {mMaxY=a;}
    void setMaxZ(float a) {mMaxZ=a;}
    void setMinDelta(float a) {mMinDelta=a;}
    void setMaxDelta(float a) {mMaxDelta=a;}
    void setNormalCoordinateBounds(bool a) {mNormalCoordinateBounds=a;}
    float getVolume(const Timestep*) const;
protected:
    void internalUpdate(const vector<Atom> *, const Timestep *) override;
    float mMinX;
    float mMinY;
    float mMinZ;
    float mMaxX;
    float mMaxY;
    float mMaxZ;
    float mMinDelta, mMaxDelta;
    static bool normalCoordinateBounds(float, float, float, float) ;
    static bool negativeLowerBound(float, float, float ,float) ;
    bool mNormalCoordinateBounds;
};



#endif
