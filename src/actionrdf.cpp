#include "actionrdf.h"
#include "rdf.h"

ActionRDF::ActionRDF() :Action(ActionType::RDF, 0, 1000, 1, 1) { }

void ActionRDF::internalFinalAction(const vector<Atom> *allatoms, const Timestep *t) {
    internalPrint(allatoms, t);
    //for (auto &x : mvpObjects) {
    //    RDF *pRDF = dynamic_cast<RDF*>(x); //only RDF pointers should have been added to mvpObjects, so this should be no problem (casting needed to call print() function)
    //    pRDF->print(mpOut, t);
    //}
}

void ActionRDF::internalPrint(const vector<Atom> *allatoms, const Timestep *t) {
    for (auto &x : mvpObjects) {
        RDF *pRDF = dynamic_cast<RDF*>(x); //only RDF pointers should have been added to mvpObjects, so this should be no problem (casting needed to call print() function)
        mpOut->close();
        mpOut->open(mvOutFilenames.front());
        pRDF->print(mpOut, t);
        mpOut->close();
    }
}
