#ifndef ACTIONRESIDENCETIMESSP_H
#define ACTIONRESIDENCETIMESSP_H
#include "actionresidencetime.h"

class ActionResidenceTimeSSP : public ActionResidenceTime {
public:
    ActionResidenceTimeSSP();
   void addGroup(Group *g) override;
protected:
   TCF* newTCF() const override; 
   void internalAddToTCF(int, const Atom*) override;
   Group *mpReactants;
   Group *mpProducts;
   void internalFinalAction(const vector<Atom>* allatoms, const Timestep *t) override;
};


#endif
