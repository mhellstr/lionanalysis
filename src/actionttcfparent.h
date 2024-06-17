#ifndef ACTIONTTCFPARENT_H
#define ACTIONTTCFPARENT_H
#include "action.h"
#include "tcfcondition.h"

namespace ActionTTCFOptions {
    enum {
        UseAllTimeOrigins,
        DoNormalize0,
        DoPrintRawNumbers,
        DoPrintHeader,
        DoPrintAllMembers,
        UncareAllWhenUncareRemaining,
        ValuePrecedenceOverUncare,
        ManipulateIndividualTCFs,
        Average,
        Discretize,
        DiscretizeContinuous,
        DiscretizeDiscrete,
        DoPrintVectorOut,
        Hungarian,
        IgnoreConflicts,
        DoPrintIds,
        HungarianSquaredPenalty,
        DoPrintPositions,
        AT_END
    };
};

class ActionTTCFParent : public Action {
public:
    virtual ~ActionTTCFParent() { };
    void setNumItemsPerTimeStep(int a) { mNumItemsPerTimeStep=a; }
    void decNumItemsPerTimeStep() { mNumItemsPerTimeStep--; }
    void incNumItemsPerTimeStep() { mNumItemsPerTimeStep++; }
    void setAverage(bool a=true) {mAverage=a;}
    void setDimX(bool a=true) { mDimX = a; }
    void setDimY(bool a=true) { mDimY = a; }
    void setDimZ(bool a=true) { mDimZ = a; }
    void setMaxTime(float a, bool realtime=false);
    void setTidyOld(int a) {mTidyOld=a;}
    int getTidyOld() const { return mTidyOld; }
    virtual void setRealTime(float);
    const vector<TCFCondition>& conditions() const override { return mvTCFConditions; }
    void addCondition(TCFCondition a, bool realtime=false);
    void setCorrelationFrequency(float a, bool realtime=false);
    void addGroup(Group *g) override;
    void setDoNormalize0(bool a) {mDoNormalize0=a;}
    void setDoPrintRawNumbers(bool a) {mDoPrintRawNumbers=a;}
    void setDoPrintHeader(bool a) {mDoPrintHeader=a;}
    void setDoPrintAllMembers(bool a) {mDoPrintAllMembers=a;}
    void setShellGroup(Group *g) { mpShellGroup = g;}
    void setShellGroup1(Group *g) { mpShellGroup1 = g;}
    void setShellGroup2(Group *g) { mpShellGroup2 = g;}
    void setTCFFrom(ActionTTCFParent *a) { mpTCFFrom = a; }
    void setUseAllTimeOrigins(bool a) {mUseAllTimeOrigins =a ;}
    void setUncareAllWhenUncareRemaining(bool a) {mUncareAllWhenUncareRemaining = a;}
    bool getUncareAllWhenUncareRemaining() const { return mUncareAllWhenUncareRemaining;}
    void setValuePrecedenceOverUncare(bool a) {mValuePrecedenceOverUncare = a;}
    bool getValuePrecedenceOverUncare() const { return mValuePrecedenceOverUncare;}
    void setManipulateIndividualTCFs(bool a) {mManipulateIndividualTCFs=a;}
    virtual void setManipulateIndividualTCFs(bool a, double min, double max, int type) {mManipulateIndividualTCFs=a;}
    void setOption(int option, bool value);
    bool getOption(int option) const;
protected:
    ActionTTCFParent(ActionType, int, int, int, int);
    int mMaxTime;
    int mNumItemsPerTimeStep;
    int mTidyOld;
    int mCorrelationFrequency;
    vector<TCFCondition> mvTCFConditions;
    vector<bool> mvBoolsToSave;
    bool mAverage;
    //bool mContinuousMembers;
    bool mDimX, mDimY, mDimZ;
    Group *mpSubGroup;
    bool mDoNormalize0; //normalize so that myout[0] = 1 (used for VACF)
    bool mDoPrintRawNumbers; //print myout[dt].first and myout[dt].second
    bool mDoPrintHeader;
    bool mDoPrintAllMembers;
    Group *mpShellGroup; //"parent group" for the shell
    Group *mpShellGroup1; //"condition group" for the shell of center 1 (reactants in SSP, the only one in TResidenceTimeCoordionationShell)
    Group *mpShellGroup2; //"condition group" for the shell of center 2 (products in SSP)
    ActionTTCFParent *mpTCFFrom;
    bool mUseAllTimeOrigins;
    bool mUncareAllWhenUncareRemaining;
    bool mValuePrecedenceOverUncare;
    bool mManipulateIndividualTCFs; //for VACFs, whether to Fourier-transform  each individual one and extract the frequency
    bool mSimpleAddToTCF; //whether to simply call internalAddToTCF within the loop over mpGroup in ActionTTCF::internalAction; this should be true for all kinds EXCEPT MSDFollow
    bool mOptions[ActionTTCFOptions::AT_END];
    double mShiftBy; //shift the output by mShiftBy*mTimeUnit
};


#endif
