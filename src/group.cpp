#include "group.h"
#include "utility.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include "formatter.h"
using namespace std;

//Group::Group() { }
Group::Group(GROUP_TYPE g, string id) :mType(g), mId(id), mInitialized(false), mStatic(false), mUpdateCount(0), mTotalSize(0), mRequiresDistances(true), mMaxTotalSize(-1) { }
Group::~Group() { }

//bool Group::setStatic(bool a) {
//    //cout << "a is " << a << endl;
//    //mStatic = a;
//}
void Group::streamProperties(stringstream &ss, const vector<GROUP_PROPERTY> &vec, int &numentries, string separator, bool general_bool_value) const {
//general_bool_value is only used for printproperties distances on a sum group
//if it is set to false, only distances between atoms in different consitutent groups will be printed
//default value for general_bool_value is true
    bool printdistances=false;
    //bool printednumatoms=false;
    //bool printedtimestepnumber=false;
    //bool printedtimestepiteration=false;
    // first check for timestepnumber, timestepiteration, or numatoms (these are only printed once at the beginning of the line in the order they come on the configuration line)
    for (int j=0; j<vec.size(); j++) {
        vector<string> vAtomTypes; //these are zeroed for every new property that is read.
        vector<int> vCountTypes;
        switch (vec[j]) {
            case GROUP_PROPERTY::NAME: ss << getId() << " "; break;
            case GROUP_PROPERTY::NUMATOMS: ss << size() << " "; ++numentries; break; 
            case GROUP_PROPERTY::TOTALSIZE: ss << mTotalSize << " "; ++numentries; break; 
            case GROUP_PROPERTY::AVERAGESIZE: ss << getAverageSize() << " "; ++numentries; break;
            //case GROUP_PROPERTY::QAVERAGE:
                //float qsum=0;
                //for (const auto &k : mAtoms) { qsum+=k->q; }
                //ss << qsum/size() << " "; ++numentries; break;
            //case GROUP_PROPERTY::TIMESTEPNUMBER: ss << timestep->number << " "; break;
            //case GROUP_PROPERTY::TIMESTEPITERATION: ss << timestep->iteration << " "; break;
            case GROUP_PROPERTY::DISTANCES: printdistances=true; break;
            case GROUP_PROPERTY::ATOMTYPES:
                for (int i=0; i<mAtoms.size(); i++) {
                    bool foundtype=false;
                    for (int j=0; j<vAtomTypes.size(); j++) {
                        if (mAtoms[i]->type == vAtomTypes[j]) {
                            foundtype=true;
                            vCountTypes[j]++;
                            break;
                        }
                    }
                    if (!foundtype) {
                        vAtomTypes.push_back(mAtoms[i]->type);
                        vCountTypes.push_back(1);
                    }
                }
                for (int i=0; i<vAtomTypes.size(); i++) {
                    //ss << vCountTypes[i] << " " << vAtomTypes[i] << " "; numentries+=2;
                    ss << vAtomTypes[i] << " "; ++numentries;
                }
                break;
            default: derivedStreamPerGroupProperties(ss, vec[j], numentries); break;
        }
        //(*out) << ss.str();
    }
    //now check for the per-atom properties
    for (int i=0; i<mAtoms.size(); i++) {
        for (int j=0; j<vec.size(); j++) {
            switch (vec[j]) {
                case GROUP_PROPERTY::X: ss << atom(i)->x << " "; ++numentries; break;
                case GROUP_PROPERTY::Y: ss << atom(i)->y << " "; ++numentries; break;
                case GROUP_PROPERTY::Z: ss << atom(i)->z << " "; ++numentries; break;
                case GROUP_PROPERTY::Q: ss << atom(i)->q << " "; ++numentries; break;
                case GROUP_PROPERTY::XU: ss << atom(i)->xu << " "; ++numentries; break;
                case GROUP_PROPERTY::YU: ss << atom(i)->yu << " "; ++numentries; break;
                case GROUP_PROPERTY::ZU: ss << atom(i)->zu << " "; ++numentries; break;
                case GROUP_PROPERTY::VX: ss << atom(i)->vx << " "; ++numentries; break;
                case GROUP_PROPERTY::VY: ss << atom(i)->vy << " "; ++numentries; break;
                case GROUP_PROPERTY::VZ: ss << atom(i)->vz << " "; ++numentries; break;
                case GROUP_PROPERTY::ID: ss << atom(i)->id << " "; ++numentries; break;
                case GROUP_PROPERTY::TYPE: ss << atom(i)->type << " "; ++numentries; break;
                case GROUP_PROPERTY::DELTA: ss << atom(i)->delta << " "; ++numentries; break;
                default: derivedStreamProperties(ss, vec[j], i, numentries); break; //coordinationnumber and coordinationtable only exist for certain kinds of groups
            }
        }
        ss << separator;
      //(*out) << ss.str();
    }
    if (printdistances == true) {
        streamDistances(ss, numentries, general_bool_value);
    }
    /*
      if (mType == GROUP_TYPE::SUMGROUP && general_bool_value == false && mConstituentGroups.size() >= 2) { //if sumgroup, and the "DistDiffGroup" property is set in the parent action (==> general_bool_value == false), only print distances between atoms that are not in the same constituent group.
        for (int i=0; i<mAtoms.size(); i++) {
          for (int j=i+1; j<mAtoms.size(); j++) {
            int hits=0;
            for (int k=0; k<mConstituentGroups.size() && hits==0; k++) {
              if (mConstituentGroups[k]->isMember(mAtoms[i]) && mConstituentGroups[k]->isMember(mAtoms[j]) ) { hits++; }
            }
            if (hits == 0) {
              ss << mAtoms[i]->bonds[mAtoms[j]->id] << " ";
              ++numentries;
            }
          }
        }
      }
      else { //this is what normally happens
        for (int i=0; i<mAtoms.size(); i++) {
          for (int j=i+1; j<mAtoms.size(); j++) {
            ss << mAtoms[i]->bonds[mAtoms[j]->id] << " ";
            ++numentries;
          }
        }
      }
    }
    */
    //(*out) << endl; //one line per timestep
  //}
}

