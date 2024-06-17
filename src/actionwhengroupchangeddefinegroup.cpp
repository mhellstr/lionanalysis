#include "actionwhengroupchangeddefinegroup.h"
#include "groupexternal.h"
#include "actiontype.h"

//there are TWO ways to use this action
//1) it can be called from ChangeGroupTime
//2) if ParentGroup is set in the configuration file, those atoms will be used (with the specified "duration" and "delay")

// the WhenGorupChangedDefineGroup action is defined BEFORE ChangeGroupTime in the input configuration file
// therefore, the internalAction will be called BEFORE any calls to addNewChange() (addNewChange() is called by objects of ActionChangeGroupTime)
// of course, addNewChange will only be called when there is something new to add.

ActionWhenGroupChangedDefineGroup::ActionWhenGroupChangedDefineGroup() 
: ActionWhenGroupChangedParent(ActionType::WhenGroupChangedDefineGroup, 1, 10, 0, 0), mDuration(1), mDelay(0), mParentGroup(nullptr)
{
}

void ActionWhenGroupChangedDefineGroup::addNewChange(const Atom* a, int firstentered, int lastentered, int productentered) {
    //cout << " INSIDE ADDNEWCHANGE " << a << " " << endl;
    //cout << mvpGroups.size() << endl;
    //for (int i=0; i<mvpGroups.size(); i++) {
        //mvpGroups[i]->addAtom(a);
        //cout << mvpGroups[i]->size() << endl;
    //}
    mvToDo.push_back(AtomDurationDelay(a, mDuration, mDelay));
    processSingleToDo(mvToDo.back());
    if (mvToDo.back().duration <= 0) {
        mvToDo.pop_back();
        //cout << "ERASING from addNewChange" << endl;
        //for (int i=0; i<mvpGroups.size(); i++) {
            //cout << "size of group " << i << " is " << mvpGroups[i]->size() << endl;
        //}
    }
}

void ActionWhenGroupChangedDefineGroup::internalAction(const vector<Atom> *allatoms, const Timestep *timestep) {
    for (int i=0; i<mvpGroups.size(); i++) {
        mvpGroups[i]->clear();
    }
    if (mParentGroup!=nullptr) {
        for (int i=0; i<mParentGroup->size(); i++) {
            mvToDo.push_back(AtomDurationDelay(mParentGroup->atom(i), mDuration, mDelay));
        }
    }
    for (auto it=mvToDo.begin(); it!=mvToDo.end(); ) {
        processSingleToDo(*it);
        if (it->duration <= 0) {
            it = mvToDo.erase(it);
            //cout << "ERASING from intenralACtion" << endl;
        }
        else ++it;
    }
}

void ActionWhenGroupChangedDefineGroup::addGroup(Group *g) {
    if (dynamic_cast<GroupExternal*>(g) == nullptr) throw string("Error! ActionWhenGroupChangedDefineGroup requires a group of type GroupExternal!");
    mvpGroups.push_back(g);
}


void ActionWhenGroupChangedDefineGroup::processSingleToDo(AtomDurationDelay &elem) {
    if (elem.delay > 0) {
        elem.delay--;
    }
    else if (elem.duration >= 0) {
        for (int i=0; i<mvpGroups.size(); i++) {
            //cout << "Im adding an ATOM!!!" << endl;
            mvpGroups[i]->addAtom(elem.atom);
        }
        elem.duration--; //the object in the vector needs to be deleted by the calling function
    }
}
