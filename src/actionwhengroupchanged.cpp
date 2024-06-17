#include "actionwhengroupchanged.h"
#include "group.h"
#include "groupcoordination.h"
#include <memory>
#include "grouplist.h"
#include "groupbond.h"

ActionWhenGroupChanged::ActionWhenGroupChanged() 
: ActionWhenGroupChangedParent(ActionType::WhenGroupChanged, 1, 2, 1, 2), mBeforeTime(0), mAfterTime(0), mResolution(0), mpProductMustBe(nullptr), mpReactantMustBe(nullptr), mMinProductTime(-1), mWriteSuccessfulOut(false)
{
}

void ActionWhenGroupChanged::internalInitialize(const vector<Atom> *allatoms, const Timestep *timestep) {
    if (mpGroup == nullptr) throw string("ActionWhenGroupChanged::internalInitialize: I need a group!");
    mvSavedData.resize(mpGroup->size());
    mvSum.resize(mpGroup->size());
    mvIsProduct.resize(mpGroup->size());
    mvIsReactant.resize(mpGroup->size());
    for (int i=0; i<mvSavedData.size(); i++) {
        mvSavedData[i].resize(mBeforeTime+mAfterTime+1, 0);
        mvIsProduct[i].resize(mBeforeTime+mAfterTime+1, false);
        mvIsReactant[i].resize(mBeforeTime+mAfterTime+1, false);
    }
    //mMinCount = mBeforeTime;
    mHisto.setMinVal(-mBeforeTime * mTimeUnit);
    mHisto.setMaxVal((mAfterTime+1) * mTimeUnit);
    //if (mResolution == 0) mResolution = mTimeUnit;
    //mHisto.setResolution(mResolution);
    mHisto.setResolution(mTimeUnit);
    //cout << "initialized!" << endl;
}

void ActionWhenGroupChanged::addNewChange(const Atom* a, int firstentered, int lastentered, int productentered) {
    //cout << " INSIDE ADDNEWCHANGE " << endl;
    int id=mpGroup->getMemberId(a);
    if (id <0) throw string("ActionWhenGroupChanged::addNewChange: the atom was not a member of mpGroup!");
    mvToDo.push_back( std::pair<int, int>( id, productentered + mAfterTime ) );
}

void ActionWhenGroupChanged::internalAction(const vector<Atom> *allatoms, const Timestep *timestep) {
    //save the data that you want to save
    int elem = (mCount-1) % mvSavedData[0].size();
    for (int i=0; i<mpGroup->size(); i++) {
        double data=0;
        if (mProp==GROUP_PROPERTY::XYZENVIRONMENT) { // STILL IN DEVELOPMENT CANNOT BE USED
            //mvGroupMembers[i].clear();
            std::unique_ptr<GroupList> g1(new GroupList(""));
            g1->addAtomId(mpGroup->atom(i)->id);
            std::unique_ptr<GroupBond> g2(new GroupBond(""));
            g2->setMaxDist(5);
            g2->setIncludeToGroup(true);
        }
        else {
            switch (mProp) {
                case GROUP_PROPERTY::COORDINATIONNUMBER: data=dynamic_cast<GroupCoordination*>(mpGroup)->getCoordinationNumber(i); break;
                case GROUP_PROPERTY::DELTA: data=mpGroup->atom(i)->delta; break;
                case GROUP_PROPERTY::BONDS: data=dynamic_cast<GroupCoordination*>(mpGroup)->getAverageBond(i); break;
                case GROUP_PROPERTY::BOND0: data=dynamic_cast<GroupCoordination*>(mpGroup)->getShortestBond(i, 0); break;
                case GROUP_PROPERTY::BOND1: data=dynamic_cast<GroupCoordination*>(mpGroup)->getShortestBond(i, 1); break;
                case GROUP_PROPERTY::BOND2: data=dynamic_cast<GroupCoordination*>(mpGroup)->getShortestBond(i, 2); break;
                case GROUP_PROPERTY::BOND3: data=dynamic_cast<GroupCoordination*>(mpGroup)->getShortestBond(i, 3); break;

                    //{
                    //    int ctsize=dynamic_cast<GroupCoordination*>(mpGroup)->getCoordinationTable(i)->size();
                    //    if (ctsize == 0) break;
                    //    //cout << ctsize << endl;
                    //    for (int j=0; j<ctsize; j++) {
                    //        const Atom *a1=mpGroup->atom(i);
                    //        const Atom *a2=dynamic_cast<GroupCoordination*>(mpGroup)->getCoordinationTable(i)->atom(j);
                    //        data+=a1->bonds[a2->id];
                    //        //cout << "the bond between atom " << a1->id << " " << a1->type << " and atom " << a2->id << " " << a2->type << " is " << a1->bonds[a2->id] << endl;
                    //        //cout << data << " " << i << " " << j << endl;
                    //    }
                    //    data = data* 1.0/ctsize;
                    //    //cout << " data " << data << " size " << ctsize << endl;
                    //}
                    //break;
                default:
                    throw string("Unimplemented property for WhenGroupChanged");
            }
            //if (i == 0) cout << "updating sum mCount " << mCount << " i= " << i << " elem = " << elem <<  " data = " << data << " " << mvSum.size() << " " << mvSavedData.size() << " " << mvSavedData[i].size() << " " << mpGroup->size() << endl;
            mvSum[i] += data - mvSavedData[i][elem];
            mvSavedData[i][elem] = data;
            //if (data >5) cout << "saving data " << data << " at elem " << elem << " for atom " << mpGroup->atom(i)->id << " " << endl;
        }
        if (mpReactantMustBe != nullptr) mvIsReactant[i][elem] = mpReactantMustBe->isMember(mpGroup->atom(i));
        if (mpProductMustBe != nullptr) mvIsProduct[i][elem] = mpProductMustBe->isMember(mpGroup->atom(i));
    }
    //cout << "before checktodolist" << allatoms->size() << " " << mpGroup->size() << endl;
    if (mCount > mBeforeTime+mAfterTime+1) {
        checkToDoList(timestep);
    }
}

