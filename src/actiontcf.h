#ifndef ACTIONTCF_H
#define ACTIONTCF_H

#include "action.h"
#include "utility.h"
#include "tcf.h"

class ActionTCF : public Action {
public:
    ~ActionTCF();
    void setNumItemsPerTimeStep(int a) { mNumItemsPerTimeStep=a; }
    void decNumItemsPerTimeStep() { mNumItemsPerTimeStep--; }
    void incNumItemsPerTimeStep() { mNumItemsPerTimeStep++; }
    void setContinuousMembers(bool a=true) { mContinuousMembers=a; }
    void setAverage(bool a=true) {mAverage=a;}
    void setDimX(bool a=true) { mDimX = a; }
    void setDimY(bool a=true) { mDimY = a; }
    void setDimZ(bool a=true) { mDimZ = a; }
    void setReserveMultiple(int a) { mReserveMultiple=a; }
    void setMaxTime(float a, bool realtime=false);
    //void setParentGroup(Group *g) {mParentGroup=g;}
    void setTidyOld(int a) {mTidyOld=a;}
    virtual void setRealTime(float);
protected:
    ActionTCF(ActionType, int, int, int, int, TCF_t);
    virtual void internalAction(const vector<Atom>*, const Timestep*) override;
    virtual void internalFinalAction(const vector<Atom>*, const Timestep*) override;
    virtual void internalMoreAtomsThanExpected(int) { };
    virtual void internalAtomLeftEnteredVector(int) { };
    virtual void internalAddToTCF(int, const Atom*) { };
    std::vector<TCF*> mTCF;
    std::vector<int> mEntered;
    std::vector< floatintpairvec > mOutData;
    TCF_t mTCFType;
    int mMaxTime;
    int mNumItemsPerTimeStep;
    int mReserveMultiple; //number of "complete" data sets to reserve for the mTCF
    bool mAverage;
    bool mContinuousMembers;
    bool mDimX, mDimY, mDimZ;
    void update_with_continuous_members(const Timestep *);
    void updateEnteredVector(const Timestep *);
    virtual void push_back_in_data(const Timestep *);
    virtual TCF* newTCF() const = 0;
    int mTidyOld;
    //Group *mParentGroup;
    Group *mpSubGroup;
};


#endif

