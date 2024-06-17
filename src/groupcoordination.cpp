#include "groupcoordination.h"
#include "formatter.h"
#include "groupexternal.h"
#include <sstream>
#include <stdexcept>
#include <list>

/*
GroupCoordination::GroupCoordination() 
: mSaveCoordinationTable(true)
{ }
GroupCoordination::GroupCoordination(GROUP_TYPE g) 
: Group(g), mSaveCoordinationTable(true)
{ }
*/
//GroupCoordination::GroupCoordination(GROUP_TYPE g, string id) 
//: Group(g, id), mSaveCoordinationTable(true), mMinCoord(1), mMaxCoord(1000)
//{ }
GroupCoordination::GroupCoordination(GROUP_TYPE g, string id, int mincoord, int maxcoord) 
: Group(g, id), mSaveCoordinationTable(true), mMinCoord(mincoord), mMaxCoord(maxcoord)
{ }


GroupCoordination::~GroupCoordination()
{ }

void GroupCoordination::derivedStreamPerGroupProperties(stringstream &ss, GROUP_PROPERTY gp, int& numentries) const {
    switch (gp) {
        case GROUP_PROPERTY::COORDINATIONTYPES:
            vector<string> vAtomTypes;
            vector<int> vCountTypes;
            for (int i=0; i<mAtoms.size(); i++) {
                for (int k=0; k<mCoordinationTable[i].size(); k++) {
                    bool foundtype=false;
                    for (int j=0; j<vAtomTypes.size(); j++) {
                        if (mCoordinationTable[i].atom(k)->type == vAtomTypes[j]) {
                            foundtype=true;
                            vCountTypes[j]++;
                            break;
                        }
                    }
                    if (!foundtype) {
                        vAtomTypes.push_back(mCoordinationTable[i].atom(k)->type);
                        vCountTypes.push_back(1);
                    }
                }
            }
            for (int i=0; i<vAtomTypes.size(); i++) {
                //ss << vCountTypes[i] << " " << vAtomTypes[i] << " "; numentries+=2;
                ss << vAtomTypes[i] << " "; ++numentries;
            }
            break;
    }
}
void GroupCoordination::derivedStreamProperties(stringstream &ss, GROUP_PROPERTY gp, int index, int& numentries) const {
//this function is called from Group::streamProperties(), the index is guaranteed to be < mCoordinationNumbers.size()
    vector<GROUP_PROPERTY> dummyvec(1, GROUP_PROPERTY::ID);
    switch (gp) {
        case GROUP_PROPERTY::BONDS:
            for (int i=0; i<mCoordinationTable[index].size(); i++) {
                ss << atom(index)->bonds[mCoordinationTable[index].atom(i)->id] << " ";
            }
            break;
        case GROUP_PROPERTY::BOND0: ss << getShortestBond(index, 0) << " " ; ++numentries; break;
        case GROUP_PROPERTY::BOND1: ss << getShortestBond(index, 1) << " " ; ++numentries; break;
        case GROUP_PROPERTY::BOND2: ss << getShortestBond(index, 2) << " " ; ++numentries; break;
        case GROUP_PROPERTY::BOND3: ss << getShortestBond(index, 3) << " " ; ++numentries; break;
        case GROUP_PROPERTY::COORDINATIONNUMBER: 
            ss << mCoordinationNumbers[index] << " ";
            ++numentries;
            break;
        case GROUP_PROPERTY::COORDINATIONTABLE:
            //vector<GROUP_PROPERTY> dummyvec(1, GROUP_PROPERTY::ID);
            mCoordinationTable[index].streamProperties(ss, dummyvec, numentries, "", false); //mCoordinationTable will have the same size as mAtoms, so this is safe.... or should be...
            break;
        case GROUP_PROPERTY::COORDINATIONTABLEX:
            //vector<GROUP_PROPERTY> dummyvec(1, GROUP_PROPERTY::X);
            dummyvec[0] = GROUP_PROPERTY::X;
            mCoordinationTable[index].streamProperties(ss, dummyvec, numentries, "", false); //mCoordinationTable will have the same size as mAtoms, so this is safe.... or should be...
            break;
        case GROUP_PROPERTY::COORDINATIONTABLEY:
            //vector<GROUP_PROPERTY> dummyvec(1, GROUP_PROPERTY::Y);
            dummyvec[0] = GROUP_PROPERTY::Y;
            mCoordinationTable[index].streamProperties(ss, dummyvec, numentries, "", false); //mCoordinationTable will have the same size as mAtoms, so this is safe.... or should be...
            break;
        case GROUP_PROPERTY::COORDINATIONTABLEZ:
            //vector<GROUP_PROPERTY> dummyvec(1, GROUP_PROPERTY::Z);
            dummyvec[0] = GROUP_PROPERTY::Z;
            mCoordinationTable[index].streamProperties(ss, dummyvec, numentries, "", false); //mCoordinationTable will have the same size as mAtoms, so this is safe.... or should be...
            break;
    }
}

