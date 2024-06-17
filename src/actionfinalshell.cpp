#include "actionfinalshell.h"

ActionFinalShell::ActionFinalShell()
: Action(ActionType::FinalShell, 0, 0, 0, 0), mCommand("")
{ }

void ActionFinalShell::internalAction(const vector<Atom> *atoms, const Timestep *timestep) {

}
void ActionFinalShell::internalFinalAction(const vector<Atom> *atoms, const Timestep *timestep) {
    std::cout << mCommand << std::endl;
    const int systemresult = system(mCommand.c_str());
}