void ActionWhenGroupChanged::checkToDoList(const Timestep *timestep) {
    for (auto iter = mvToDo.begin(); iter != mvToDo.end();) {
        //iter->first is the atom id within the group
        //iter->second is the timestep at which the stuff should be printed
        //cout << "looping todo" << endl;
        if (timestep->iteration == iter->second) { //should only be ==. IN THE PAST, I used >= "to be safe" but that introduces BUGS, because if aftertime is large you will do multiple ones from the "todo" list as soon as timstep->iteration >= iter-.second, and the data is not correct. Only use == !!!!
            //cout << " DOING atom " << mpGroup->atom(iter->first)->id << " at timestep " << timestep->iteration << endl;
            bool readytogo=true;
            if (mProp == GROUP_PROPERTY::BONDS || mProp == GROUP_PROPERTY::BOND0 || mProp == GROUP_PROPERTY::BOND1 || mProp == GROUP_PROPERTY::BOND2 || mProp == GROUP_PROPERTY::BOND3 ) {
                //check that there are no zeroes in the saved data, since this will mess HEAVILY with the average bond
                for (int i=0; i<mvSavedData[iter->first].size(); i++) {
                    if (mvSavedData[iter->first][i] == 0) {
                        readytogo=false;
                        //cout << " SKIPPING atom " << mpGroup->atom(iter->first)->id << " at timestep " << timestep->iteration << endl;
                        break;
                    }
                }
            }
            if (readytogo) {
                if (mpProductMustBe != nullptr) {
                    if (mMinProductTime > 0) {
                        int sum=0;
                        for (int i=0; i<mvIsProduct[iter->first].size(); i++) {
                            sum+=mvIsProduct[iter->first][i];
                        }
                        if (sum < mMinProductTime) readytogo=false;
                    }
                }
            }
            if (readytogo) {
                if (mWriteSuccessfulOut) {
                    (*mvpOuts[1]) << mpGroup->atom(iter->first)->id << " " << iter->second - mAfterTime << endl;
                }
                int finalelem = (mCount -1) % mvSavedData[iter->first].size();
                //double t = mAfterTime*mTimeUnit;
                int histointernalindex=-1;
                for (int i=finalelem+1; i<mvSavedData[iter->first].size(); i++) {
                    histointernalindex++;
                    //mHisto.add(-mBeforeTime*mTimeUnit + histointernalindex*mTimeUnit, mvSavedData[iter->first][i]);
                    if (mpProductMustBe != nullptr && histointernalindex > mBeforeTime+1 && !mvIsProduct[iter->first][i]) {
                        continue;
                    }
                    if (mpReactantMustBe != nullptr && histointernalindex < mBeforeTime/2 && !mvIsReactant[iter->first][i]) {
                        continue;
                    }
                    mHisto.addToInternalIndex(histointernalindex, mvSavedData[iter->first][i]);
                    //if (mvSavedData[iter->first][i] > 5) {
                    //    cout << " atom " << mpGroup->atom(iter->first)->id << " at timestep " << timestep->iteration << " histointernalindex = " << histointernalindex << " data = " << mvSavedData[iter->first][i] << endl;
                    //}
                    //cout << "AFTERTIME" << mAfterTime << " adding " << mvSavedData[iter->first][i] << " (atom " << mpGroup->atom(iter->first)->id << ") at timstep = " << timestep->iteration << " to histointernalindex " << histointernalindex << endl;
                }
                for (int i=0; i<=finalelem; i++) {
                    histointernalindex++;
                    //mHisto.add(-mBeforeTime*mTimeUnit + histointernalindex*mTimeUnit, mvSavedData[iter->first][i]);
                    if (mpProductMustBe != nullptr && histointernalindex > mBeforeTime+1 && !mvIsProduct[iter->first][i]) {
                        continue;
                    }
                    if (mpReactantMustBe != nullptr && histointernalindex < mBeforeTime/2 && !mvIsReactant[iter->first][i]) {
                        continue;
                    }
                    mHisto.addToInternalIndex(histointernalindex, mvSavedData[iter->first][i]);
                    //cout << "AFTERTIME" << mAfterTime << " adding " << mvSavedData[iter->first][i] << " (atom " << mpGroup->atom(iter->first)->id << ") at timstep = " << timestep->iteration << " to histointernalindex " << histointernalindex << endl;
                }
            }
            iter = mvToDo.erase(iter);
            //int i=finalelem;
            //for (double t = mAfterTime*mTimeUnit; t >= -mBeforeTime * mTimeUnit; t-=mResolution) {
            //    mHisto.add(t, mvSavedData[iter->first][i]);
            //    --i;
            //    if (i < 0) i = mpGroup->size()-1;
            //}
        }
        else {
            ++iter;
        }
    }
    //cout << " after checktodolist" << endl;
}
void ActionWhenGroupChanged::internalPrint(const vector<Atom> *allatoms, const Timestep *timestep) {
    //cout << " INTERNAL PRINT " << endl;
    mpOut->close();
    mpOut->open(mvOutFilenames.front());
    (*mpOut) << "# " << getDescription() << "\n";
    long long int sum=mHisto.sum();
    long long int sumcount=mHisto.sumCount();
    //(*mpOut) << "#value #count #fractional #divbycount " + getDescription() + "# mCount = " << mCount << " # sum = " << sum << " # sumcount = " << sumcount << "\n";
    (*mpOut) << "#value #divbycount #count #fractional " + getDescription() + "# mCount = " << mCount << " # sum = " << sum << " # sumcount = " << sumcount << "\n";
    for (int i=0; i<mHisto.size(); i++) {
        (*mpOut) << mHisto.indexToReal(i) << " " << mHisto.getElement(i) * 1.0f / mHisto.getCount(i) << " " << mHisto.getElement(i) << " " << mHisto.getElement(i) * 1.0f / sum << "\n";
    }
    (*mpOut) << flush;
    mpOut->close();
}

void ActionWhenGroupChanged::internalFinalAction(const vector<Atom> *allatoms, const Timestep *timestep) {
    internalPrint(allatoms, timestep);
}

void ActionWhenGroupChanged::setBeforeTime(double a, bool realtime) {
    if (!realtime) { mBeforeTime = int(a); }
    else { mBeforeTime = int( round(a / mTimeUnit) ); }
}
void ActionWhenGroupChanged::setAfterTime(double a, bool realtime) {
    if (!realtime) { mAfterTime = int(a); }
    else { mAfterTime = int( round(a / mTimeUnit) ); }
}

void ActionWhenGroupChanged::addGroup(Group *g) {
    mvpGroups.push_back(g);
    mpGroup=mvpGroups[0];
    if (mvpGroups.size() > 1) {
        mpProductMustBe = mvpGroups[1];
    }
}

