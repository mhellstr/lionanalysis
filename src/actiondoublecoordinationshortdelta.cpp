#include "actiondoublecoordinationshortdelta.h"
#include "anglerdf.h"
#include <algorithm>

ActionDoubleCoordinationShortDelta::ActionDoubleCoordinationShortDelta()
:Action(ActionType::ShortDelta, 2, 4, 2, 3), mResolution(0.01), mMaxDelta(2.0), 
mNewGroupL1(nullptr), mNewGroupL2(nullptr), mNewGroupL3(nullptr),
mNewGroupL1CoordinationGroup(ANGLERDF_NEWGROUPCOORDINATION_BOTH),
mNewGroupL2CoordinationGroup(ANGLERDF_NEWGROUPCOORDINATION_BOTH),
mNewGroupL3CoordinationGroup(ANGLERDF_NEWGROUPCOORDINATION_BOTH),
mNewGroupR1(nullptr), mNewGroupR2(nullptr), mNewGroupR3(nullptr),
mNewGroupR1CoordinationGroup(ANGLERDF_NEWGROUPCOORDINATION_BOTH),
mNewGroupR2CoordinationGroup(ANGLERDF_NEWGROUPCOORDINATION_BOTH),
mNewGroupR3CoordinationGroup(ANGLERDF_NEWGROUPCOORDINATION_BOTH),
mpLHSLeft(nullptr), mpLHSMiddle(nullptr),
mpRHSLeft(nullptr), mpRHSMiddle(nullptr),
mpLGroup2MustBe(nullptr), mpLGroup3MustBe(nullptr),
mpRGroup2MustBe(nullptr), mpRGroup3MustBe(nullptr),
mMinDelta(0.0),
mMin13(1), mMax13(5), mResolution13(0.1), mWellAt0(true), mPrintAllDeltas(false), oPrintAllDeltas(nullptr)
{
}

void ActionDoubleCoordinationShortDelta::addGroup(Group *g) {
    if (g != nullptr && dynamic_cast<GroupCoordination*>(g) == nullptr) {
        throw string("Tried to add group without coordination table to ActionDoubleCoordinationShortDelta! : " + g->getId());
    }
    mvpGroups.push_back(g);
    mpLHSLeft = static_cast<GroupCoordination*>(mvpGroups[0]);
    if (mvpGroups.size() > 1) {
        mpLHSMiddle = static_cast<GroupCoordination*>(mvpGroups[1]);
        if (mvpGroups.size() > 2) {
            mpRHSLeft = static_cast<GroupCoordination*>(mvpGroups[2]);
            if (mvpGroups.size() > 3) {
                mpRHSMiddle = static_cast<GroupCoordination*>(mvpGroups[3]);
            }
        }
    }
}

void ActionDoubleCoordinationShortDelta::setPrintAllDeltas(const string &f) {
    addOut(f);
    oPrintAllDeltas = mvpOuts[mvpOuts.size()-1];
    mPrintAllDeltas = true;
    (*oPrintAllDeltas) << "#timestepiteration atom1id atom2id atom3id dist12 dist23 dist13 delta atom2x atom2y atom2z" << endl;
}

