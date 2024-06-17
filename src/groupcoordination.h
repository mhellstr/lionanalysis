#ifndef CGROUPCOORDINATION_H
#define CGROUPCOORDINATION_H
#include "group.h"
class GroupCoordination;

class GroupCoordination : public Group {
public:
    GroupCoordination();
    ~GroupCoordination();
    //GroupCoordination(GROUP_TYPE);
    //GroupCoordination(GROUP_TYPE, string);
    GroupCoordination(GROUP_TYPE g, string id, int mincoord=1, int maxcoord=1000); 
    bool isMemberOfCoordinationShell(int internalindex, const Atom*) const;
    void addToCoordinationTable(int, const Atom*, bool autosetcoordinationnumber=false); //internal group member id, Atom*
    const Group* getCoordinationTable(int internalindex) const { return &mCoordinationTable[internalindex]; } //i is the INTERNAL position corresponding to the mAtoms array
    void incCoordinationNumber(int); //parameters is the internal group member id. internal call from update(), external call from class AngleRDF
    void setCoordinationNumber(int, int);
    int getCoordinationNumber(int) const;
    void setSaveCoordinationTable(bool a) {mSaveCoordinationTable=a;}
    void clear();
    void setAtomsFromExternalSource(const Group *) override;
    int addAtom(const Atom*) override;
    void delAtom(const Atom*);
    void setMinCoord(int a) { mMinCoord = a; }
    void setMaxCoord(int a) { mMaxCoord = a; }
    void copyCoordination(int ingroupid, const GroupCoordination *g);
    void addCoordinationFromOtherGroup(int ingroupid, const GroupCoordination *g, int inothergroupid=-1);
    double getAverageBond(int) const;
    double getAverageBond() const;
    double getShortestBond(int internalindex, int sortindex) const;
protected:
    bool mSaveCoordinationTable; //it doesn't really cost anything to save the coordination tables so you may as well do it
    vector<int> mCoordinationNumbers; //BONDCRITERION, and EXTERNAL (updated from AngleRDF)
    vector<Group> mCoordinationTable; //for GROUP_TYPE::BONDCRITERION, and EXTERNAL (updated from AngleRDF) contains atoms that are coordinated to the atoms in mAtoms. mAtoms.size() == mCoordinationTable.size(), if mSaveCoordinationTable is true.
    virtual void internalUpdate(const vector<Atom> *, const Timestep *) override;
    void derivedStreamProperties(stringstream &, GROUP_PROPERTY, int, int&) const override;
    void derivedStreamPerGroupProperties(stringstream &ss, GROUP_PROPERTY gp, int& numentries) const override;
    int mMinCoord;
    int mMaxCoord;
}; 
#endif
