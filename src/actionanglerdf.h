#ifndef ACTIONANGLERDF_H
#define ACTIONANGLERDF_H

#include "action.h"

#define ACTIONANGLERDF_NUMOUTPUTFILES 8

class ActionAngleRDF : public Action {
public:
    ActionAngleRDF();
    //void setGnu(bool, const string&, const string&);
protected:
    void internalFinalAction(const vector<Atom> *, const Timestep *) override;
    void internalPrint(const vector<Atom> *, const Timestep *) override;
    //bool mDoGnu;
    //string mOutPrefix;
    //string mOutSuffix;
};


#endif