void newFindShortDelta(const GroupCoordination* const left, const GroupCoordination* const middle, int inleftid, const Atom* const atom1, const Atom* &atom2, const Atom* &atom3, double &delta, bool &success, const Timestep *t, ofstream *oPrintAllDeltas) {
    double shortr12=1000; int short12id=0;
    double shortr23=1000; int short23id=0;
    bool found12=false, found23=false;
    int iid=atom1->id;
    double mindelta=100000;
    success = false;
    delta=0;
    atom2=nullptr; atom3=nullptr;
    const Group* leftct=left->getCoordinationTable(inleftid);
    for (int i=0; i<leftct->size(); i++) {
        int inmiddleid = middle->getMemberId(leftct->atom(i));
        if (inmiddleid < 0) continue;
        const Atom* middleatom = middle->atom(inmiddleid);
        //cout << "atom1: " << atom1 << " " << atom1->id << "    middle: " << middleatom << " " << middleatom->id << endl;
        double leftdist=atom1->bonds[middleatom->id];
        if (leftdist < shortr12) shortr12 = leftdist;
        const Group* middlect=middle->getCoordinationTable(inmiddleid);
        for (int j=0; j<middlect->size(); j++) {
            const Atom* rightatom = middlect->atom(j);
            if (rightatom == atom1) continue;
            double rightdist=middleatom->bonds[rightatom->id];
            delta=abs(leftdist-rightdist);
            if (oPrintAllDeltas != nullptr) {
                //cout << "HELLO" << endl;
                //cout << oPrintAllDeltas << endl;
                (*oPrintAllDeltas) << t->iteration << " " << atom1->id << " " << middleatom->id << " " << rightatom->id << " " << leftdist << " " << atom1->bonds[rightatom->id] << " " << rightdist <<  " " << delta <<  " " << middleatom->x << " " << middleatom->y << " " << middleatom->z << "\n";
            }
            //cout << atom1->id << " " << middleatom->id << " " << rightatom->id << " " << delta << endl;
            if (delta < mindelta) {
                atom2=middleatom;
                atom3=rightatom;
                success=true;
                mindelta = delta;
            }
        }
    }
    if (success) {
        delta=mindelta;
        //cout << atom1->id << " " << atom2->id << " " << atom3->id << " " << atom1->bonds[atom2->id] << " " << atom2->bonds[atom3->id] << " " << delta << endl;
        //if (shortr12 != atom1->bonds[atom2->id] && delta < 0.2) {
            //cout << "INTERESTING! " << shortr12 << " " << atom1->bonds[atom2->id] << " " << delta << endl;
        //}
    }
}


void ActionDoubleCoordinationShortDelta::internalAction(const vector<Atom>* allatoms, const Timestep *t) {
    if (mCount == 1) {
        mOutLeft.resize(int(mMaxDelta/mResolution)+1);
        mOutRight.resize(int(mMaxDelta/mResolution)+1);
        for (int i=0; i<mOutLeft.size(); i++) {
            mOutLeft[i].resize(int(mMax13/mResolution13)+1);
            mOutRight[i].resize(int(mMax13/mResolution13)+1);
        }
    }
    if (mNewGroupL1 != nullptr) { mNewGroupL1->clear(); }
    if (mNewGroupL2 != nullptr) { mNewGroupL2->clear(); }
    if (mNewGroupL3 != nullptr) { mNewGroupL3->clear(); }
    if (mNewGroupR1 != nullptr) { mNewGroupR1->clear(); }
    if (mNewGroupR2 != nullptr) { mNewGroupR2->clear(); }
    if (mNewGroupR3 != nullptr) { mNewGroupR3->clear(); }
    //cout << mOut.size() << endl;

    const Atom* atom1=nullptr;
    //int iid=atom1->id;
    const Atom* atom2=nullptr;
    const Atom* atom3=nullptr;
    bool success=false; double delta=0;
    for (int i=0; i<mpLHSLeft->size(); i++) {
        // delta and success are passed by reference
        atom1=mpLHSLeft->atom(i);
        newFindShortDelta(mpLHSLeft, mpLHSMiddle, i, atom1, atom2, atom3, delta, success, t, oPrintAllDeltas);
        if (success) {
                //if (mpLGroup3MustBe != nullptr) {
                //cout << "LHS atom3 id: " << atom3->id << " ";
                //for (int j=0; j<mpLGroup3MustBe->size(); j++) {
                //    cout << mpLGroup3MustBe->atom(j)->id << " ";
                //}
                //cout << endl;
                //}
            if (mpLGroup2MustBe != nullptr && ! mpLGroup2MustBe->isMember(atom2)) continue;
            if (mpLGroup3MustBe != nullptr && ! mpLGroup3MustBe->isMember(atom3)) continue;
            addResults(delta, mOutLeft, atom1, atom2, atom3, mNewGroupL1, mNewGroupL2, mNewGroupL3, mNewGroupL1CoordinationGroup, mNewGroupL2CoordinationGroup, mNewGroupL3CoordinationGroup);
        }
    }
    if (mpRHSLeft != nullptr && mpRHSMiddle != nullptr) {
        for (int i=0; i<mpRHSLeft->size(); i++) {
            // delta and success are passed by reference
            atom1=mpRHSLeft->atom(i);
            //cout << atom1 << endl;
            newFindShortDelta(mpRHSLeft, mpRHSMiddle, i, atom1, atom2, atom3, delta, success, t, oPrintAllDeltas);
                //if (mpRGroup3MustBe != nullptr) {
                //cout << "RHS atom3 id: " << atom3->id << " ";
                //for (int j=0; j<mpRGroup3MustBe->size(); j++) {
                //    cout << mpRGroup3MustBe->atom(j)->id << " ";
                //}
                //cout << endl;
                //}
            if (success) {
                if (mpRGroup2MustBe != nullptr && ! mpRGroup2MustBe->isMember(atom2)) continue;
                if (mpRGroup3MustBe != nullptr && ! mpRGroup3MustBe->isMember(atom3)) continue;
                addResults(delta, mOutRight, atom1, atom2, atom3, mNewGroupR1, mNewGroupR2, mNewGroupR3, mNewGroupR1CoordinationGroup, mNewGroupR2CoordinationGroup, mNewGroupR3CoordinationGroup);
            }
        }
    }
    if (mNewGroupL1 != nullptr) { mNewGroupL1->update(nullptr, nullptr); }
    if (mNewGroupL2 != nullptr) { mNewGroupL2->update(nullptr, nullptr); }
    if (mNewGroupL3 != nullptr) { mNewGroupL3->update(nullptr, nullptr); }
    if (mNewGroupR1 != nullptr) { mNewGroupR1->update(nullptr, nullptr); }
    if (mNewGroupR2 != nullptr) { mNewGroupR2->update(nullptr, nullptr); }
    if (mNewGroupR3 != nullptr) { mNewGroupR3->update(nullptr, nullptr); }
}

