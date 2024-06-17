#include "actionshortdelta.h"
#include "anglerdf.h"
#include <algorithm>

ActionShortDelta::ActionShortDelta()
:Action(ActionType::ShortDelta, 3, 4, 1, 1), mResolution(0.01), mMaxDelta(2.0), 
mNewGroup1(nullptr), mNewGroup2(nullptr), mNewGroup3(nullptr),
mNewGroup1CoordinationGroup(ANGLERDF_NEWGROUPCOORDINATION_BOTH),
mNewGroup2CoordinationGroup(ANGLERDF_NEWGROUPCOORDINATION_BOTH),
mNewGroup3CoordinationGroup(ANGLERDF_NEWGROUPCOORDINATION_BOTH),
mSetDelta(0), mMargin(0), mDesiredWinner(0), mHistoryWinner(0),
mpHistoryGroup(nullptr), mpGroup3MustBe(nullptr), mMinDelta(0.0), mExcludeGroup1CoordinationTable(false)
{
}

void ActionShortDelta::addGroup(Group *g) {
    mvpGroups.push_back(g);
    mpGroup = mvpGroups[0];
    if (mvpGroups.size() > 1) {
        mpMiddle = mvpGroups[1];
        if (mvpGroups.size() > 2) {
            mpEnd = mvpGroups[2];
            if (mvpGroups.size() > 3) {
                mpParentGroup = mvpGroups[3];
            }
        }
    }
}

void findShortDelta(const Atom* const atom1, const Atom* &atom2, const Atom* &atom3, const Group* const first, const Group* const middle, const Group* const end, const Group* const group3mustbe, bool excludegroup1coordinationtable, double &delta, bool &success) {
    double shortr12=1000; int short12id=0;
    double shortr23=1000; int short23id=0;
    bool found12=false, found23=false;
    int iid=atom1->id;
    success = false;
    for (int j=0; j<middle->size(); j++) {
        //cout << j << " ";
        int jid=middle->atom(j)->id;
        double b=atom1->bonds[jid];
        //cout << "bond from "<< iid << " to " << jid << " is " << b << endl;
        if (excludegroup1coordinationtable && dynamic_cast<const GroupCoordination*>(first) != nullptr ) {
            //cout << "TESTING EXLUDE" << endl;
            if (static_cast<const GroupCoordination*>(first)->isMemberOfCoordinationShell(first->getMemberId(atom1), middle->atom(j))) {
                //cout << "atom " << jid << " was in the coordinational shell of " << iid << " internal id " << first->getMemberId(atom1) << endl;
                continue;
            }
        }
        if (b < shortr12 && jid != iid) {
            shortr12=b;
            short12id=j;
            found12=true;
            atom2=middle->atom(j);
            //cout << " FOUND! ";
        }
    }
    if (!found12) { success=false; return; }
    //cout << "shortr12 is " << shortr12 << endl;
    for (int j=0; j<end->size(); j++) {
        //cout << j << " ";
        int jid=end->atom(j)->id;
        double b=atom2->bonds[jid];
        if (b < shortr23 && jid != iid && jid != short12id) {
            shortr23=b;
            short23id=j;
            found23=true;
            atom3=end->atom(j);
            //cout << " FOUND! ";
            //cout << endl;
        }
    }
    if (!found23) { success=false; return; }
    if (group3mustbe != nullptr && ! group3mustbe->isMember(atom3))  { success=false; return; }
    success = true;
    if (atom3 == nullptr) cout << "RETURNING success for aotm3 nullptr" << endl;
    delta = shortr12 - shortr23;
    if (delta <0) delta=-delta;
    //cout << "shortr12 " << shortr12 << " shortr23 " << shortr23 << " " << atom1->id << " " << atom2->id << " " << atom3->id << " " << middle->getId() << " " << end->getId() << " " << delta << " " << atom1->bonds[atom2->id] << " " << atom2->bonds[atom3->id] << endl;

}


