#ifndef ACTIONPRINTPROPERTIES_H
#define ACTIONPRINTPROPERTIES_H

#include "action.h"
#include "group.h"

enum class ActionPrintFormat { SingleLines, MultipleLines, SingleColumnPerGroup };

class ActionPrintProperties : public Action {
public:
    ActionPrintProperties();
    virtual ~ActionPrintProperties();
    void setPrintPropertiesTimestepIteration(bool a=true) {mPrintPropertiesTimestepIteration = a; }
    void setPrintPropertiesTimestepNumber(bool a=true) {mPrintPropertiesTimestepNumber = a; }
    void setPrintPropertiesCell(bool a=true) {mPrintPropertiesCell = a; }
    void setPrintNumEntriesPerGroup(bool a=true) {mPrintNumEntriesPerGroup = a; }
    void setPrintFormat(ActionPrintFormat a) {mPrintFormat = a; }
    void setDistDiffGroup(bool a=true) {mDistDiffGroup=a;}
    void setPrintSeparator(const string &s) {mPrintSeparator=s;}
    void addProperty(GROUP_PROPERTY x) { mvProperties.push_back(x); }
protected:
    virtual void internalAction(const vector<Atom> *, const Timestep *) override;
    virtual void updateDescription() override;
    bool mPrintPropertiesTimestepIteration; 
    bool mPrintPropertiesTimestepNumber; 
    bool mPrintPropertiesCell;
    bool mPrintNumEntriesPerGroup; 
    bool mDistDiffGroup; //default false
    string mPrintSeparator; //default " "
    std::vector<GROUP_PROPERTY> mvProperties; 
    ActionPrintFormat mPrintFormat;
private:
};


#endif