void ActionDoubleCoordinationShortDelta::addResults(double delta, vector<vector<long long int>>& out, const Atom* const atom1, const Atom* const atom2, const Atom* const atom3, GroupExternal* newgroup1, GroupExternal *newgroup2, GroupExternal *newgroup3, int newgroup1coordinationgroup, int newgroup2coordinationgroup, int newgroup3coordinationgroup) {
    //cout << delta << " " <<  mMaxDelta << " " << mMinDelta << " " <<  atom1->bonds[atom3->id] << " " <<  mMax13 << " " << mMin13 << endl;
    if (delta < mMaxDelta && delta >=mMinDelta && atom1->bonds[atom3->id] < mMax13 && atom1->bonds[atom3->id] >= mMin13 ) {
        int deltabox=int(delta/mResolution);
        int a13box=int(atom1->bonds[atom3->id] / mResolution13);
        out[deltabox][a13box]++;
        //cout << deltabox << " " << a13box << endl;
        //out[box]++;
        if (atom1==nullptr) cout << "atom1 null delta " << delta << endl;
        if (atom2==nullptr) cout << "atom2 null delta " << delta << endl;
        if (atom3==nullptr) cout << "atom3 null delta " << delta << endl;
        if (newgroup1coordinationgroup == ANGLERDF_NEWGROUPCOORDINATION_BOTH) push_into_new_group(newgroup1, atom1, atom2, atom3);
        else if (newgroup1coordinationgroup == 2) push_into_new_group(newgroup1, atom1, atom2);
        else if (newgroup1coordinationgroup == 3) push_into_new_group(newgroup1, atom1, atom3);
        if (newgroup2coordinationgroup == ANGLERDF_NEWGROUPCOORDINATION_BOTH) push_into_new_group(newgroup2, atom2, atom1, atom3);
        else if (newgroup2coordinationgroup == 1) push_into_new_group(newgroup2, atom2, atom1);
        else if (newgroup2coordinationgroup == 3) push_into_new_group(newgroup2, atom2, atom3);
        if (newgroup3coordinationgroup == ANGLERDF_NEWGROUPCOORDINATION_BOTH) push_into_new_group(newgroup3, atom3, atom1, atom2);
        else if (newgroup3coordinationgroup == 1) push_into_new_group(newgroup3, atom3, atom1);
        else if (newgroup3coordinationgroup == 2) push_into_new_group(newgroup3, atom3, atom2);
    }
}

