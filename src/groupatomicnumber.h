#ifndef CGROUPATOMICNUMBER_H
#define CGROUPATOMICNUMBER_H
#include "group.h"

class GroupAtomicNumber : public Group {
public:
    GroupAtomicNumber(string id="");
    void setTargetAtomType(string s) {mTargetAtomType=s;}
    void setTargetAtomMass(float x) {mTargetAtomMass=x; mDoSetMasses=true;}
private:
    void internalUpdate(const vector<Atom> *, const Timestep *) override;
    string mTargetAtomType;
    float mTargetAtomMass;
    bool mDoSetMasses;
};

#endif
