#include "actiondihedralrdf.h"
#include "dihedralrdf.h"

ActionDihedralRDF::ActionDihedralRDF() :Action(ActionType::DihedralRDF, 0, 1000, 1, 1) { }

void ActionDihedralRDF::internalFinalAction(const vector<Atom> *allatoms, const Timestep *t) {
    internalPrint(allatoms, t);
    //for (auto &x : mvpObjects) {
    //    RDF *pRDF = dynamic_cast<RDF*>(x); //only RDF pointers should have been added to mvpObjects, so this should be no problem (casting needed to call print() function)
    //    pRDF->print(mpOut, t);
    //}
}

void ActionDihedralRDF::internalPrint(const vector<Atom> *allatoms, const Timestep *t) {
    for (auto &x : mvpObjects) {
        DihedralRDF *pDihedralRDF = dynamic_cast<DihedralRDF*>(x); //only DihedralRDF pointers should have been added to mvpObjects, so this should be no problem (casting needed to call print() function)
        mpOut->close();
        mpOut->open(mvOutFilenames.front());
        pDihedralRDF->print(mpOut, t);
        mpOut->close();
    }
}
