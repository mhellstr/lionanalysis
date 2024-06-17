#include "actionfinalprintproperties.h"
#include "utility.h"

ActionFinalPrintProperties::ActionFinalPrintProperties()
: ActionPrintProperties()
{
}

void ActionFinalPrintProperties::internalAction(const vector<Atom> *atoms, const Timestep *timestep) {
}

void ActionFinalPrintProperties::internalPrint(const vector<Atom> *atoms, const Timestep *timestep) {
    int oldmCount = mCount;
    mpOut->close();
    mpOut->open(mvOutFilenames.front());
    (*mpOut) << "# mCount = " << mCount << " DESCR " << getDescription() << endl;
    mCount = 1; //this is to fool internalAction so that it prints the header
    ActionPrintProperties::internalAction(atoms, timestep);
    mpOut->close();
    mCount = oldmCount;
}

void ActionFinalPrintProperties::internalFinalAction(const vector<Atom> *atoms, const Timestep *timestep) {
    internalPrint(atoms, timestep);
}

