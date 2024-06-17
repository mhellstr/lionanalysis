#ifndef UTILITY_H
#define UTILITY_H

#include "action.h"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>
#include <string>
#include "timestep.h"
#include "atom.h"
#include "rdf.h"
#include "constants.h"
#include "groupincludes.h"
#include "consequence.h"
#include "actionttcf.h"
#include "angletype.h"

using namespace std;
//class Action;

enum class Overwrite { Ask, YesAll, DevNull };

typedef std::pair<float,int> floatintpair ;
typedef vector<floatintpair> floatintpairvec ;

typedef pair< std::string, float> NamedConst;

void trim(string& str);
void trim_trailing(string& str);
void stow(const string &s, vector<string> &v);

bool permission_to_write(string &totalfilename, Overwrite &overwritefiles);
bool FileExists(string strFilename);
template<class T> void CloseAllFiles(vector<T> &v);
//void CloseAllFiles(vector<ofstream*> &);
//void CloseAllFiles(vector<ifstream*> &);
double gettime(void);

bool read_frame(istream &xmolout, vector<Atom> &atoms, Timestep &curtimestep, string format);

float distance(float,float,float,float,float,float,const Timestep*);
void periodicdistances(float,float,float,float,float,float,float *,const Timestep*);
float angle(const Atom* a, const Atom* b, const Atom* c, const Timestep*);
float angle(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, const Timestep *curtimestep);
float signedangle(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3,  float z3, const AngleType, const Timestep *t);
float dihedral(const Atom*, const Atom*, const Atom*, const Atom*, const Timestep*);
void cross_prod(float,float,float, float,float,float, float&,float&,float&);
void TranslateAtomCoordinatesToVicinity(float&, float&, float&, float, float, float, const Timestep *);
//void addGroup(vector<Group> &v, const Group &);
Group* addGroup(vector<Group*> &, Group*);
Action* addAction(vector<Action*> &, Action*);
CObject *addObject(vector<CObject*> &, CObject *);

GROUP_PROPERTY StringToProperty(const std::string&);
string PropertyToString(const GROUP_PROPERTY);

void calculateVelocities(const vector<Atom>& oldatoms, vector<Atom>& newatoms);
void calculateMomentum(const vector<Atom>& atoms, Timestep *timestep); //modifies timestep
void intelligentUnwrap(const vector<Atom>& oldatoms, vector<Atom>& newatoms, const Timestep *timestep);
void deleteObjects(vector<CObject*> &v);

int stringAtomToNumAtom(const string &s);
float getAtomMass(const int);

bool isFloat(string);
std::ifstream::pos_type getFileSize(const string& filename);
std::string ReplaceString(std::string subject, const std::string& search, const std::string& replace);

int FindGroupIndexWithId(vector<Group> &groups, string groupname);
//Group *FindGroupWithId(vector<Group> &, const string&);
Group *FindGroupWithId(vector<Group*> &, const string&);
Group *FindGroupWithIdNoThrow(vector<Group*> &, const string&);
Action *FindActionWithId(vector<Action*> &, const string&);
Consequence stringToConsequence(const string &s);
TCFConditionType stringToTCFConditionType(const string&);
void parseTCFOptions(vector<string> &w, vector<string> &origw, ActionTTCFParent *, vector<Action*> &vpActions, vector<Group*> &vpGroups, vector<NamedConst> &, float, int, string, string, Overwrite&);


template <class T> void print_vector(ostream& out, vector<T> &v) ;
template <class T> bool from_string(T& t, const std::string& s, vector<NamedConst> *v=nullptr);
template <class T> T from_string(const std::string& s, vector<NamedConst> *v=nullptr);

template <class T> void print_vector(ostream& out, vector<T> &v, Timestep *t) {
  if (t != nullptr) {
    for (int i=0; i<v.size(); i++) {
      out << t->number << " " << i << " " << v[i] << endl;
    }
  }
  else {
    for (int i=0; i<v.size(); i++) {
      out << i << " " << v[i] << endl;
    }
  }
}


template <class T> bool from_string(T& t, const std::string& s, vector<NamedConst> *v)
{
    if (v!=nullptr) {
        for (int i=0; i<v->size(); i++) {
            if ((*v)[i].first == s) {
                //t = (T) (*v)[i].second;
                t = (*v)[i].second;
                //cout << "returning t " << t << " from constant " << s << endl;
                return true;
            }
        }
    }
    std::istringstream iss(s);
    iss >> noskipws >> t;
    //if (!iss.eof() || iss.fail()) {
    if (iss.fail()) {
        throw string("String " + s + " not of expected type");
    }
    //return !(iss >> t).fail();
    return true;
}
template <class T> T from_string(const std::string& s, vector<NamedConst> *v)
{
    if (v!=nullptr) {
        for (int i=0; i<v->size(); i++) {
            if ((*v)[i].first == s) {
                //t = (T) (*v)[i].second;
                return (*v)[i].second;
                //cout << "returning t " << t << " from constant " << s << endl;
                //return true;
            }
        }
    }
    std::istringstream iss(s);
    T t;
    iss >> t;
    //if (!iss.eof() || iss.fail()) {
        //throw string("String " + s + " is not of the expected type.");
    //}
    return t;
    //return !(iss >> t).fail();
}

template <class T> void CloseAllFiles(vector<T> &v) { //T should be ofstream* or ifstream*, that were created with new from the main loop
    for (auto x : v) {
        //cout << "attempting to close: " << x << endl;
        x->close();
        delete x;
    }
}


#endif
