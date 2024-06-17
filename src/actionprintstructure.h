#ifndef ACTIONPRINTXYZ_H
#define ACTIONPRINTXYZ_H

#include "action.h"
#include "groupcoordination.h"

enum class StructureFormat { mattixyz, xmolout, ymol, lammpsrdx, mattixyzdelta, runnerwithforceweights, runner };

class ActionPrintStructure : public Action {
public:
    ActionPrintStructure();
    ~ActionPrintStructure();
    void setPrintFormat(StructureFormat a) {mPrintFormat = a;}
    void setPrintEmptyFrames(bool a) {mPrintEmptyFrames = a;}
    void setCenterOnId(bool a) { mCenterOnId = a; }
    void setCenterOn(int a) { mCenterOn = a; }
    void addSpecial(const GroupAndString &gas) { mvSpecial.push_back(gas); }
    void addSpecial2(const GroupAndString &gas) { mvSpecial2.push_back(gas); }
    void setPrintIndividualCoordinationEnvironments(bool a) {mPrintIndividualCoordinationEnvironments=a;}
    void setPrintPolyhedra(bool a) {mPrintPolyhedra=a;}
    void setScaleBy(double a) {mScaleBy=a;}
    void setScaleByMax(double a) {mScaleByMax=a;}
    void setCTScaleBy(double a) {mCTScaleBy=a;}
    void setCTMinDist(double a) {mCTMinDist=a;}
    void setCenterAtOrigin(bool a) {mCenterAtOrigin = a;}
    void setMinSize(int a) {mMinSize=a;}
    void setMaxActualPrint(int a) { mMaxActualPrints = a;}
    void setCenterOnXYZ(bool a, float x, float y, float z) { mCenterOnXYZ = a; mCenterOnX = x; mCenterOnY = y; mCenterOnZ = z; }
    void setFirstCenterOnStaticXYZ(bool a) {mFirstCenterOnStaticXYZ=a;}
    void setPrintUnwrapped(bool a) {mPrintUnwrapped=a;}
    //void setScale
    //void setPrintAndScaleCoordinationTables(
    //void setFillCoordinationTableGroup(Group *g);
    //void setRadius(double radius, Group* radiusgroup) { mRadius=radius; mRadiusGroup=radiusgroup; };
protected:
    virtual void internalAction(const vector<Atom> *, const Timestep *) override;
    virtual void internalFinalAction(const vector<Atom> *, const Timestep *) override;
    virtual void updateDescription();
    void actualPrint(const vector<Atom> *allatoms, const Timestep *t, const Group *g);
    StructureFormat mPrintFormat;
    bool mPrintEmptyFrames;
    int mCenterOn; 
    bool mCenterOnId;
    vector<GroupAndString> mvSpecial;
    vector<GroupAndString> mvSpecial2; //this contains either the custom charge (for writing lammps) OR the custom force weights (for writing runnerwithforceweights)
    string getSpecial(const Atom*, const vector<GroupAndString>&);
    bool mPrintIndividualCoordinationEnvironments;
    bool mPrintPolyhedra;
    bool mCenterAtOrigin;
    double mScaleBy, mScaleByMax;
    double mCTScaleBy;
    double mCTMinDist;
    int mMinSize;
    int mActualPrints, mMaxActualPrints;
    bool mCenterOnXYZ;
    float mCenterOnX, mCenterOnY, mCenterOnZ;
    bool mFirstCenterOnStaticXYZ;
    bool mPrintUnwrapped;
    //GroupCoordination *mFillCoordinationTableGroup;
    //double mRadius;
    //Group *mRadiusGroup;
private:
};

#endif
