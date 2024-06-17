#include "actionresidencetime.h"
#include "tcf.h"
#include "tcfresidencetime.h"

ActionResidenceTime::ActionResidenceTime() 
: ActionTCF(ActionType::ResidenceTime,2,3,1,5, TCF_t::ResidenceTime), mOldMustBeMember(false), mAllowRecrossings(false)
{
    mNumItemsPerTimeStep=1;
}
ActionResidenceTime::ActionResidenceTime(ActionType actiontype, TCF_t tcftype)
: ActionTCF(actiontype, 2, 3, 1, 5, tcftype), mOldMustBeMember(false), mAllowRecrossings(false)
{ 
    mNumItemsPerTimeStep=1;
}

ActionResidenceTime::~ActionResidenceTime()
{ }


void ActionResidenceTime::internalMoreAtomsThanExpected(int numatoms) {
}

void ActionResidenceTime::internalAtomLeftEnteredVector(int zerobasedid) {
    //mTCF[zerobasedid]->clear(); important to NOT clear this thing for ResidenceTime
}
void ActionResidenceTime::internalAddToTCF(int zerobasedid, const Atom* a) {
    if (mpSubGroup->isMember(a)) {
        mTCF[zerobasedid]->add(1);
    }
    else {
        mTCF[zerobasedid]->add(0);
    }
}

TCF* ActionResidenceTime::newTCF() const {
    TCF* a = new TCFResidenceTime;
    static_cast<TCFResidenceTime*>(a)->setOldMustBeMember(mOldMustBeMember);
    static_cast<TCFResidenceTime*>(a)->setTidyOld(mTidyOld);
    return a;
}


void ActionResidenceTime::addGroup(Group *g) {
    mvpGroups.push_back(g);
    mpGroup=mvpGroups[0];
    if (mvpGroups.size() > 1) {
        mpSubGroup = mvpGroups[1];
    }
}



void ActionResidenceTime::internalFinalAction(const vector<Atom>* allatoms, const Timestep *t) {
    ActionTCF::internalFinalAction(allatoms, t);
    float offset=getGnuplotFitOffset();
    stringstream ss;
    ss << offset;
    string offsetstring=ss.str();
//    if (mvpOuts.size() >= 2 && mvOutFilenames.size() >=1 && mAverage == true) {
//        ofstream *o = mvpOuts.at(1);
//        string tcffilename = mvOutFilenames.at(0);
//        string cmd = "offset = " + offsetstring + "; " + R"( 
//            f(x) = A1*exp(-(x-offset)/t1) + (1-A1)*exp(-(x-offset)/t2)
//            g(x) = B1*exp(-(x-offset)/T1) + B2*exp(-(x-offset)/T2) + (1-B1-B2)*exp(-(x-offset)/T3)
//            A1 = 0.1; 
//            B1 = 0.1; B2 = 0.2; 
//            t1=1; t2=10;
//            T1=1; T2=3; T3=10;
//            fit f(x) ')" + tcffilename + R"(' using 1:(column(1) > offset ? column(2) : 1/0)  via A1, t1, t2
//            fit g(x) ')" + tcffilename + R"(' using 1:(column(1) > offset ? column(2) : 1/0)  via B1, B2, T1, T2, T3
//            ftitle = sprintf( "%.3f*exp(-x/%.3f)+%.3f*exp(-x/%.3f) ", A1, t1, 1-A1, t2)
//            gtitle = sprintf( "%.3f*exp(-x/%.3f)+%.3f*exp(-x/%.3f)+%.3f*exp(-x/%.3f) ", B1, T1, B2, T2, 1-B1-B2, T3)
//            plot ')" + tcffilename + R"(' using 1:2 w l lw 2 title ')" + getDescription() + R"(', f(x) title ftitle, g(x) title gtitle
//        )";
//        (*o) << cmd;
//    }

}
