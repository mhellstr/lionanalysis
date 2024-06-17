#ifndef ACTIONDEFINEGROUP_H
#define ACTIONDEFINEGROUP_H

#include "action.h"

class ActionDefineGroup : public Action {
public:
    ActionDefineGroup();
    bool requiresDistances() const override;
protected:
};


#endif