void ActionDoubleCoordinationShortDelta::internalPrint(const vector<Atom>* allatoms, const Timestep *t) {
    mpOut->close();
    mpOut->open(mvOutFilenames[0]);
    mvpOuts[1]->close();
    mvpOuts[1]->open(mvOutFilenames[1]);
    int mindeltaindex = int(mMinDelta/mResolution);
    int maxdeltaindex = int(mMaxDelta/mResolution);
    int min13index = int(mMin13/mResolution13);
    int max13index = int(mMax13/mResolution13);
    double shiftby=0;
    vector<long long int> vSumLeft(mOutLeft.size(), 0);
    vector<long long int> vSumRight(mOutRight.size(), 0);
    for (int i=0; i<mOutLeft.size(); i++) {
        for (int j=0; j<mOutLeft[i].size(); j++) {
            if (mWellAt0) {
                if (mOutLeft[i][j] > shiftby) shiftby=mOutLeft[i][j];
                if (mOutRight[i][j] > shiftby) shiftby=mOutRight[i][j];
            }
            vSumLeft[i]+=mOutLeft[i][j];
            vSumRight[i]+=mOutRight[i][j];
        }
    }
    if (mWellAt0 && shiftby>0) {
        shiftby=log(shiftby);
    }


    // PRINT 1D
    for (int i=vSumLeft.size()-1; i>=0; i--) {
        (*mpOut) << -i*mResolution - mResolution/2.0  << " " << -log(vSumLeft[i]) << " " << vSumLeft[i] << endl;
    }
    if (mpRHSMiddle == nullptr) {
        for (int i=0; i<vSumLeft.size(); i++) {
            (*mpOut) << i*mResolution + mResolution/2.0  << " " << -log(vSumLeft[i]) << " " << vSumLeft[i] << endl;
        }
    }
    else {
        for (int i=0; i<vSumRight.size(); i++) {
            (*mpOut) << i*mResolution + mResolution/2.0  << " " << -log(vSumRight[i]) << " " << vSumRight[i] << endl;
        }
    }

    // PRINT 2D
    //cout << mvpOuts.size() << endl;
    ofstream *out = mvpOuts[1];
    for (int i=maxdeltaindex; i>=0; i--) {
        //cout << i << " " << min13index << " " << max13index << " " << mOutLeft.size() << " " << mMaxDelta << " " << mResolution << " " << mCount << endl;
        //continue;
        for (int j=min13index; j<mOutLeft[i].size() && j<=max13index; j++) {
            (*out) << -i*mResolution - mResolution/2.0 << " " << j*mResolution13 + mResolution13/2.0 << " " << -log(mOutLeft[i][j])+shiftby << " " << mOutLeft[i][j] << "\n";
        }
    }
    if (mpRHSMiddle == nullptr) {
        for (int i=mindeltaindex; i<mOutLeft.size() && i<=maxdeltaindex; i++) {
            for (int j=min13index; j<mOutLeft[i].size() && j<=max13index; j++) {
                (*out) << i*mResolution + mResolution/2.0 << " " << j*mResolution13 + mResolution13/2.0 << " " << -log(mOutLeft[i][j])+shiftby << " " << mOutLeft[i][j] << "\n";
            }
        }
    }
    else {
        for (int i=mindeltaindex; i<mOutRight.size() && i<=maxdeltaindex; i++) {
            for (int j=min13index; j<mOutRight[i].size() && j<=max13index; j++) {
                (*out) << i*mResolution + mResolution/2.0 << " " << j*mResolution13 + mResolution13/2.0 << " " << -log(mOutRight[i][j])+shiftby << " " << mOutRight[i][j] << "\n";
            }
        }
    }


    //for (int i=mOutLeft.size()-1; i>=0; i--) {
    //    for (int j=0; j<mOutLeft[i].size(); j++) {
    //        (*mpOut) << -i*mResolution - mResolution/2.0 << " " << j*mResolution13 + mResolution13/2.0 << " " << -log(mOutLeft[i][j]) << " " << mOutLeft[i][j] << "\n";
    //    }
    //}
    //if (mpRHSMiddle == nullptr) {
    //    for (int i=0; i<mOutLeft.size(); i++) {
    //        for (int j=0; j<mOutLeft[i].size(); j++) {
    //            (*mpOut) << i*mResolution + mResolution/2.0 << " " << j*mResolution13 + mResolution13/2.0 << " " << -log(mOutLeft[i][j]) << " " << mOutLeft[i][j] << "\n";
    //        }
    //    }
    //}
    //else {
    //    for (int i=0; i<mOutRight.size(); i++) {
    //        for (int j=0; j<mOutRight[i].size(); j++) {
    //            (*mpOut) << i*mResolution + mResolution/2.0 << " " << j*mResolution13 + mResolution13/2.0 << " " << -log(mOutRight[i][j]) << " " << mOutRight[i][j] << "\n";
    //        }
    //    }
    //}


    mpOut->close();
    out->close();
}
void ActionDoubleCoordinationShortDelta::internalFinalAction(const vector<Atom>* allatoms, const Timestep *t) {
    internalPrint(allatoms, t);
}


