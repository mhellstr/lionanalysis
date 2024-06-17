#ifndef ACTIONTRESIDENCETIMESSP_H
#define ACTIONTRESIDENCETIMESSP_H
#include "actiontresidencetime.h"

class ActionTResidenceTimeSSP : public ActionTResidenceTime {
public:
   ActionTResidenceTimeSSP();
   void addGroup(Group *g) override;
protected:
   virtual TTCF<bool, long long int>* newTCF() const override;
   void internalAddToTCF(int, const Atom*) override;
   Group *mpReactants;
   Group *mpProducts;
   void internalFinalAction(const vector<Atom>* allatoms, const Timestep *t) override;
};


#endif