void GroupCoordination::incCoordinationNumber(int i) {
    if (i < mCoordinationNumbers.size()) mCoordinationNumbers[i]++;
    else throw std::out_of_range(Formatter() << " Error in group " << mId << " : incCoordinationNumber(" << i << ") but mAtoms.size()==" << mAtoms.size());
}
void GroupCoordination::setCoordinationNumber(int i, int value) {
    if (i < mCoordinationNumbers.size()) mCoordinationNumbers[i] = value;
    else throw std::out_of_range(Formatter() << " Error in group " << mId << " : incCoordinationNumber(" << i << ") but mAtoms.size()==" << mAtoms.size());
}


void GroupCoordination::addToCoordinationTable(int index, const Atom* a, bool autosetcoordinationnumber) {
    if (index < mCoordinationTable.size()) {
        mCoordinationTable[index].addAtom(a); //no risk of duplicates since addAtom checks for duplicates
        if (autosetcoordinationnumber) {
            mCoordinationNumbers[index] = mCoordinationTable[index].size();
        }
    }
    else throw std::out_of_range(Formatter() << " Error in group " << mId << " : addToCoordinationTable(" << index << ", atomid=" << a->id << ") but mAtoms.size() == " << mAtoms.size());
}


int GroupCoordination::addAtom(const Atom *a) { //resizes the mAtoms, mCoordinationTable, and mCoordinationNumbers arrays
   //cout << "this is GroupCoordionation::addAtom()" << endl;
   if (isMember(a)) {
        //cout << "WARNING: in group " << mId << " atom " << a->id << " is already a member... this should never happen!" << endl;
        //it's actually completely legitimate for this to happen, especially if this function is called from the AngleRDF class
        //cout << a->id << " was already a member" << endl;
        return getMemberId(a);
   }
   //cout << " pushing back " << a->id << endl;
   mAtoms.push_back(a);
   //mCoordinationTable.push_back(Group(GROUP_TYPE::EXTERNAL,""));
   mCoordinationTable.push_back(GroupExternal());
   mCoordinationNumbers.push_back(0);
   if (mAtoms.size() != mCoordinationTable.size() || mAtoms.size() != mCoordinationNumbers.size()) {
       throw std::runtime_error(Formatter() << "Error in group " << mId << ".addAtom(atomid==" << a->id << ") : inconsistent sizes : mAtoms.size() == " << mAtoms.size() << " mCoordinationTable.size() == " << mCoordinationTable.size() << " mCoordinationNumbers.size() == " << mCoordinationNumbers.size());
   }
   //cout << " returning size: " << mAtoms.size()-1 << endl;
   return mAtoms.size()-1;
}

void GroupCoordination::delAtom(const Atom* a)
{
    int ingroupid = getMemberId(a);
    if (ingroupid >=0) {
        mAtoms.erase(mAtoms.begin()+ingroupid);
        mCoordinationTable.erase(mCoordinationTable.begin()+ingroupid);
        mCoordinationNumbers.erase(mCoordinationNumbers.begin()+ingroupid);
    }
}


