#include "actiontresidencetime.h"
#include "ttcf.h"
#include "ttcfresidencetime.h"

ActionTResidenceTime::ActionTResidenceTime() 
: ActionTTCF<INTYPE,OUTTYPE>(ActionType::ResidenceTime,2,3,1,5, TCF_t::ResidenceTime)
{
    mNumItemsPerTimeStep=1;
}
ActionTResidenceTime::ActionTResidenceTime(ActionType actiontype, TCF_t tcftype)
: ActionTTCF<INTYPE,OUTTYPE>(actiontype, 2, 3, 1, 5, tcftype)
{ 
    mNumItemsPerTimeStep=1;
}

ActionTResidenceTime::~ActionTResidenceTime()
{ }


void ActionTResidenceTime::internalMoreAtomsThanExpected(int numatoms) {
}

void ActionTResidenceTime::internalAtomLeftEnteredVector(int zerobasedid) {
    //mTCF[zerobasedid]->clear(); important to NOT clear this thing for ResidenceTime
}
void ActionTResidenceTime::internalAddToTCF(int zerobasedid, const Atom* a) {
    //cout << mpSubGroup->getId() << endl;
    //cout << mpGroup->getId() << endl;
    bool b = mpSubGroup->isMember(a);
    mTCF[zerobasedid]->add(b);
    addValueBoolInfo(zerobasedid, b, 1);
    //mTCF[zerobasedid]->add(mpSubGroup->isMember(a));
    //if (mpSubGroup->isMember(a)) {
        //mTCF[zerobasedid]->add(true);
        //if (mvBoolsToSave[BoolInfoIndex::IsMember1]) mTCF[zerobasedid]->addBoolInfo(true, BoolInfoIndex::IsMember1);
        //cout << "adding true " << endl;
    //}
    //else {
        //mTCF[zerobasedid]->add(false);
        //if (mvBoolsToSave[BoolInfoIndex::IsMember1]) mTCF[zerobasedid]->addBoolInfo(false, BoolInfoIndex::IsMember1);
        //cout << "adding false " << endl;
    //}
}

TTCF<bool, long long int>* ActionTResidenceTime::newTCF() const {
    TTCF<bool, long long int>* a = new TTCFResidenceTime(this);
    //a->setParentAction(this);
    //setTTCFConditions(a);
    //static_cast<TTCFResidenceTime*>(a)->setOldMustBeMember(mOldMustBeMember);
    //static_cast<TTCFResidenceTime*>(a)->setTidyOld(mTidyOld);
    return a;
}





void ActionTResidenceTime::internalFinalAction(const vector<Atom>* allatoms, const Timestep *t) {
    ActionTTCF::internalFinalAction(allatoms, t);
    float offset=getGnuplotFitOffset();
    stringstream ss;
    ss << offset;
    string offsetstring=ss.str();

    //if (mvpOuts.size() >= 2 && mvOutFilenames.size() >=1 && mAverage == true) {
    //    ofstream *o = mvpOuts.at(1);
    //    string tcffilename = mvOutFilenames.at(0);
    //    string cmd = "offset = " + offsetstring + "; " + R"( 
    //        f(x) = A1*exp(-(x-offset)/t1) + (1-A1)*exp(-(x-offset)/t2)
    //        g(x) = B1*exp(-(x-offset)/T1) + B2*exp(-(x-offset)/T2) + (1-B1-B2)*exp(-(x-offset)/T3)
    //        A1 = 0.1; 
    //        B1 = 0.1; B2 = 0.2; 
    //        t1=1; t2=10;
    //        T1=1; T2=3; T3=10;
    //        fit f(x) ')" + tcffilename + R"(' using 1:(column(1) > offset ? column(2) : 1/0)  via A1, t1, t2
    //        fit g(x) ')" + tcffilename + R"(' using 1:(column(1) > offset ? column(2) : 1/0)  via B1, B2, T1, T2, T3
    //        ftitle = sprintf( "%.3f*exp(-x/%.3f)+%.3f*exp(-x/%.3f) ", A1, t1, 1-A1, t2)
    //        gtitle = sprintf( "%.3f*exp(-x/%.3f)+%.3f*exp(-x/%.3f)+%.3f*exp(-x/%.3f) ", B1, T1, B2, T2, 1-B1-B2, T3)
    //        plot ')" + tcffilename + R"(' using 1:2 w l lw 2 title ')" + getDescription() + R"(', f(x) title ftitle, g(x) title gtitle
    //    )";
    //    (*o) << cmd;
    //}

}


void ActionTResidenceTime::addValueBoolInfo(int zerobasedid, bool value, const int reactantsorproducts) {
    vector<bool> vAddedBoolInfo(16,false);
    for (int i=0; i<mvTCFConditions.size(); i++) {
        switch (mvTCFConditions[i].type)  {
            case TCFConditionType::OldValueIsTrueFor1:
            case TCFConditionType::NewValueIsTrueFor1:
                //cout << "1 x " << endl;
                if (reactantsorproducts != 1) break; //break froms witch
                if (!vAddedBoolInfo[mvTCFConditions[i].bii]) {
                    mTCF[zerobasedid]->addBoolInfo(value, mvTCFConditions[i].bii);
                }
                vAddedBoolInfo[mvTCFConditions[i].bii]=true;
                break; //break froms witch
            case TCFConditionType::OldValueIsTrueFor2:
            case TCFConditionType::NewValueIsTrueFor2:
                if (reactantsorproducts != 2) break; //break from switch
                if (!vAddedBoolInfo[mvTCFConditions[i].bii]) {
                    //cout << value << endl;
                    mTCF[zerobasedid]->addBoolInfo(value, mvTCFConditions[i].bii);
                }
                vAddedBoolInfo[mvTCFConditions[i].bii]=true;
                break; //break froms witch
            //case TCFConditionType::OldValueIsTrueFor3:
            //case TCFConditionType::OldValueIsFalseFor3:
            //case TCFConditionType::OldValueIsTrueFor4:
            //case TCFConditionType::OldValueIsFalseFor4:
            //case TCFConditionType::NewValueIsTrueFor3:
            //case TCFConditionType::NewValueIsFalseFor3:
            //case TCFConditionType::NewValueIsTrueFor4:
            //case TCFConditionType::NewValueIsFalseFor4:
            //case TCFConditionType::OldValueIsFalseFor1:
            //case TCFConditionType::NewValueIsFalseFor1:
            //case TCFConditionType::OldValueIsFalseFor2:
            //case TCFConditionType::NewValueIsFalseFor2:
        }
    }
}

