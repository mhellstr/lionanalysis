#ifndef ACTION_H
#define ACTION_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include "group.h"
#include "timestep.h"
#include "atom.h"
//#include "rdf.h"
//#include "anglerdf.h"
//#include "dihedralrdf.h"
//#include "msd.h"
//#include "mbd.h"
//#include "vac.h"
//#include "tcf.h"
#include "cobject.h"
#include "actiontype.h"
#include <memory>
#include "tcfcondition.h"


using namespace std;


class Action {
public:
    Action();
    virtual ~Action();
    void action(const vector<Atom> *allatoms, const Timestep *timestep); //called at each timestep from main loop
    void finalAction(const vector<Atom> *, const Timestep *); //called when all timesteps have been read
    virtual void addGroup(Group *g);
    void addObject(CObject *o);
    void addOut(ofstream *o);
    void addOut(const string&);
    //void addOut(const string&);
    string getDescription() const { return mDescription; }
    int getNumGroups() const { return mvpGroups.size(); }
    int getNumObjects() const { return mvpObjects.size(); }
    ActionType getType() const { return mType; }
    void setDescription(const string s) { mDescription = s; }
    void setEvery(int a) { mEvery = a; }
    void setPrintEvery(double a, bool realtime=false);
    void setFlushEvery(double a, bool realtime=false);
    void setStartFrom(int a) { mMinCount = a; }
    void setTimeUnit(float a) {mTimeUnit=a;}
    void setTimeUnit(float basictimeunit, float superevery, float every) { mTimeUnit = basictimeunit*superevery*every; }
    //virtual void setRealTime();
    //void setObject(CObject *o) { mpObject = o; }
    virtual bool requiresDistances() const { return true; } 
    virtual const vector<TCFCondition>& conditions() const { return mvTCFConditions; } //only used by inherited ActionTTCF but needs to be defined here since it must be called by the TTCF<> class
    string getId() const { return mId; }
    void setId(const string& a) { mId = a; }
    void setReadRestart(const string &s);
    void setWriteRestart(const string &s);
    void setIfGroupNotEmpty(Group *g) {mIfGroupNotEmpty=g;}
    void setIfGroupEmpty(Group *g) {mIfGroupEmpty=g;}
protected:
    Action(ActionType, int, int, int, int);
    virtual void internalAction(const vector<Atom> *atoms, const Timestep *timestep);
    virtual void internalFinalAction(const vector<Atom> *atoms, const Timestep *timestep) { };
    virtual void internalAtomLeftEnteredVector(int)  { }; //used in ActionTCF
    virtual void internalMoreAtomsThanExpected(int) { }; //used in ActionTCF
    virtual void internalInitialize(const vector<Atom>*, const Timestep*) { };
    virtual void internalPrint(const vector<Atom>*, const Timestep*) { };
    //void setDescription(const string& s) {mDescription=s;}
    virtual void updateDescription();
    void checkNumGroups();
    vector<Group*> mvpGroups; //these groups are used in ActionPrintStructure and ActionPrintProperties.... i.e., mvpObjects is NOT used in those cases
    Group *mpGroup; //alias for mvpGroups[0]
    vector<ofstream*> mvpOuts;
    ofstream *mpWriteRestart;
    ifstream *mpReadRestart;
    string mReadRestartFilename;
    string mWriteRestartFilename;
    vector<string> mvOutFilenames;
    //vector<unique_ptr<ofstream>> mmvpOuts;
    ofstream *mpOut; //alias for mvpOuts[0]
    unique_ptr<ofstream> mmpOut; //alias for mvpOuts[0]
    int mCount;
    int mMinCount; //before the action really starts... should be 2 if velocities are calculated from consecutive frames.... not USED yet. FIXME
    int mEvery;
    int mPrintEvery;
    int mFlushEvery;
    void setActionType(ActionType a) { mType = a; }
    ActionType mType; //what kind of ACTION_TYPE ?
    int mMinGroups; //minimum allowed size of mvpGroups
    int mMaxGroups; //maximum allowed size of mvpGroups
    int mMinOuts;
    int mMaxOuts;
    float mTimeUnit;
    vector<TCFCondition> mvTCFConditions; // not used here, only by ActionTTCFParent
    bool mInitialized; //all constructors set this to false. action() sets it to true
    string mDescription; //human-friendly description of the action. set by the constructor. accessed by getDescription()
    vector<CObject*> mvpObjects;
    CObject *mpObject; //AngleRDF, RDF, MSD, TimeCorrelationFunction, etc.
    bool mDynamicCreationOutputFiles;
    void closeOutputFiles();
    string mId;
    long long mMinTimestepNumber;
    long long mMaxTimestepNumber;
    Group *mIfGroupNotEmpty, *mIfGroupEmpty;
private:
};


#endif
