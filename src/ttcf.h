#ifndef TTCF_H
#define TTCF_H

#include <vector>
#include <utility>
#include <exception>
#include <string>
#include "group.h"
#include "groupcoordination.h"
#include "consequence.h"
#include "tcf.h"
#include "action.h"
#include "tcfcondition.h"
#include "actionttcfparent.h"


template<typename Tin, typename Tout>
class TTCF {
public:
    virtual ~TTCF();
    //virtual void  correlateWithLatest(int externalmaxdt=-1);
    virtual void  clear();
    void  add(const Tin&); //to mInData1
    void  add(const Tin&, const Tin&); //to mInData1, mInData2
    void  add2(const Tin&); //to mInData2
    void addBoolInfo(bool, int);
    void addBoolInfo(bool, int, int);
    //void addCare(bool a) { mDoICare.push_back(a); }
    int   getMaxTime() const { return mMaxTime; }
    Tout getOutDataValue(int a) const { return mOutData[a].first; };
    void setOutDataValue(int a, Tout b) { mOutData[a].first = b; }
    const vector<std::pair<Tout, long long int>>& getTotalOut() { return mOutData; }
    long long int   getOutDataCount(int a) const { return mOutData[a].second; };
    void setOutDataCount(int a, long long int b) { mOutData[a].second = b; }
    int   inData1Size() const { return mInData1.size(); }
    Tin getInData1(int a) const  { return mInData1.at(a); }
    int   inData2Size() const { return mInData2.size(); }
    Tin getInData2(int a) const  { return mInData2.at(a); }
    void  reserve(int a) {mInData1.reserve(a);}
    int   capacity() const {return mInData1.capacity();}
    void  setMaxTime(int a){
        mMaxTime = a;
        mOutData.resize(mMaxTime, std::pair<Tout, long long int>(Tout(0),0));
        //mPrecedingOutData.resize(mMaxTime, std::pair<Tout, long long int>(Tout(0),0));
    };
    void  setNumItemsPerTimeStep(int a) {mNumItemsPerTimeStep=a; }
    //void setTidyOld(int a) {mTidyOld = a;}
    virtual void correlate(int numtimeorigins, int externalmaxdt = -1);
    //void setUncareAllWhenUncareRemaining(bool a) { mUncareAllWhenUncareRemaining = a; }
    //void setCorrelateForward(bool a) {mCorrelateForward=a;}
    //void setFuncAllowedIndices(std::function<bool(float,float)> a) { mFuncAllowedIndices = a; }
protected:
    TTCF(const ActionTTCFParent* a); //called from derived classes, the action is the action
    int getTidyOld() const { return mParentAction->getTidyOld(); }
    bool uncareAllWhenUncareRemaining() const { return mParentAction->getUncareAllWhenUncareRemaining(); }
    bool valuePrecedenceOverUncare() const { return mParentAction->getValuePrecedenceOverUncare(); }
    std::vector<Tin> mInData1;
    std::vector<Tin> mInData2;
    std::vector< std::pair<Tout, long long int> > mOutData;
    //std::vector< std::pair<Tout, long long int> > mPrecedingOutData;
    vector<vector<bool>*> mBoolInfo; //these contain
    vector<vector<int>*> mIntInfo;
    vector<vector<double>*> mDoubleInfo;
    const vector<TCFCondition>& conditions() const { return mParentAction->conditions(); }
    //vector<bool> mDoICare;
    //virtual float internalCorrelateWithLatest(unsigned int, unsigned int) const = 0;
    virtual std::pair<Tout, long long int> internalCorrelateWithLatest(unsigned int, unsigned int) const = 0;
    int mNumItemsPerTimeStep;
    TCF_t mType;
    int mMaxTime; //this is ONE LESS than the maximum number of timesteps back to correlate. Thus, mMaxTime == 1000 means correlate timesteps 0, 1, ... , 999
    int getMaxDT(int oldindex=0) const;
    int getMinDT() const;
    //virtual bool allowedIndices(int, int); 
    //bool mCorrelateForward;
    //int mTidyOld;
    void tidyInData();
    virtual void correlateFromOldIndex(int oldindex);
    bool mUncareWhenZero;
    bool mZeroAllWhenZero;
    bool mUncareWhenUncare;
    std::pair<Tout, long long int> quickCorrelate(int oldindex, int newindex, Consequence &c);
    vector<TCFCondition> mvTCFCondition;
    bool checkIfMember(int, int);
    bool checkIfMemberFor(int, int, int, int, int, bool);
    bool checkTCFCondition(int oldindex, int newindex, Consequence c);
    Consequence doICare(int oldindex, int newindex);
    const ActionTTCFParent *mParentAction;
    bool mJoinedCorrelation;
    //bool mUncareAllWhenUncareRemaining;
    //std::function<bool(float,float)> mFuncAllowedIndices;
};







#endif

