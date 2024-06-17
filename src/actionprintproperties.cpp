#include "actionprintproperties.h"
#include "utility.h"

ActionPrintProperties::ActionPrintProperties()
: Action(ActionType::PrintProperties, 0, 10000, 1, 1),
mPrintFormat(ActionPrintFormat::SingleLines),
mPrintSeparator(" "),
mPrintNumEntriesPerGroup(false),
mPrintPropertiesTimestepNumber(false),
mPrintPropertiesTimestepIteration(false),
mDistDiffGroup(false)
{
}

ActionPrintProperties::~ActionPrintProperties() { }

void ActionPrintProperties::internalAction(const vector<Atom> *atoms, const Timestep *timestep) {
    stringstream ss("");
    if (mCount == 1) {
        ss << "#"; //
        if (mPrintFormat == ActionPrintFormat::SingleLines) {
            //if printformat is multiplelines, then the timestep iteration/number get printed on the "FRAME" line anyway so no need to print in the header (makes postprocessing more complicated)
            if (mPrintPropertiesTimestepIteration) ss << "iter ";
            if (mPrintPropertiesTimestepNumber) ss << "numb ";
            if (mPrintPropertiesCell) ss << "cellx celly cellz ";
        }
        for (auto& x : mvpGroups) {
            ss << x->getId() << " ";
        }
        (*mpOut) << ss.str() << "\n";
        ss.clear();
        ss.str(std::string());
    }
    if (mPrintPropertiesTimestepIteration) ss << timestep->iteration << " ";
    if (mPrintPropertiesTimestepNumber) ss << timestep->number << " ";
    if (mPrintPropertiesCell) ss << timestep->cellx << " " << timestep->celly << " " << timestep->cellz << " ";
    if (mPrintFormat == ActionPrintFormat::MultipleLines) {
        (*mpOut) << "### FRAME " << ss.str() << " ###\n";
        ss.clear(); ss.str(std::string());
    }
        
    int numentries=0;
    for (int i=0; i<mvpGroups.size(); i++) {
        numentries=0;
        //cout << " time to print properites of group " << mvpGroups[i]->getId() << " size: " << mvpGroups[i]->size() << endl;
        mvpGroups[i]->streamProperties(ss, mvProperties, numentries, mPrintSeparator, mDistDiffGroup);
        if (mPrintNumEntriesPerGroup) (*mpOut) << numentries << " " << mPrintSeparator << ss.str();
        else (*mpOut) << ss.str();
        if (i==mvpGroups.size()-1 || mPrintFormat == ActionPrintFormat::MultipleLines) (*mpOut) << "\n";
        else (*mpOut) << " ";
        ss.clear(); ss.str(std::string());
    }
    //(*mpOut) << flush;
}

void ActionPrintProperties::updateDescription() {
    stringstream ss("");
    ss << "ActionPrintProperties: GROUPS ";
    for (auto &x : mvpGroups) {
        ss << x->getId() << " ";
    }
    ss << "PROPERTIES: ";
    for (auto &x : mvProperties) {
        ss << PropertyToString(x) << " ";
    }
    setDescription(ss.str());
}
