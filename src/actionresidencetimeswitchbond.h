#ifndef ACTIONRESIDENCETIMESWITCHBOND_H
#define ACTIONRESIDENCETIMESWITCHBOND_H
#include "actionresidencetimecoordinationshell.h"

class ActionResidenceTimeSwitchBond : public ActionResidenceTimeCoordinationShell {
public:
   ActionResidenceTimeSwitchBond();
   void setOverwriteInData(bool a) {mOverwriteInData=a;}
protected:
   TCF* newTCF() const override; 
   bool mOverwriteInData;
};


#endif
