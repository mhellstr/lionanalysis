#include "action.h"
#include <exception>
#include <stdexcept>
#include "formatter.h"
#include <memory>

Action::Action() 
:mCount(0), mEvery(1), mInitialized(false), mType(ActionType::Simple),
mMinGroups(0), mMaxGroups(100), mMinOuts(0), mMaxOuts(100), mMinCount(1), mDynamicCreationOutputFiles(false), mTimeUnit(1), mPrintEvery(0), mpReadRestart(nullptr), mpWriteRestart(nullptr), mWriteRestartFilename(""), mReadRestartFilename(""), mMinTimestepNumber(-5000), mMaxTimestepNumber(1000000000000000000), mFlushEvery(1000), mIfGroupNotEmpty(nullptr), mIfGroupEmpty(nullptr)
{
    //cout << "default constructor " << mMinGroups << " " << mMaxGroups << endl;
}
Action::Action(ActionType type, int ming, int maxg, int mino, int maxo)
: mCount(0), mEvery(1), mInitialized(false), mType(type), mMinGroups(ming), mMaxGroups(maxg), mMinOuts(mino), mMaxOuts(maxo), mMinCount(1), mDynamicCreationOutputFiles(false), mpGroup(nullptr), mpOut(nullptr), mTimeUnit(1), mPrintEvery(0), mpReadRestart(nullptr), mpWriteRestart(nullptr), mWriteRestartFilename(""), mReadRestartFilename(""), mMinTimestepNumber(-5000), mMaxTimestepNumber(1000000000000000000), mFlushEvery(1000), mIfGroupNotEmpty(nullptr), mIfGroupEmpty(nullptr)
{ }

Action::~Action() {
    //cout << "Action::~Action: " << getDescription() << endl;
    if (mDynamicCreationOutputFiles) {
        for (auto &x : mvpOuts) {
            //cout << " Action " << getDescription() << " closing ofstream: " << x << endl;
            x->close();
            delete x;
        }
    }
    if (mpReadRestart != nullptr ) {
        mpReadRestart->close();
        delete mpReadRestart;
    }
    if (mpWriteRestart != nullptr) {
        mpWriteRestart->close();
        delete mpWriteRestart;
    }
}

void Action::closeOutputFiles() {
    if (mDynamicCreationOutputFiles) {
        for (auto &x : mvpOuts) {
            //cout << " Action " << getDescription() << " closing ofstream: " << x << endl;
            if (x->is_open()) x->close();
        }
    }
}

void Action::checkNumGroups() {
    if (mvpGroups.size() < mMinGroups || mvpGroups.size() > mMaxGroups) {
        throw std::runtime_error(Formatter() << "Error in Action: " << getDescription() << "; mvpGroups.size() = " << mvpGroups.size() << " outside allowed range of " << mMinGroups << " to " << mMaxGroups);
    }
}


void Action::action(const vector<Atom> *allatoms, const Timestep *timestep) {
    if (mIfGroupNotEmpty!=nullptr) {
        if (mIfGroupNotEmpty->empty()) return;
    }
    if (mIfGroupEmpty!=nullptr) {
        if (!mIfGroupEmpty->empty()) return;
    }
    if (timestep->totaltimesteps % mEvery == 0) {
        mCount++;
        //cout << "ACTION: " << getDescription() << " mCount = " << mCount << " mMinCount = " << mMinCount << endl;
        if (mCount == 1) {
            if (mvpGroups.size() < mMinGroups || mvpGroups.size() > mMaxGroups) {
                throw std::runtime_error(Formatter() << "ERROR in action: " << mDescription << " mvpGroups.size() = " << mvpGroups.size() << " mMinGroups = " << mMinGroups << " mMaxGroups = " << mMaxGroups);
            }
            if (mvpOuts.size() < mMinOuts || mvpOuts.size() > mMaxOuts) {
                throw std::runtime_error(Formatter() << "ERROR in action: " << mDescription << " mvpOuts.size() = " << mvpOuts.size() << " mMinOuts = " << mMinOuts << " mMaxOuts = " << mMaxOuts);
            }
            internalInitialize(allatoms, timestep);
        }
        //cout << mDescription << endl;
        if (mCount >= mMinCount && timestep->number <= mMaxTimestepNumber && timestep->number >= mMinTimestepNumber) {
            internalAction(allatoms, timestep);
        }
    }
    if (mPrintEvery > 0 && timestep->totaltimesteps % mPrintEvery == 0) {
        internalPrint(allatoms, timestep);
    }
    if (mCount % mFlushEvery == 0) {
        for (int i=0; i<mvpOuts.size(); i++) {
            if (mvpOuts[i]->is_open()) {
                (*mvpOuts[i]) << flush;
            }
        }
    }
}
        
void Action::finalAction(const vector<Atom> *allatoms, const Timestep *timestep) {
    internalFinalAction(allatoms, timestep);
}
        

//void Action::internalInitialize(const vector<Atom>*, const Timestep*) {
//}
//void Action::internalPrint(const vector<Atom>*, const Timestep*) {
//}

void Action::internalAction(const vector<Atom> *atoms, const Timestep *timestep) {
    //cout << "inside Action::internalAction for Actin: " << getDescription() <<  " mvpObjects.size() = " << mvpObjects.size() << endl;
    //the default action is to simply call update() on whatever the member mpObject is (Group, RDF, etc.)
    for (auto & obj : mvpObjects) {
        obj->update(atoms, timestep);
    }
}
//void Action::internalFinalAction(const vector<Atom> *atoms, const Timestep *timestep) {
//}

void Action::updateDescription() {
}

void Action::addOut(ofstream *o) {
    mvpOuts.push_back(o);
    mpOut = mvpOuts[0];
}
void Action::addOut(const string& filename) {
    mDynamicCreationOutputFiles=true;
    mvpOuts.push_back(new ofstream(filename));
    mpOut = mvpOuts.front();
    mvOutFilenames.push_back(filename);
}

void Action::addGroup(Group *g) {
    mvpGroups.push_back(g);
    mpGroup = mvpGroups[0];
}
void Action::addObject(CObject *o) {
    //cout << "pushing back";
    mvpObjects.push_back(o);
    mpObject = mvpObjects[0];
}

void Action::setReadRestart(const string &s) {
    mReadRestartFilename = s;
    mpReadRestart = new ifstream(mReadRestartFilename);
}
void Action::setWriteRestart(const string &s) {
    mWriteRestartFilename = s;
    mpWriteRestart = new ofstream(mWriteRestartFilename);
}
void Action::setPrintEvery(double a, bool realtime) {
    if (realtime) {
        mPrintEvery = int( round(a / mTimeUnit));
    }
    else {
        mPrintEvery = int(a);
    }
}

void Action::setFlushEvery(double a, bool realtime) {
    if (realtime) {
        mFlushEvery = int( round(a / mTimeUnit));
    }
    else {
        mFlushEvery = int(a);
    }
}

//void Action::setRealTime() {
//    int every = int( mEvery / mTimeUnit);
//    if (every==0) every=1;
//    setEvery(every);
//}