void GroupCoordination::clear() {
        mAtoms.clear();
        mCoordinationNumbers.clear();
        mCoordinationTable.clear();
}


void GroupCoordination::internalUpdate(const vector<Atom> *allatoms, const Timestep *t) { }




void GroupCoordination::setAtomsFromExternalSource(const Group *g) {
    //clear();
    //mAtoms = g->mAtoms;
    Group::setAtomsFromExternalSource(g); //TODO: VERIFY THAT THIS WORKS!
    mCoordinationNumbers.resize(mAtoms.size(),0);
    mCoordinationTable.resize(mAtoms.size(), Group());
}

int GroupCoordination::getCoordinationNumber(int i) const {
    if (i < mCoordinationNumbers.size()) return mCoordinationNumbers[i];
    else return -1;
}


bool GroupCoordination::isMemberOfCoordinationShell(int internalindex, const Atom* a) const {
    return mCoordinationTable[internalindex].isMember(a);
}

// copy coordination for a single atom in the present group from GroupCordinationg *g
void GroupCoordination::copyCoordination(int ingroupid, const GroupCoordination *g) {
    int inothergroupid = g->getMemberId(mAtoms[ingroupid]);
    if (inothergroupid >=0) {
        const Group* targetcoordinationtable = g->getCoordinationTable(inothergroupid);
        mCoordinationTable[ingroupid].clear();
        for (int i=0; i<targetcoordinationtable->size(); i++) {
            addToCoordinationTable(ingroupid, targetcoordinationtable->atom(i));
        }
        setCoordinationNumber(ingroupid, g->getCoordinationNumber(inothergroupid));
    }
}

void GroupCoordination::addCoordinationFromOtherGroup(int ingroupid, const GroupCoordination *g, int inothergroupid) { //inothergroupid default -1
    if (inothergroupid < 0) {
        inothergroupid = g->getMemberId(mAtoms[ingroupid]);
    }
    if (inothergroupid >= 0) {
        const Group* targetcoordinationtable = g->getCoordinationTable(inothergroupid);
        for (int i=0; i<targetcoordinationtable->size(); i++) {
            addToCoordinationTable(ingroupid, targetcoordinationtable->atom(i));
        }
        setCoordinationNumber(ingroupid, mCoordinationTable[ingroupid].size());
    }
}


double GroupCoordination::getAverageBond(int i) const { //i is internalindex
    double data=0;
    int ctsize=getCoordinationTable(i)->size();
    if (ctsize == 0) return 0;
    for (int j=0; j<ctsize; j++) {
        const Atom *a1=atom(i);
        const Atom *a2=getCoordinationTable(i)->atom(j);
        data+=a1->bonds[a2->id];
    }
    data = data*1.0/ctsize;
    return data;
}

double GroupCoordination::getAverageBond() const {
    //cout << ctsize << endl;
    if (size() == 0) return 0;
    double data=0;
    for (int i=0; i<size(); i++) {
        data+=getAverageBond(i);
    }
    data = data* 1.0/size();
    return data;
}

double GroupCoordination::getShortestBond(int internalindex, int sortindex) const { //internalindex gives the atom, and sortindex=0 means shortest bond, sortindex=1 means next shortest bond ,etc.
    std::list<double> bonds;
    const Group* coordinationtable=getCoordinationTable(internalindex);
    for (int i=0; i<coordinationtable->size(); i++) {
        bonds.push_back(atom(internalindex)->bonds[coordinationtable->atom(i)->id]);
    }
    bonds.sort();
    if (sortindex >= bonds.size()) return 0;
    int count=-1;
    for (auto iter=bonds.begin(); iter != bonds.end(); ++iter) {
        count++;
        if (count == sortindex) return *iter;
    }
    return 0;
}