void Group::streamDistances(stringstream &ss, int &numentries, bool general_bool_value) const {
    for (int i=0; i<mAtoms.size(); i++) {
        for (int j=i+1; j<mAtoms.size(); j++) {
            ss << mAtoms[i]->bonds[mAtoms[j]->id] << " ";
            ++numentries;
        }
    }
}

void Group::update(const vector<Atom> *allatoms, const Timestep *t) {
  //if (mInitialized && mStatic) return; //don't do anything if the group is static and it's been updated before
  mInitialized = true;
  if (! mStatic || mUpdateCount==0) internalUpdate(allatoms, t); //call internalUpdate for static groups only of mUpdateCount==0
  incUpdateCount();
  mTotalSize+=size();
  //cout << "size of group  " << getId() << " is now " << size() << endl;
}



bool Group::isMember(const Atom* a) const {
  for (int i=0; i<mAtoms.size(); i++) {
    int mysize=mAtoms.size();
    string id=getId();
    if (mAtoms[i] == a) return true;
  }
  return false;
}
bool Group::isMember(int id) const {
  for (int i=0; i<mAtoms.size(); i++) {
    if (mAtoms[i]->id == id) return true;
  }
  return false;
}

int Group::getMemberId(const Atom* a) const { //returns -1 if the Atom is not in the group.
  for (int i=0; i<mAtoms.size(); i++) {
    if (mAtoms[i] == a) return i;
  }
  return -1;
}
int Group::getMemberId(int id) const { //returns -1 if the Atom is not in the group.
  for (int i=0; i<mAtoms.size(); i++) {
    if (mAtoms[i]->id == id) return i;
  }
  return -1;
}


/*
void Group::setAtomsFromExternalSource(Group *g) {
    clear();
    mAtoms = g->mAtoms;
    mCoordinationNumbers.resize(mAtoms.size(),0);
    mCoordinationTable.resize(mAtoms.size(), Group(GROUP_TYPE::EXTERNAL,""));
}
*/


void Group::clear() { //virtual function, overloaded in GroupCoordination (then also the coordination table arrays are updated)
    mAtoms.clear();
}

void Group::addConstituentGroup(Group *g) {
    for (int i=0; i<mConstituentGroups.size(); i++) {
      if (mConstituentGroups[i] == g) return;
    }
    mConstituentGroups.push_back(g);
    internalAddConstituentGroup(g); //derived classes may need to do additional crazy things (mostly create aliases)
}


int Group::addAtom(const Atom *a) { //virtual function, overloaded in GroupCoordination (then also the coordination table arrays are updated)
   if (isMember(a)) {
        return getMemberId(a);
   }
   mAtoms.push_back(a);
   return mAtoms.size()-1;
}

void Group::internalUpdate(const vector<Atom> *allatoms, const Timestep *timestep) {  //virtual function, must be defined in derived classes
    cout << "cgroup internalupdate... you should never see this; group id = " << mId << endl;
}

void Group::setAtomsFromExternalSource(const Group *g) {
    if (g == this) return;
    clear();
    mAtoms = g->mAtoms;
}


float Group::getMass() const {
    float sum=0;
    for (auto x : mAtoms) {
        //cout << "mass of atom " << x->id << " is " << x->mass << endl;
        sum+=x->mass;
    }
    return sum;
}
