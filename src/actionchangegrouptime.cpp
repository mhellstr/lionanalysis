#include "actionchangegrouptime.h"
#include "actionwhengroupchanged.h"
#include "groupcoordination.h"

ActionChangeGroupTime::ActionChangeGroupTime()
: Action(ActionType::ChangeGroupTime, 4, 4, 1, 1), mpReactants(nullptr), mpProducts(nullptr), mpShellGroup(nullptr), mpCannotBe(nullptr), mMinReactantTime(0), mDoPrintFile(true), mMaxReactionTime(1000000), mCheckCoordinationTable(false)
{
}

void ActionChangeGroupTime::internalInitialize(const vector<Atom> *allatoms, const Timestep *timestep) {
    int na=mpGroup->size();
    int vecsize=na;
    if (mCheckCoordinationTable) {
        if (dynamic_cast<GroupCoordination*>(mpReactants) == nullptr) throw string("Error! ActionChangeGroupTime with CheckCoordinationTable but no coordination table in group " + mpReactants->getId());
        if (dynamic_cast<GroupCoordination*>(mpProducts) == nullptr) throw string("Error! ActionChangeGroupTime with CheckCoordinationTable but no coordination table in group " + mpProducts->getId());
        vecsize=na*mpShellGroup->size();
    }
    mvFirstEntered.resize(vecsize, 0);
    mvContinuousBeforeLeaving.resize(vecsize, 0);
    mvLastEntered.resize(vecsize, 0);
    mvProductEntered.resize(vecsize, 0);
    mvMemberBefore.resize(vecsize, false);
}

void ActionChangeGroupTime::addGroup(Group *g) {
    mvpGroups.push_back(g);
    mpGroup=mvpGroups[0];
    if (mvpGroups.size() > 1) {
        mpReactants=mvpGroups[1];
        if (mvpGroups.size() > 2) {
            mpProducts = mvpGroups[2];
            if (mvpGroups.size() > 3) {
                mpCannotBe = mvpGroups[3];
            }
        }
    }
}

void ActionChangeGroupTime::printLine(int i) {
    int atomid;
    if (mCheckCoordinationTable) { atomid=mpShellGroup->atom(i % mpShellGroup->size())->id; }
    else { atomid=mpGroup->atom(i)->id; }
    (*mpOut) << atomid << " " << mvFirstEntered[i] << " " << mvContinuousBeforeLeaving[i] << " " << mvLastEntered[i] << " " << mvProductEntered[i] << endl;
    //(*mpOut) << mpGroup->atom(i)->id << " " << mvFirstEntered[i] << " " << mvContinuousBeforeLeaving[i] << " " << mvLastEntered[i] << " " << mvProductEntered[i] << endl;
}

void ActionChangeGroupTime::printLine(const Atom *a, int i) {
    int atomid=a->id;
    (*mpOut) << atomid << " " << mvFirstEntered[i] << " " << mvContinuousBeforeLeaving[i] << " " << mvLastEntered[i] << " " << mvProductEntered[i] << endl;
}

void ActionChangeGroupTime::internalAction(const vector<Atom> *allatoms, const Timestep *timestep)
{
    if (mCheckCoordinationTable) {
        internalActionCheckCoordinationTable(allatoms, timestep);
    }
    else {
        int it=timestep->iteration;
        //cout << mpGroup->getId() << " " << mpReactants->getId() << " " << mpProducts->getId() << endl;
        for (int i=0; i<mpGroup->size(); i++) {
            const Atom *a =mpGroup->atom(i);
            if (mpReactants->isMember(a)) {
                //cout << " i is reactant!";
                if (!mvMemberBefore[i]) {
                    if (mvFirstEntered[i] == 0) {
                        //cout << "mvFirstEntered[" << i << "] = " << it << endl;
                        mvFirstEntered[i]=it;
                    }
                    mvContinuousBeforeLeaving[i]=it;
                    //cout << "mvLastEntered[" << i << "] = " << it << endl;
                }
                mvLastEntered[i]=it;
                    //cout << "mvFirstEntered[" << i << "] = " << mvFirstEntered[i]  << " "
                         //<< "mvLastEntered[" << i << "] = " << mvLastEntered[i] <<  endl;
            }
            if (mpProducts->isMember(a)) {
                //cout << i << " PRODUCT " << endl;
                if (mvFirstEntered[i] > 0 && mvLastEntered[i] >0) {
                    mvProductEntered[i]=it;
                    //cout << it << " xPRODUCT " << endl;
                    if (satisfyPrintConstraints(i)) {
                        if (mDoPrintFile) {
                            printLine(i);
                        }
                        for (int j=0; j<mvActionWhenGroupChanged.size(); j++) {
                            if (mvActionWhenGroupChanged[j] != nullptr) {
                                mvActionWhenGroupChanged[j]->addNewChange(a, mvFirstEntered[i], mvLastEntered[i], mvProductEntered[i]);
                            }
                        }
                    }
                    mvFirstEntered[i]=0;
                    mvLastEntered[i]=0;
                    mvContinuousBeforeLeaving[i]=0;
                }
            }
            mvMemberBefore[i]=mpReactants->isMember(a);
        }
    }
}

