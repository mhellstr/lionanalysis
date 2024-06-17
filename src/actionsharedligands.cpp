#include "actionsharedligands.h"
#include "group.h"
#include "groupcoordination.h"
#include "utility.h"


ActionSharedLigands::ActionSharedLigands()
: Action(ActionType::SharedLigands, 1, 1, 1, 6), mpLigandParentGroup(nullptr), mpCentralParentGroup(nullptr)
{
}

int ActionSharedLigands::FindType(const Atom *a, const vector<Group*> &v)
{
    for (int i=0; i<v.size(); i++) {
        if (v[i]->isMember(a)) return i;
    }
    return -1;
}

void ActionSharedLigands::internalAction(const vector<Atom>* allatoms, const Timestep *t) {
    vCentralCentral.assign(vCentralCentral.size(), vector<int>(vCentralCentral.size(), 0));
    //for (int i=0; i<vCentralCentral.size(); i++) {
    //    for (int j=0; j<vCentralCentral[i].size(); j++) {
    //        vCentralCentral[i][j]=0;
    //    }
    //}
    const GroupCoordination *g=dynamic_cast<GroupCoordination*>(mpGroup);
    for (int i=0; i<g->size(); i++) { //mpGroup is O_Na
        //cout << i << endl;
        const Group* ct=g->getCoordinationTable(i);
        int ligandcoordinationnumber=g->getCoordinationNumber(i);
        int inligandid=FindType(g->atom(i), mvpLigandGroups);
        if (inligandid < 0) throw string("unknown ligand type!");
        const Group* ligandtype=mvpLigandGroups[inligandid];
        //double loop over the coordination table
        for (int j=0; j<ct->size(); j++) {
            //cout << " j " << j << " " << ct->size() << endl;
            int id1=mpCentralParentGroup->getMemberId(ct->atom(j));
            if (id1 < 0) throw string("This was not supposed to happen");
            for (int k=0; k<j; k++) {
                int id2=mpCentralParentGroup->getMemberId(ct->atom(k));
                if (id2 < 0) throw string("Nor was this supposed to happen");
                //cout << g->atom(i)->id << " " << id1 << " " << id2 << endl;
                vCentralCentral[id1][id2]++;
                vCentralCentral[id2][id1]++;
            }
        }
    }

    //double loop over the central ions
    for (int j=0; j<vCentralCentral.size(); j++) {
        int invid1=FindType(mpCentralParentGroup->atom(j), mvpCentralGroups);
        if (invid1 < 0) continue;
        const Group* central1type=mvpCentralGroups[invid1];
        string central1id=central1type->getId();
        for (int k=0; k<j; k++) {
        //for (int k=0; k<j; k++) {
            if (vCentralCentral[j][k] > 0) {
                int invid2=FindType(mpCentralParentGroup->atom(k), mvpCentralGroups);
                if (invid2 < 0) continue;
                const Group *central2type=mvpCentralGroups[invid2];
                string central2id=central2type->getId();
                //if (central1id < central2id) { (*mpOut) << central1id << " " << central2id << " "; }
                //else { (*mpOut) << central2id << " " << central1id << " "; }
                
                //LOTS OF INFO:
                //(*mpOut) << t->iteration << " " << mpCentralParentGroup->atom(j)->id << " " << central1id << " " << mpCentralParentGroup->atom(k)->id << " " << central2id << " " << vCentralCentral[j][k] << endl;
                //(*mpOut) << central2id << " " << central1id << " ";
                //(*mpOut) << vCentralCentral[j][k] << "\n";
                //(*mpOut) << central1id << " " << central2id << " ";
                //(*mpOut) << vCentralCentral[j][k] << "\n";


                vHistoCentral[invid1][invid2].add(vCentralCentral[j][k]);
                vHistoCentral[invid2][invid1].add(vCentralCentral[j][k]);
            }
        }
    }
}

void ActionSharedLigands::internalPrint(const vector<Atom>* allatoms, const Timestep *t) {
    for (int i=0; i<mvpCentralGroups.size(); i++) {
        for (int j=0; j<mvpCentralGroups.size(); j++) {
            //cout << mvpCentralGroups[i]->getId() << " " << mvpCentralGroups[j]->getId() << endl;
            long long int sum=vHistoCentral[i][j].sum();
            long long int sumcount=vHistoCentral[i][j].sumCount();
            double norm1;
            if (sum > 0) norm1= 1.0f/sum;
            else norm1=0;
            for (int k=0; k<vHistoCentral[i][j].size(); k++) {
                //cout << i << " " << j << " " << k << " " << mvpCentralGroups.size() << " " << vHistoCentral[i][j].size() << endl;
                double norm2;
                if (vHistoCentral[i][j].getCount(k) > 0) norm2=1.0f / vHistoCentral[i][j].getCount(k);
                else norm2=0;
                //(*mpOut) << mvpCentralGroups[i]->getId() << "\n";

                //THIS IS THE MONEY LINE: UNCOMMENT BEFORE COMMITTING
                (*mpOut) << mvpCentralGroups[i]->getId() << " " << mvpCentralGroups[j]->getId() << " " << vHistoCentral[i][j].indexToReal(k) << " " << vHistoCentral[i][j].getElement(k) << " " << vHistoCentral[i][j].getElement(k) * norm1 << "\n";
            }

            //wavg is the AVERAGE NUMBER OF SHARED LIGANDS between two specific kinds of central ions, GIVEN that they share at least one ligand!
            double wavg=vHistoCentral[i][j].weightedAverage();
            (*mvpOuts[1]) << mvpCentralGroups[i]->getId() << " " << mvpCentralGroups[j]->getId() << " " << wavg << " " << "\n";
        }
    }
}
void ActionSharedLigands::internalInitialize(const vector<Atom>* allatoms, const Timestep *t) {
    //cout << "initializ" << endl;
    vCentralCentral.resize(mpCentralParentGroup->size());
    for (int i=0; i<vCentralCentral.size(); i++) {
        vCentralCentral[i].resize(mpCentralParentGroup->size());
    }
    vHistoCentral.resize(mvpCentralGroups.size());
    for (int i=0; i<vHistoCentral.size(); i++) {
        vHistoCentral[i].resize(mvpCentralGroups.size());
        for (int j=0; j<vHistoCentral[i].size(); j++) {
            vHistoCentral[i][j].setMinVal(0);
            vHistoCentral[i][j].setMaxVal(4);
            vHistoCentral[i][j].setResolution(1);
            vHistoCentral[i][j].initialize(); //needed in case histogram.add() is never called
        }
    }
    //cout << "END" << endl;
}

void ActionSharedLigands::internalFinalAction(const vector<Atom>* allatoms, const Timestep *t) {
    internalPrint(allatoms, t);
}
