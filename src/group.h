#ifndef GROUP_H
#define GROUP_H

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <utility>
#include "atom.h"
#include "timestep.h"
#include "cobject.h"

#define GROUP_CONSTANT_ALLATOMS "-1"

class Group;
typedef std::pair<Group*, string> GroupAndString;

enum class GROUP_TYPE {
    BONDCRITERION,
    ATOMICNUMBER,
    SUMGROUP,
    DIFFGROUP,
    INTERSECTIONGROUP,
    LIST,
    REGION,
    SUBGROUP,
    CORRESPONDING, //not used! use EXTERNAL instead
    ANGLEBOND, //not used! use an AngleRDF class instead and define groups via that one
    READ,
    EXTERNAL,
    SUBGROUPCN,
    MEMBERHISTORY,
    SUBGROUPRANDOM,
    COORDINATIONTABLEINTERSECTION,
    FINDSHORTEST,
    INVERTCOORDINATION,
    INCLUDECOORDINATION,
    ADDCOORDINATION
};

enum class GROUP_PROPERTY {
    X,
    Y,
    Z,
    Q,
    QAVERAGE,
    COORDINATIONNUMBER,
    ID,
    TYPE,
    DISTANCES,
    NUMATOMS,
    TIMESTEPNUMBER,
    TIMESTEPITERATION,
    TOTALSIZE,
    NAME,
    COORDINATIONTABLE,
    BONDS,
    AVERAGESIZE,
    DELTA,
    BOND0,
    BOND1,
    BOND2,
    BOND3,
    CELL,
    XYZENVIRONMENT,
    COORDINATIONTYPES,
    ATOMTYPES,
    COORDINATIONTABLEX,
    COORDINATIONTABLEY,
    COORDINATIONTABLEZ,
    XU,
    YU,
    ZU,
    VX,
    VY,
    VZ
};

class Group : public CObject {
public:
    Group(GROUP_TYPE g=GROUP_TYPE::EXTERNAL, string id="");
    virtual ~Group();
    virtual int addAtom(const Atom* a);  //checks that a is not already a member, also resizes mCoordinationNumbers and mCoordinationTable
    const Atom* atom(int i) const { return mAtoms[i]; }
    virtual void clear(); //clears mAtoms
    string getId() const { return mId; } //this is the id of the group itself
    int getMemberId(const Atom*) const; //look for the Atom* in mAtoms, return INTERNAL position in mAtoms array
    int getMemberId(int) const;  //look for the atom with id in mAtoms, return INTERNAL position in mAtoms array
    bool isInitialized() { return mInitialized; }
    bool isMember(const Atom*) const;
    bool isMember(int id) const;
    bool isStatic() { return mStatic; }
    void setStatic(bool a=true) { mStatic = a; };
    unsigned int size() const { return mAtoms.size(); };
    bool empty() const { return mAtoms.empty(); }
    float getMass() const;
    void streamProperties(stringstream &, const vector<GROUP_PROPERTY> &, int &, string separator="", bool general_bool_value=false) const;
    virtual void streamDistances(stringstream &, int&, bool) const;
    void update(const vector<Atom> *allatoms, const Timestep *t);
    void addConstituentGroup(Group *); 
    void setId(const string& s) { mId=s; }
    virtual void setAtomsFromExternalSource(const Group*);
    int getTotalSize() const { return mTotalSize; } //summed over all timesteps
    float getAverageSize() const { if (mUpdateCount==0) return 0; return mTotalSize*1.0/mUpdateCount; }
    bool getRequiresDistances() const { return mRequiresDistances; }
    void setRequiresDistances(bool a) { mRequiresDistances=a;}
    const vector<Group*> *getConstituentGroups() const { return &mConstituentGroups; }
    void setMaxTotalSize(int a) {mMaxTotalSize = a;}
    int getMaxTotalSize() const { return mMaxTotalSize; }
protected:
    Group(GROUP_TYPE);
    vector<const Atom*> mAtoms; //the money group
    string mId;
    bool mInitialized; //set to true after first action has been performed
    bool mStatic; //static == true means group members are not updated between frames
    int mUpdateCount;
    int mTotalSize;
    int mMaxTotalSize;
    GROUP_TYPE mType;
    vector<Group*> mConstituentGroups; 
    virtual void derivedStreamProperties(stringstream &, GROUP_PROPERTY, int, int&) const { }; //additional streaming properties for certain kinds of groups, like coordination groups
    virtual void derivedStreamPerGroupProperties(stringstream &, GROUP_PROPERTY, int&) const { };
    virtual void internalUpdate(const vector<Atom> *, const Timestep *);
    virtual void internalAddConstituentGroup(Group *) { };
    void incUpdateCount() { mUpdateCount++; }
    bool mRequiresDistances;
};

#endif

