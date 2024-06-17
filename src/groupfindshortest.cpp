#include "groupfindshortest.h"

GroupFindShortest::GroupFindShortest(string s) 
: GroupCoordination(GROUP_TYPE::FINDSHORTEST, s) //, mMaxDist(-1)
{
}


void GroupFindShortest::internalUpdate(const vector<Atom> *allatoms, const Timestep *t)
{
    clear();
    if (mConstituentGroups.size() != 2) throw string("Error! GroupFindShortest NEEDS 2 constituentgroups!");
    const Group *fromgroup=mConstituentGroups[0]; //these are NOT the new group members
    const Group *togroup = mConstituentGroups[1]; //these ARE the new group members
    for (int i=0; i<fromgroup->size(); i++) {
        double mindist=100000;
        const Atom *froma=fromgroup->atom(i);
        const Atom *mytoa=nullptr;
        for (int j=0; j<togroup->size(); j++) {
            const Atom *toa=togroup->atom(j);
            if (froma->bonds[toa->id] < mindist) {
                mindist=froma->bonds[toa->id];
                //cout << "mindist " << mindist << endl;
                mytoa=toa; //save toatom
            }
        }
        if (mytoa == nullptr) continue;
        int ingroupid=addAtom(mytoa);
        //cout << "ingroupid " << ingroupid << endl;
        addToCoordinationTable(ingroupid, froma, true);
    }
    //cout << "MY SIZE IS " << size() << endl;
}