void ActionShortDelta::internalAction(const vector<Atom>* allatoms, const Timestep *t) {
    if (mCount == 1) {
        mOut.resize(int(mMaxDelta/mResolution)+1);
        mMemberStart.resize(mpParentGroup->size());
        mMemberEnd.resize(mpParentGroup->size());
        for (int i=0; i<mpParentGroup->size(); i++) {
            mMemberStart[i].resize(mHistoryWinner, false);
            mMemberEnd[i].resize(mHistoryWinner, false);
        }
        if (mpHistoryGroup == nullptr) mpHistoryGroup=mpGroup;
    }
    if (mDesiredWinner > 0) {
        int pos=(mCount - 1) % mHistoryWinner;
        for (int i=0; i<mpParentGroup->size(); i++) {
            mMemberStart[i][pos]=mpHistoryGroup->isMember(mpParentGroup->atom(i));
            mMemberEnd[i][pos]=mpHistoryGroup->isMember(mpParentGroup->atom(i));
        }
        if (mCount <= mHistoryWinner) return;
    }
    if (mNewGroup1 != nullptr) { mNewGroup1->clear(); }
    if (mNewGroup2 != nullptr) { mNewGroup2->clear(); }
    if (mNewGroup3 != nullptr) { mNewGroup3->clear(); }
    //cout << mOut.size() << endl;
    for (int i=0; i<mpGroup->size(); i++) {
        //cout << "i = " << i << " ";
        //double shortr12=1000; int short12id=0;
        //double shortr23=1000; int short23id=0;
        const Atom* const atom1=mpGroup->atom(i);
        //int iid=atom1->id;
        const Atom* atom2=nullptr;
        const Atom* atom3=nullptr;
        bool success=false; double delta=0;
        // delta and success are passed by reference
        bool oldsucces1=success;
        findShortDelta(atom1, atom2, atom3, mpGroup, mpMiddle, mpEnd, mpGroup3MustBe, mExcludeGroup1CoordinationTable, delta, success);
        bool oldsucces2=success;
        //cout << iid << endl;
        //bool found12=false, found23=false;
        //for (int j=0; j<mpMiddle->size(); j++) {
        //    //cout << j << " ";
        //    int jid=mpMiddle->atom(j)->id;
        //    double b=mpGroup->atom(i)->bonds[jid];
        //    //cout << "bond from "<< iid << " to " << jid << " is " << b << endl;
        //    if (b < shortr12 && jid != iid) {
        //        shortr12=b;
        //        short12id=j;
        //        found12=true;
        //        atom2=mpMiddle->atom(j);
        //        //cout << " FOUND! ";
        //    }
        //}
        ////cout << "shortr12 is " << shortr12 << endl;
        //for (int j=0; j<mpEnd->size(); j++) {
        //    //cout << j << " ";
        //    int jid=mpEnd->atom(j)->id;
        //    double b=mpMiddle->atom(short12id)->bonds[jid];
        //    if (b < shortr23 && jid != iid && jid != short12id) {
        //        shortr23=b;
        //        short23id=j;
        //        found23=true;
        //        atom3=mpEnd->atom(j);
        //        //cout << " FOUND! ";
        //        //cout << endl;
        //    }
        //}
        //if (mpGroup3MustBe != nullptr && ! mpGroup3MustBe->isMember(atom3))  {
        //    continue;
        //}
        //cout << "shortr23 is " << shortr23 << endl;
        //if (found12 && found23) {
        if (success) {
            if (mDesiredWinner > 0) {
                int winner=0;
                int sid=mpParentGroup->getMemberId(atom1);
                int eid=mpParentGroup->getMemberId(atom3);
                int memberStartCount = std::count(mMemberStart[sid].begin(), mMemberStart[sid].end(), true);
                int memberEndCount = std::count(mMemberEnd[eid].begin(), mMemberEnd[eid].end(), true);
                int memberDelta = memberStartCount - memberEndCount;
                if (memberDelta > 0 && memberDelta > mMargin) winner=1;
                else if (memberDelta < 0 && memberDelta < -mMargin) winner=3;
                if (winner != mDesiredWinner) continue;
            }
            //double delta = shortr23 - shortr12;

            //if (delta < 0) delta = -delta;
            //cout << "delta is " << delta << endl;
            if (delta < mMaxDelta && delta >=mMinDelta) {
                int box=int(delta/mResolution);
                mOut[box]++;
                if (atom1==nullptr) cout << "atom1 null delta " << delta << endl;;
                if (atom2==nullptr) cout << "atom2 null delta " << delta << endl;;
                if (atom3==nullptr) cout << "atom3 null delta " << delta << " " << oldsucces1 << " " << oldsucces2 << endl;;
                actualSetDelta(1, delta, box, mNewGroup1, atom1);
                actualSetDelta(2, delta, box, mNewGroup2, atom2);
                actualSetDelta(3, delta, box, mNewGroup3, atom3);
                //if (mSetDelta == 2 || mSetDelta == 4) {
                //    if (mSaveExactDelta) const_cast<Atom*>(atom2)->delta = delta;
                //    else const_cast<Atom*>(atom2)->delta = box*mResolution;
                //}
                //if (mSetDelta == 3 || mSetDelta == 4) {
                //    if (mSaveExactDelta) const_cast<Atom*>(atom3)->delta = delta;
                //    else const_cast<Atom*>(atom3)->delta = box*mResolution;
                //}
                if (mNewGroup1CoordinationGroup == ANGLERDF_NEWGROUPCOORDINATION_BOTH) push_into_new_group(mNewGroup1, atom1, atom2, atom3);
                else if (mNewGroup1CoordinationGroup == 2) push_into_new_group(mNewGroup1, atom1, atom2);
                else if (mNewGroup1CoordinationGroup == 3) push_into_new_group(mNewGroup1, atom1, atom3);
                if (mNewGroup2CoordinationGroup == ANGLERDF_NEWGROUPCOORDINATION_BOTH) push_into_new_group(mNewGroup2, atom2, atom1, atom3);
                else if (mNewGroup2CoordinationGroup == 1) push_into_new_group(mNewGroup2, atom2, atom1);
                else if (mNewGroup2CoordinationGroup == 3) push_into_new_group(mNewGroup2, atom2, atom3);
                if (mNewGroup3CoordinationGroup == ANGLERDF_NEWGROUPCOORDINATION_BOTH) push_into_new_group(mNewGroup3, atom3, atom1, atom2);
                else if (mNewGroup3CoordinationGroup == 1) push_into_new_group(mNewGroup3, atom3, atom1);
                else if (mNewGroup3CoordinationGroup == 2) push_into_new_group(mNewGroup3, atom3, atom2);
            }
        }
    }
    if (mNewGroup1 != nullptr) {
        mNewGroup1->update(nullptr, nullptr);
    }
    if (mNewGroup2 != nullptr) {
        mNewGroup2->update(nullptr, nullptr);
    }
    if (mNewGroup3 != nullptr) {
        mNewGroup3->update(nullptr, nullptr);
    }
    //cout << endl;
    //cout << endl;
    //cout << endl;
}


