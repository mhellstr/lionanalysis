#ifndef ACTIONTTCF_H
#define ACTIONTTCF_H

#include "action.h"
#include "actionttcfparent.h"
#include "ttcf.h"
#include "tcfcondition.h"
#include "timestep.h"

template <typename Tin, typename Tout>
class ActionTTCF : public ActionTTCFParent {
public:
    virtual ~ActionTTCF();
    //void setContinuousMembers(bool a=true);
    //void setReserveMultiple(int a) { mReserveMultiple=a; }
    //void setParentGroup(Group *g) {mParentGroup=g;}
protected:
    ActionTTCF(ActionType, int, int, int, int, TCF_t);
    virtual void internalAction(const vector<Atom>*, const Timestep*) override;
    virtual void internalFinalAction(const vector<Atom>*, const Timestep*) override;
    virtual void internalPrint(const vector<Atom>*, const Timestep*) override;
    virtual void internalInitialize(const vector<Atom>*, const Timestep*) override;
    virtual void internalMoreAtomsThanExpected(int) { };
    virtual void internalAtomLeftEnteredVector(int) { };
    virtual void internalAddToTCF(int, const Atom*) { };
    virtual void internalAddToTCF(int i, const Atom* a, const Timestep* t) { internalAddToTCF(i, a); };
    virtual void internalComplicatedAddToTCF(const Timestep*) { };
    virtual void manipulateIndividualTCF(const vector<pair<Tout, long long int>>&, int, bool) { };
    std::vector<TTCF<Tin,Tout>*> mTCF;
    //std::vector<int> mEntered;
    //std::vector<Tout, long long int> mOutData;
    std::vector<vector<std::pair<float,int>>> mOutData;
    TCF_t mTCFType;
    //int mReserveMultiple; //number of "complete" data sets to reserve for the mTCF
    //void update_with_continuous_members(const Timestep *);
    //void updateEnteredVector(const Timestep *);
    //virtual void push_back_in_data(const Timestep *);
    virtual TTCF<Tin,Tout>* newTCF() const = 0;
    //Group *mParentGroup;
    void correlateAll(int);
    void writeRestart(const Timestep*);
    void createTCFs(int n);
};


#endif

