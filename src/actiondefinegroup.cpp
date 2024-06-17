#include "actiondefinegroup.h"

ActionDefineGroup::ActionDefineGroup() :Action(ActionType::DefineGroup, 0, 0, 0, 0) { }

bool ActionDefineGroup::requiresDistances() const {
    if (mvpObjects.size()>0) {
        for (int i=0; i<mvpObjects.size(); i++) {
            if (dynamic_cast<Group*>(mvpObjects[i])->getRequiresDistances() == true) {
                return true;
            }
        }
        return false;
    }
    return true;
}
