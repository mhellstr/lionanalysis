#include "action.h"
#include "actionanglerdf.h"
#include "anglerdf.h"

ActionAngleRDF::ActionAngleRDF()
:Action(ActionType::AngleRDF,0,1000,0,1000) //, mDoGnu(true), mOutPrefix(""), mOutSuffix("")
{
}

/*
void ActionAngleRDF::setGnu(bool a, const string & p, const string &s) {
    if (mvpOuts.size() < ACTIONANGLERDF_NUMOUTPUTFILES) {
        throw string("Error! Tried to setGnu() before giving all the output files to ActionAngleRDF" + mDescription);
    }
    mDoGnu=a;
    mOutPrefix=p;
    mOutSuffix=s;
    if (mDoGnu) {
        addOut(p + "_r12_a.gnu" + s);
        addOut(p + "_r23_a.gnu" + s);
        addOut(p + "_r13_a.gnu" + s);
        addOut(p + "_r12_r23.gnu" + s);
        addOut(p + "_r23_r13.gnu" + s);
        addOut(p + "_r12_r13.gnu" + s);
    }
}
*/

void ActionAngleRDF::internalFinalAction(const vector<Atom> *allatoms, const Timestep *t) {
    internalPrint(allatoms, t);
    //for (auto &x : mvpObjects) {
        //AngleRDF *pAngleRDF = dynamic_cast<AngleRDF*>(x); //only AngleRDF pointers should have been added to mvpObjects, so this should be no problem (casting needed to call print() function)
        //pAngleRDF->print(mvpOuts, t);
        //if (mDoGnu) {
            //pAngleRDF->printGnu(mvpOuts, mOutPrefix, mOutSuffix);
        //}
    //}
}

void ActionAngleRDF::internalPrint(const vector<Atom> *allatoms, const Timestep *t) {
    if (mvOutFilenames.front() == "/dev/null") return;
    for (int i=0; i<mvpOuts.size(); i++) {
        mvpOuts[i]->close(); //it is a mystery to me why I need this line, but if it's not there the output files will be empty
        mvpOuts[i]->open(mvOutFilenames[i]);
        //cout << "open " << mvOutFilenames[i] << endl;
    }
    for (auto &x : mvpObjects) {
        AngleRDF *pAngleRDF = dynamic_cast<AngleRDF*>(x); //only AngleRDF pointers should have been added to mvpObjects, so this should be no problem (casting needed to call print() function)
        pAngleRDF->print(mvpOuts, t);
        //if (mDoGnu) {
            //pAngleRDF->printGnu(mvpOuts, mOutPrefix, mOutSuffix);
        //}
    }
    for (int i=0; i<mvpOuts.size(); i++) {
        mvpOuts[i]->close();
    }
}