void ActionShortDelta::internalPrint(const vector<Atom>* allatoms, const Timestep *t) {
    mpOut->close();
    mpOut->open(mvOutFilenames[0]);
    long long int sum=mOut[0];
    for (int i=1; i<mOut.size(); i++) {
        sum+=mOut[i];
    }
    if (sum == 0) return;
    for (int i=mOut.size()-1; i>0; i--) {
        (*mpOut) << -i*mResolution  << " " << -log(mOut[i]*1./sum) << " " << mOut[i]*mScaleBy << " " << -log(mOut[i]*mScaleBy) << endl;
    }
    for (int i=0; i<mOut.size(); i++) {
        (*mpOut) << i*mResolution << " " << -log(mOut[i]*1./sum) << " " << mOut[i]*mScaleBy << " " << -log(mOut[i]*mScaleBy) << endl;
    }
    mpOut->close();
}
void ActionShortDelta::internalFinalAction(const vector<Atom>* allatoms, const Timestep *t) {
    internalPrint(allatoms, t);
}


void ActionShortDelta::actualSetDelta(int index, double delta, int box, Group *newgroup, const Atom* atom)
{
    if (mSetDelta == index || mSetDelta == 4) {
        double newdelta=0; bool setdelta=true;
        if (mSaveExactDelta) newdelta=delta;
        else newdelta=box*mResolution;
        if (newgroup != nullptr && newgroup->isMember(atom)) {
            if (newdelta > atom->delta) {
                setdelta=false;
            }
        }
        if (setdelta){
            const_cast<Atom*>(atom)->delta = newdelta;
        }
    }
}


