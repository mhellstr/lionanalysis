#ifndef ACTIONSHAREDLIGANDS_H
#define ACTIONSHAREDLIGANDS_H
#include "action.h"
#include "histogram.h"
#include <vector>

class ActionSharedLigands : public Action {
public:
    ActionSharedLigands();
    void setCentralParentGroup(Group *g) {mpCentralParentGroup=g;}
    void setLigandParentGroup(Group *g) {mpLigandParentGroup=g; }
    void addCentralGroup(Group *g) { mvpCentralGroups.push_back(g); }
    void addLigandGroup(Group *g) {mvpLigandGroups.push_back(g); }
protected:
    Group *mpCentralParentGroup, *mpLigandParentGroup;
    vector<Group*> mvpCentralGroups, mvpLigandGroups;
    void internalAction(const vector<Atom>* allatoms, const Timestep *t) override;
    void internalPrint(const vector<Atom>* allatoms, const Timestep *t) override;
    void internalInitialize(const vector<Atom>* allatoms, const Timestep *t) override;
    void internalFinalAction(const vector<Atom>* allatoms, const Timestep *t) override;
    vector<vector<int>> vCentralCentral;
    vector<vector<Histogram>> vHistoCentral;
    int FindType(const Atom *a, const vector<Group*> &v);
};

#endif
