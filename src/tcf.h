#ifndef TCF_H
#define TCF_H

#include <vector>
#include <utility>
#include <exception>
#include <string>
#include "group.h"
#include "groupcoordination.h"

enum class TCF_t { VACF, MSD, MRT, ResidenceTime, ResidenceTimeContinuous, ResidenceTimeCoordinationShell, ResidenceTimeSSP, ResidenceTimeSwitchBond, ResidenceTimeSSPCoordinationShell };

TCF_t stringToTCF_t(const string& s);

class TCF {
public:
    virtual ~TCF();
    virtual void  correlateWithLatest(int externalmaxdt=-1);
    void  clear();
    void  add(const float); //to mInData1
    void  add(const float, const float); //to mInData1, mInData2
    void  add2(const float); //to mInData2
    void addCare(bool a) { mDoICare.push_back(a); }
    int   getMaxTime() const { return mMaxTime; }
    float getOutDataValue(int) const;
    int   getOutDataCount(int) const;
    int   inData1Size() const { return mInData1.size(); }
    void  reserve(int a) {mInData1.reserve(a);}
    int   capacity() const {return mInData1.capacity();}
    void  setMaxTime(int a);
    void  setNumItemsPerTimeStep(int a) {mNumItemsPerTimeStep=a; }
    void setTidyOld(int a) {mTidyOld = a;}
    void setCorrelateForward(bool a) {mCorrelateForward=a;}
    //void setFuncAllowedIndices(std::function<bool(float,float)> a) { mFuncAllowedIndices = a; }
protected:
    TCF(TCF_t pType); //called from derived classes 
    std::vector<float> mInData1;
    std::vector<float> mInData2;
    std::vector< std::pair<double, long long int> > mOutData;
    vector<bool> mDoICare;
    //virtual float internalCorrelateWithLatest(unsigned int, unsigned int) const = 0;
    virtual std::pair<double, long long int> internalCorrelateWithLatest(unsigned int, unsigned int) const = 0;
    int mNumItemsPerTimeStep;
    TCF_t mType;
    int mMaxTime; //this is ONE LESS than the maximum number of timesteps back to correlate. Thus, mMaxTime == 1000 means correlate timesteps 0, 1, ... , 999
    int getMaxDT() const;
    int getMinDT() const;
    virtual bool allowedIndices(int, int); 
    bool mCorrelateForward;
    int mTidyOld;
    virtual void tidyInData();
    //std::function<bool(float,float)> mFuncAllowedIndices;
};



/*
class AdvancedTimeCorrelationFunction {
public:
    AdvancedTimeCorrelationFunction(TCF_t, Group*, int);
    void print(ofstream *) const;
    void update(const Timestep *timestep=nullptr);
    void setNumItemsPerTimeStep(int a) { mNumItemsPerTimeStep=a; }
    void setContinuousMembers(bool a=true) { mContinuousMembers=a; }
    void setCalculateVelocities( bool a=true) {mCalculateVelocities=a;}
    void setAverage(bool a=true) {mAverage=a;}
    void setIntelligent(bool a=true) { mIntelligent = a; }
    void setDimX(bool a=true) { mDimX = a; }
    void setDimY(bool a=true) { mDimY = a; }
    void setDimZ(bool a=true) { mDimZ = a; }
    void setTimeUnit(float x) { mTimeUnit =x; }
    void setSubGroup1(GroupCoordination *g) { mSubGroup1 = g; }
    void setReserveMultiple(int a);
private:
    std::vector<int> mEntered;
    std::vector<TCF> mTCF;
    std::vector< floatintpairvec > mOutData;
    const TCF_t mType;
    Group *mGroup1;
    GroupCoordination *mSubGroup1; //for residence times, mGroup1 == all, and mSubGroup1 is the group whose lifetime is counted
    vector<float> mLastPositions;
    int mMaxTime;
    bool mInitialized;
    float mTimeUnit; //in picosec
    bool mDimX, mDimY, mDimZ;
    bool mIntelligent;
    bool mCalculateVelocities;
    //void calculateVelocities(float, float, float, float, float, float, vector<float>&);
    //void normalize();
    bool mContinuousMembers;
    void update_with_continuous_members(const Timestep *);
    //bool mSaveLastPositions;
    int mNumItemsPerTimeStep;
    bool mAverage;
    void updateEnteredVector(const Timestep *);
    void push_back_in_data(const Timestep *);
    int mReserveMultiple; //number of "complete" data sets to reserve for the mTCF
};
*/


#endif