void ActionChangeGroupTime::internalActionCheckCoordinationTable(const vector<Atom>* atoms, const Timestep *timestep) {
    int it=timestep->iteration;
    //cout << mpGroup->getId() << " " << mpReactants->getId() << " " << mpProducts->getId() << endl;
    int shellgroupsize=mpShellGroup->size();
    for (int i=0; i<mpGroup->size(); i++) {
        const Atom *a =mpGroup->atom(i);
        int ingroupid=mpReactants->getMemberId(a);
        if (ingroupid >=0) {
            const Group *ct=static_cast<GroupCoordination*>(mpReactants)->getCoordinationTable(ingroupid);
            //cout << " i is reactant!";
            for (int j=0; j<shellgroupsize; j++) {
                int ii=i*shellgroupsize+j;
                const Atom *shellatom=mpShellGroup->atom(j);
                if (ct->isMember(shellatom)) {
                    if (!mvMemberBefore[ii]) {
                        if (mvFirstEntered[ii] == 0) {
                            //cout << "mvFirstEntered[" << i << "] = " << it << endl;
                            mvFirstEntered[ii]=it;
                        }
                        mvContinuousBeforeLeaving[ii]=it;
                        //cout << "mvLastEntered[" << i << "] = " << it << endl;
                    }
                    mvLastEntered[ii]=it;
                    //cout << "mvFirstEntered[" << ii << "] = " << mvFirstEntered[ii]  << " "
                         //<< "mvLastEntered[" << ii << "] = " << mvLastEntered[ii] <<  endl;
                }
            }
        }
        if (mpCannotBe != nullptr) {
            ingroupid=mpCannotBe->getMemberId(a);
            if (ingroupid>=0) {
                const Group *ct=static_cast<GroupCoordination*>(mpCannotBe)->getCoordinationTable(ingroupid);
                for (int j=0; j<shellgroupsize; j++) {
                    int ii=i*shellgroupsize+j;
                    if (ct->isMember(mpShellGroup->atom(j))) {
                        mvFirstEntered[ii]=0;
                        mvLastEntered[ii]=0;
                        mvContinuousBeforeLeaving[ii]=0;
                    }
                }
            }
        }
        ingroupid=mpProducts->getMemberId(a);
        if (ingroupid>=0) {
            //cout << i << " PRODUCT " << endl;
            const Group *ct=static_cast<GroupCoordination*>(mpProducts)->getCoordinationTable(ingroupid);
            for (int j=0; j<shellgroupsize; j++) {
                int ii=i*shellgroupsize+j;
                if (ct->isMember(mpShellGroup->atom(j))) {
                    if (mvFirstEntered[ii] > 0 && mvLastEntered[ii] >0) {
                        mvProductEntered[ii]=it;
                        //cout << it << " xPRODUCT " << endl;
                        if (satisfyPrintConstraints(ii)) {
                            if (mDoPrintFile) {
                                printLine(mpShellGroup->atom(j), ii);
                            }
                            for (int k=0; k<mvActionWhenGroupChanged.size(); k++) {
                                if (mvActionWhenGroupChanged[k] != nullptr) {
                                    mvActionWhenGroupChanged[k]->addNewChange(mpShellGroup->atom(j), mvFirstEntered[ii], mvLastEntered[ii], mvProductEntered[ii]);
                                    //cout << "calling addnewhcnage" << endl;
                                }
                            }
                        }
                        mvFirstEntered[ii]=0;
                        mvLastEntered[ii]=0;
                        mvContinuousBeforeLeaving[ii]=0;
                    }
                }
            }
        }
        ingroupid=mpReactants->getMemberId(a);
        for (int j=0; j<shellgroupsize; j++) {
            int ii=i*shellgroupsize+j;
            if (ingroupid>=0) {
                mvMemberBefore[ii]=static_cast<GroupCoordination*>(mpReactants)->isMemberOfCoordinationShell(ingroupid, mpShellGroup->atom(j));
                //if (mvMemberBefore[ii]) cout << "mvMB[" << ii << "]=" << mvMemberBefore[ii] << " ";
            }
            else {
                mvMemberBefore[ii]=false;
            }
        }
        //cout << endl;
    }
}



void ActionChangeGroupTime::internalFinalAction(const vector<Atom> *, const Timestep *) {
}
bool ActionChangeGroupTime::satisfyPrintConstraints(int i) const {
    if (mvLastEntered[i] - mvFirstEntered[i] < mMinReactantTime) {
        //cout << " no success! " << i << " " << i%mpShellGroup->size() << " " << mvLastEntered[i] << " " << mvFirstEntered[i] << " " << mMinReactantTime << endl;
        return false;
    }
    if (mvProductEntered[i] - mvLastEntered[i] > mMaxReactionTime) {
        return false;
    }
        //cout << " success! " << i << " " << mvLastEntered[i] << " " << mvFirstEntered[i] << " " << mMinReactantTime << endl;

    return true;

}

void ActionChangeGroupTime::addActionWhenGroupChanged(Action *a) {
    if (a == nullptr) return;
    mvActionWhenGroupChanged.push_back(dynamic_cast<ActionWhenGroupChangedParent*>(a));
    if (mvActionWhenGroupChanged.back() == nullptr) {
        throw string("ERROR! Need action of type ActionWhenGroupChanged in ActionChangeGroupTime");
    }
}
