#include "anglerdf.h"
#include "actionanglerdf.h"
#include "utility.h"

AngleRDF::AngleRDF()
: mGroup1(nullptr), mGroup2(nullptr), mGroup3(nullptr),
mGroup1MinDist(0.0), mGroup1MaxDist(1.0), mGroup3MinDist(0.0), mGroup3MaxDist(1.0),
mAngleResolution(1.0), mDistanceResolution(0.1), mMinAngle(0.0), mMaxAngle(180.0),
mNewGroup1(nullptr), mNewGroup2(nullptr), mNewGroup3(nullptr),
mGroup13MinDist(0.0), mGroup13MaxDist(0.0),
mMinDist23Frac12(-1), 
mMinDist13Frac12(-1),
mMinDist13Frac23(-1),
mMaxDist23Frac12(-1),
mMaxDist13Frac12(-1),
mMaxDist13Frac23(-1),
mMinHits12(0), mMaxHits12(0),
mUnique12(false),
mNewGroup1CoordinationGroup(ANGLERDF_NEWGROUPCOORDINATION_BOTH),
mNewGroup2CoordinationGroup(ANGLERDF_NEWGROUPCOORDINATION_BOTH),
mNewGroup3CoordinationGroup(ANGLERDF_NEWGROUPCOORDINATION_BOTH),
mAngleType(AngleType::XYZ),
mNegateMustBeInCoordinationShell(false),
mCount(0),
mGroup1Type(AngleRDFGroupType::Real),
mGroup3Type(AngleRDFGroupType::Real),
mSignedAngle(false)
{
    mGroupMustBeInCoordinationShellOf.resize(9, false);
}

void AngleRDF::setGroupMustBeInCoordinationShellOf(int group, int coordinationshellofgroup, bool a) {
    //0: group1 must be in coordination shell of group 1 (makes no sense)
    //1: group1 must be in coordination shell of group 2
    //2: group1 must be in coordination shell of group 3
    //3: group2 must be in coordination shell of group 1
    //9: group3 must be in coordination shell of group 3 (makes no sense)
    int index = (group-1)*3 + (coordinationshellofgroup-1);
    if (index >= mGroupMustBeInCoordinationShellOf.size()) {
        throw string("ERROR! setGroupMustBeInCoordinationShellOf, unallowed combination of values");
    }
    //cout << "setgmicso " << group << " " << coordinationshellofgroup << " " << a << " " << index << endl;
    mGroupMustBeInCoordinationShellOf[index]=true;
}

bool AngleRDF::groupMustBeInCoordinationShellOf(int group, int coordinationshellofgroup) {
    int index = (group-1)*3 + (coordinationshellofgroup-1);
    if (index >= mGroupMustBeInCoordinationShellOf.size()) {
        throw string("ERROR! msutGroupMustBeInCoordinationShellOf, unallowed combination of values");
    }
    return mGroupMustBeInCoordinationShellOf[index];
}

bool AngleRDF::checkCoordinationShell(int g1, int g2, int indexing2, const Atom *a)
{
    Group *gg2=nullptr;
    //cout << g1 << " " << g2 << " " << indexing2 << " " << a->id << " " << groupMustBeInCoordinationShellOf(g1,g2) << endl;
    if (groupMustBeInCoordinationShellOf(g1,g2)) {
        if (g2 == 1) gg2=mGroup1;
        else if (g2==2) gg2=mGroup2;
        else if (g2==3) gg2=mGroup3;
        bool ismember=dynamic_cast<GroupCoordination*>(gg2)->isMemberOfCoordinationShell(indexing2, a);
        if (mNegateMustBeInCoordinationShell) {
            //cout << "negate: returning " << !ismember <<endl;
            return !ismember;
        }
        else {
            //cout << "no negate: returning " << ismember << endl;
            return ismember;
        }
    }
    return true;
}

void AngleRDF::initializeArrays() {
  int numdistanceelements1=int((mGroup1MaxDist-mGroup1MinDist)/mDistanceResolution); // + 1;
  int numdistanceelements3=int((mGroup3MaxDist-mGroup3MinDist)/mDistanceResolution); // + 1;
  int numdistanceelements13=int((mGroup13MaxDist-mGroup13MinDist)/mDistanceResolution);
  mDistances1.resize(numdistanceelements1);
  mDistances3.resize(numdistanceelements3);
  mDistances13.resize(numdistanceelements13);
  mDistanceDistance.resize(numdistanceelements1);
  int numangleelements=int((mMaxAngle-mMinAngle)/mAngleResolution); // + 1;
  //mNumAngleElements = numangleelements;
  for (int i=0; i<mDistances1.size(); i++) {
    mDistances1[i].resize(numangleelements, 0);
    mDistanceDistance[i].resize(numdistanceelements3, 0);
  }
  for (int i=0; i<mDistances3.size(); i++) {
    mDistances3[i].resize(numangleelements, 0);
  }
  for (int i=0; i<mDistances13.size(); i++) {
    mDistances13[i].resize(numangleelements, 0);
  }
}


void AngleRDF::setGroupDummyVector(int groupnumber, float a, float b, float c) {
    if (groupnumber == 1) {
        mGroup1DummyVector.clear();
        mGroup1DummyVector.push_back(a);
        mGroup1DummyVector.push_back(b);
        mGroup1DummyVector.push_back(c);
        mGroup1Type = AngleRDFGroupType::DummyVector;
    }
    else if (groupnumber == 3) {
        mGroup3DummyVector.clear();
        mGroup3DummyVector.push_back(a);
        mGroup3DummyVector.push_back(b);
        mGroup3DummyVector.push_back(c);
        mGroup3Type = AngleRDFGroupType::DummyVector;
    }
    else throw string("groupnumber in AngleRDF::setGroupDummyVector must be 1 or 3");
}


void AngleRDF::setGroupCoordinationShellCenterOfMass(int gid) {
    if (dynamic_cast<GroupCoordination*>(mGroup2) == nullptr) {
        throw string("Group2 must be of type GroupCoordination! (for coordinationshellcenterofmass)");
    }
    if (gid == 1) {
        mGroup1Type = AngleRDFGroupType::CoordinationShellCenterOfMass;
    }
    else if (gid == 3) {
        mGroup3Type = AngleRDFGroupType::CoordinationShellCenterOfMass;
    }
    else throw string("setGroupCoordinationShellCenterOfMass only for groups 1 or 3");
}

void AngleRDF::update(const vector<Atom> *allatoms, const Timestep *t) {
    mCount++;
    if (mCount == 1) {
        initializeArrays();
    }
    //cout <<  " mNewGroup3  is " << mNewGroup3 <<  " and itscoordination group is " << mNewGroup3CoordinationGroup << endl;
    if (mNewGroup1 != nullptr) { mNewGroup1->clear(); }
    if (mNewGroup2 != nullptr) { mNewGroup2->clear(); }
    if (mNewGroup3 != nullptr) { mNewGroup3->clear(); }
    if (mCopyGroup1) { mNewGroup1->setAtomsFromExternalSource(mGroup1); }
    if (mCopyGroup2) { mNewGroup2->setAtomsFromExternalSource(mGroup2); }
    if (mCopyGroup3) { mNewGroup3->setAtomsFromExternalSource(mGroup3); }
    if (mMinHits12 > 0 || mMaxHits12 > 0) {
        mHits12.clear();
        mHits12.resize(mGroup2->size());
        for (int i=0; i<mHits12.size(); i++) mHits12[i].resize(mGroup1->size());
    }
    if (mDistances1.size() == 0) return;
    // group 2 is a "real group"
    // group 1 and group 3 can be pseudogroups
    // for example, dumm yvectors (with respect to group 2), or center of masses (of the coordination shell of group 2)

    GroupExternal *myGroup1=nullptr;
    GroupExternal *myGroup3=nullptr;
    if (mGroup1Type == AngleRDFGroupType::Real)  { myGroup1 = static_cast<GroupExternal*>(mGroup1); }
    else if (mGroup1Type == AngleRDFGroupType::DummyVector) { myGroup1 = new GroupExternal; makeGroupDummyVector(myGroup1, mGroup1DummyVector, t); }
    else if (mGroup1Type == AngleRDFGroupType::CoordinationShellCenterOfMass) { myGroup1 = new GroupExternal; makeGroupCoordinationShellCenterOfMass(myGroup1, t); }
    if (mGroup3Type == AngleRDFGroupType::Real)  { myGroup3 = static_cast<GroupExternal*>(mGroup3); }
    else if (mGroup3Type == AngleRDFGroupType::DummyVector) { myGroup3 = new GroupExternal; makeGroupDummyVector(myGroup3, mGroup3DummyVector, t); }
    else if (mGroup3Type == AngleRDFGroupType::CoordinationShellCenterOfMass) {myGroup3 = new GroupExternal; makeGroupCoordinationShellCenterOfMass(myGroup3, t); }
    //cout << "ANGLERDF " << mCount << " " << mGroup1 << " " << mGroup2 << " " << mGroup3 << endl;
    for (int i=0; i<mGroup2->size(); i++) { //loop through center group atoms
        const Atom *atom2 = mGroup2->atom(i);
        for (int j=0; j<myGroup1->size(); j++) { //find atoms in group 1 that obey distance condition
            const Atom *atom1 = myGroup1->atom(j);
            if (mGroup1Type == AngleRDFGroupType::Real && atom2->id == atom1->id) continue;
            float bond1=0;
            if (mGroup1Type == AngleRDFGroupType::Real) {
                bond1=atom2->bonds[atom1->id];
            }
            else {
                bond1=distance(atom1->x, atom1->y, atom1->z, atom2->x, atom2->y, atom2->z, t);
            }
            if (mGroup1Type == AngleRDFGroupType::Real && !checkCoordinationShell(1,2,i,atom1)) continue;
            if (mGroup1Type == AngleRDFGroupType::Real && !checkCoordinationShell(2,1,j,atom2)) continue;
            //if (groupMustBeInCoordinationShellOf(1,2) && ! dynamic_cast<GroupCoordination*>(mGroup2)->isMemberOfCoordinationShell(i, atom1)) continue;
            //if (groupMustBeInCoordinationShellOf(2,1) && ! dynamic_cast<GroupCoordination*>(mGroup1)->isMemberOfCoordinationShell(j, atom2)) continue;
            if (mMaxHits12 > 0 && mHits12[i][j] >= mMaxHits12) continue;
            if (mUnique12) {
                // check that atom1 has not yet been part of group2
                bool foundmatch=false;
                for (int k=0; k<=i; k++) {
                    if (mGroup2->atom(k)->id == atom1->id) {
                        foundmatch=true;
                        break;
                    }
                }
                if (foundmatch) continue;
            }
            //cout << "bond1 = " << bond1 << " " << mGroup1MinDist << " " << mGroup1MaxDist << endl;;
            if (bond1 >= mGroup1MinDist && bond1 < mGroup1MaxDist) {
                //if (mGroup3DummyVector.empty()) {
                if (true) {
                    //cout << "in the loop " << endl;
                    for (int k=0; k<myGroup3->size(); k++) { //find atoms in group 3 that obey distance condition
                        const Atom *atom3 = myGroup3->atom(k);
                        if (mGroup3Type == AngleRDFGroupType::Real && (atom2->id == atom3->id || atom1->id == atom3->id)) continue;
                        if (mGroup1Type == AngleRDFGroupType::Real && mGroup3Type == AngleRDFGroupType::Real && !checkCoordinationShell(1,3,k,atom1)) continue;
                        if (mGroup3Type == AngleRDFGroupType::Real && !checkCoordinationShell(2,3,k,atom2)) continue;
                        if (mGroup1Type == AngleRDFGroupType::Real && mGroup3Type == AngleRDFGroupType::Real && !checkCoordinationShell(3,1,j,atom3)) continue;
                        if (mGroup3Type == AngleRDFGroupType::Real && !checkCoordinationShell(3,2,i,atom3)) {
                            if (mCopyGroup3) {
                                if (mNegateMustBeInCoordinationShell) {
                                    mNewGroup3->delAtom(atom3);
                                }
                            }
                            continue;
                        }
                        //if (groupMustBeInCoordinationShellOf(1,3) && ! dynamic_cast<GroupCoordination*>(mGroup3)->isMemberOfCoordinationShell(k, atom1)) continue;
                        //if (groupMustBeInCoordinationShellOf(2,3) && ! dynamic_cast<GroupCoordination*>(mGroup3)->isMemberOfCoordinationShell(k, atom2)) continue;
                        //if (groupMustBeInCoordinationShellOf(3,1) && ! dynamic_cast<GroupCoordination*>(mGroup1)->isMemberOfCoordinationShell(j, atom3)) continue;
                        //if (groupMustBeInCoordinationShellOf(3,2) && ! dynamic_cast<GroupCoordination*>(mGroup2)->isMemberOfCoordinationShell(i, atom3)) continue;
                        //const float bond3=atom2->bonds[atom3->id];
                        float bond3=0;
                        if (mGroup3Type == AngleRDFGroupType::Real) {
                            bond3=atom2->bonds[atom3->id];
                        }
                        else {
                            bond3=distance(atom3->x, atom3->y, atom3->z, atom2->x, atom2->y, atom2->z, t);
                        }
                        //cout << " bond3 = " << bond3 << " " << mGroup3MinDist << " " << mGroup3MaxDist;
                        if (bond3 >= getMinDist23(bond1) && bond3 < getMaxDist23(bond1)) { //>= minvalue but < maxvalue... it is crucial that bond3!=mGroup3MaxDist since that would give an overflow error. It actually happened once!
                            //const float bond13 = atom1->bonds[atom3->id];
                            float bond13 = 0;
                            if (mGroup3Type == AngleRDFGroupType::Real && mGroup1Type == AngleRDFGroupType::Real) {
                                bond13=atom1->bonds[atom3->id];
                            }
                            else {
                                bond13=distance(atom1->x, atom1->y, atom1->z, atom3->x, atom3->y, atom3->z, t);
                            }
                            //cout << "bond13 is " << bond13 << endl;
                            //cout << "getmindtist = " << getMaxDist13(bond1,bond3) << endl;
                            if (bond13 >= getMinDist13(bond1,bond3) && bond13 < getMaxDist13(bond1,bond3)) {
                                float ang=0;
                                if (mSignedAngle) {
                                    switch (mAngleType) {
                                        case AngleType::XYZ: ang=angle(atom1, atom2, atom3, t); break;
                                        //case AngleType::XY: ang=signedangle(atom1->x, atom1->y, atom2->x, atom2->y, atom3->x, atom3->y, t); break;
                                        //case AngleType::YZ: ang=signedangle(atom1->y, atom1->z, atom2->y, atom2->z, atom3->y, atom3->z, t); break;
                                        //case AngleType::XZ: ang=signedangle(atom1->x, atom1->z, atom2->x, atom2->z, atom3->x, atom3->z, t); break;
                                        //case AngleType::ZX: ang=signedangle(atom1->z, atom1->x, atom2->z, atom2->x, atom3->z, atom3->x, t); break;
                                        default: ang=signedangle(atom1->x, atom1->y, atom1->z, atom2->x, atom2->y, atom2->z, atom3->x, atom3->y, atom3->z, mAngleType, t); break;
                                    }
                                }
                                else {
                                    switch (mAngleType) {
                                        case AngleType::XYZ: ang=angle(atom1, atom2, atom3, t); break;
                                        case AngleType::XY: ang=angle(atom1->x, atom1->y, 0., atom2->x, atom2->y, 0., atom3->x, atom3->y, 0., t); break;
                                        case AngleType::YZ: ang=angle(0., atom1->y, atom1->z, 0., atom2->y, atom2->z, 0., atom3->y, atom3->z, t); break;
                                        case AngleType::XZ: ang=angle(atom1->x, 0., atom1->z, atom2->x, 0., atom2->z, atom3->x, 0., atom3->z, t); break;
                                        case AngleType::ZX: ang=angle(atom1->x, 0., atom1->z, atom2->x, 0., atom2->z, atom3->x, 0., atom3->z, t); break;
                                    }
                                }
                                //cout << " angle = " << ang;
                                if (ang >= mMinAngle && ang < mMaxAngle) {
                                    const int bond1bin = int ( mDistances1.size()*(bond1-mGroup1MinDist)/(mGroup1MaxDist-mGroup1MinDist) );
                                    const int bond3bin = int ( mDistances3.size()*(bond3-mGroup3MinDist)/(mGroup3MaxDist-mGroup3MinDist) );
                                    const int bond13bin = int ( mDistances13.size()*(bond13-mGroup13MinDist)/(mGroup13MaxDist-mGroup13MinDist) );
                                    int anglebin = int ( mDistances1[0].size()*(ang-mMinAngle)/(mMaxAngle-mMinAngle) );
                                    //if (anglebin > 0 && ang < 0) anglebin+=0;
                                    //cout << anglebin << " " << ang << "\n";
                                    //cout << atom1->id << " " << atom2->id << " " << atom3->id << " " << ang << " " << bond1 << " " << bond3 << " " << bond13 << endl;
                                    //if (mSignedAngle && ang > 0 && ang < 10*mAngleResolution && anglebin!=0) cout << "positive " << ang << " put into bin " << anglebin << " " << mDistances1[0].size() << " " << mMinAngle << " " << mMaxAngle << endl;
                                    //if (mSignedAngle && ang < 0 && ang > 10*-mAngleResolution && anglebin!=0) cout << "negative " << ang << " put into bin " << anglebin << endl;
                                    mDistances1[bond1bin][anglebin]++;
                                    mDistances3[bond3bin][anglebin]++;
                                    mDistances13[bond13bin][anglebin]++;
                                    mDistanceDistance[bond1bin][bond3bin]++;
                                    if (mMinHits12 > 0 || mMaxHits12 > 0) {
                                        mHits12[i][j] += 1;
                                        if (mMinHits12 > 0 && mHits12[i][j] < mMinHits12) continue;
                                    }
                                    // first argument is the atom that gets pushed, the remaining two are added to coordination tables
                                    //cout << mNewGroup3 << " mNEW3COOGR " << mNewGroup3CoordinationGroup << endl;
                                    if (mNewGroup1CoordinationGroup == ANGLERDF_NEWGROUPCOORDINATION_BOTH) push_into_new_group(mNewGroup1, atom1, atom2, atom3);
                                    else if (mNewGroup1CoordinationGroup == 2) push_into_new_group(mNewGroup1, atom1, atom2);
                                    else if (mNewGroup1CoordinationGroup == 3) push_into_new_group(mNewGroup1, atom1, atom3);
                                    if (mNewGroup2CoordinationGroup == ANGLERDF_NEWGROUPCOORDINATION_BOTH) push_into_new_group(mNewGroup2, atom2, atom1, atom3);
                                    else if (mNewGroup2CoordinationGroup == 1) push_into_new_group(mNewGroup2, atom2, atom1);
                                    else if (mNewGroup2CoordinationGroup == 3) push_into_new_group(mNewGroup2, atom2, atom3);
                                    if (mNewGroup3CoordinationGroup == ANGLERDF_NEWGROUPCOORDINATION_BOTH) push_into_new_group(mNewGroup3, atom3, atom1, atom2);
                                    else if (mNewGroup3CoordinationGroup == 1) push_into_new_group(mNewGroup3, atom3, atom1);
                                    else if (mNewGroup3CoordinationGroup == 2) push_into_new_group(mNewGroup3, atom3, atom2);

                                    //push_into_new_group(mNewGroup2, atom2, atom1, atom3);
                                    //push_into_new_group(mNewGroup3, atom3, atom1, atom2); 
                                } //angle condition
                            } //bond13 condition
                        } //bond3 condition
                    } //group3 loop
                }
                /*
                else { // if mGroup3DummyVector.empty()
                    //we specified a group3 dummy vector
                    const float dummyx=atom2->x+mGroup3DummyVector[0];
                    const float dummyy=atom2->y+mGroup3DummyVector[1];
                    const float dummyz=atom2->z+mGroup3DummyVector[2];
                    const float bond3= distance(atom2->x, atom2->y, atom2->z, dummyx, dummyy, dummyz, t);
                    const float bond13= distance(atom1->x, atom1->y, atom1->z, dummyx, dummyy, dummyz, t);
                    float ang=0;
                    ang=angle(atom1->x, atom1->y, atom1->z, atom2->x, atom2->y, atom2->z, dummyx, dummyy, dummyz, t); 
                                if (ang >= mMinAngle && ang < mMaxAngle) {
                                    const int bond1bin = int ( mDistances1.size()*(bond1-mGroup1MinDist)/(mGroup1MaxDist-mGroup1MinDist) );
                                    const int bond3bin = int ( mDistances3.size()*(bond3-mGroup3MinDist)/(mGroup3MaxDist-mGroup3MinDist) );
                                    const int bond13bin = int ( mDistances13.size()*(bond13-mGroup13MinDist)/(mGroup13MaxDist-mGroup13MinDist) );
                                    const int anglebin = int ( mDistances1[0].size()*(ang-mMinAngle)/(mMaxAngle-mMinAngle) );
                                    //cout << atom1->id << " " << atom2->id << " " << atom3->id << " " << ang << " " << bond1 << " " << bond3 << " " << bond13 << endl;
                                    mDistances1[bond1bin][anglebin]++;
                                    mDistances3[bond3bin][anglebin]++;
                                    mDistances13[bond13bin][anglebin]++;
                                    mDistanceDistance[bond1bin][bond3bin]++;
                                    if (mMinHits12 > 0) {
                                        mHits12[i][j] += 1;
                                        if (mHits12[i][j] < mMinHits12) continue;
                                    }
                                    // first argument is the atom that gets pushed, the remaining two are added to coordination tables

                                    // (mNewGroup1CoordinationGroup == 2) 
                                    push_into_new_group(mNewGroup1, atom1, atom2);
                                    // (mNewGroup2CoordinationGroup == 1) 
                                    push_into_new_group(mNewGroup2, atom2, atom1);

                                    //push_into_new_group(mNewGroup2, atom2, atom1, atom3);
                                    //push_into_new_group(mNewGroup3, atom3, atom1, atom2); 
                                } //angle condition
                } //else (if mGroup3DummyVector.empty())
                */
            } //bond1 condition
            //cout << endl;
        } //group1 loop
    } //group2 loop
    if (mNewGroup1 != nullptr) {
        mNewGroup1->update(nullptr, nullptr);
    }
    if (mNewGroup2 != nullptr) {
        mNewGroup2->update(nullptr, nullptr);
    }
    if (mNewGroup3 != nullptr) {
        mNewGroup3->update(nullptr, nullptr);
    }
    if (mGroup1Type != AngleRDFGroupType::Real) cleanUpPseudoGroup(myGroup1);
    if (mGroup3Type != AngleRDFGroupType::Real) cleanUpPseudoGroup(myGroup3);
    //if (mGroup!=nullptr) cout << "size of group " << mGroup->mId << " is now " << mGroup->size() << endl;
} //incrdf()

/*
void AngleRDF::push_into_new_group(Group *g, Atom *a) {
    if (g != nullptr) { 
        int ingroupid=g->getMemberId(a);
        if (ingroupid >= 0) g->mCoordinationNumbers[ingroupid]++; //atom already exists in the group
        else { //the atom was not a member of the group
            g->mAtoms.push_back(a);
            g->mCoordinationNumbers.push_back(1);
        }
    } 
}
*/
void push_into_new_group(GroupExternal *g, const Atom *a, const Atom *b, const Atom *c) {
    if (g != nullptr) { 
        //cout << g << endl;
        int ingroupid=g->getMemberId(a);
        //cout << "ingroupid is " << ingroupid << endl;
        if (ingroupid < 0) {
            //cout << " adding atom " << a->id << " " << a->type << " to group " << g->getId() << endl;
            ingroupid = g->addAtom(a);
            //cout << "adding atom " << a->id << " to group " << g->getId() << endl;
        }
        g->incCoordinationNumber(ingroupid); //mCoordinationtable[X].size() != mCoordinationNumber[X].size() in the group g - this is by design but the naming is a little misleading.
        //cout << "addding atom " << b->id << " to coordination tbale of atom " << a->id << " in group " << g->getId() << endl;
        g->addToCoordinationTable(ingroupid, b);
        if (c!=nullptr) g->addToCoordinationTable(ingroupid, c);
        //g->update(nullptr, nullptr); //this function doesn't change thee members of the external group but increases its internal counter and "mTotalSize"
    } 
}
    
//void AngleRDF::print(ofstream *out1, ofstream *out2, ofstream *out3, ofstream *out4, ofstream *out5, ofstream *out6, ofstream *out7, ofstream *out8, Timestep *t) {
void AngleRDF::print(const vector<ofstream*> &vpOut, const Timestep *t) {
  //print
  //out1: WEIGHT vs (bond1, angle)
  //out2: WEIGHT vs (bond3, angle)
  //out3: WEIGHT vs (bond1, bond3)
  //out4: WEIGHT vs bond1 (summed over all angles)
  //out5: WEIGHT vs bond3 (summed over all angles)
  //out6: WEIGHT vs angle (summed over all bond lengths)
  //out7: WEIGHT vs (bond13, angle)
  //out8: WEIGHT vs bond13 (summed over all angles)

  //out6 is probably the most useful output....

  //TODO: the distances are not scaled with respect to volume... not a BIG deal since
  //anglerdf will be done in a fairly small region. However, it WILL skew the figures a bit.
  //
  long long int totalsum=0;
  //cout << mDistances1.size() << "\n";
  for (int i=0; i<mDistances1.size(); i++) {
    for (int j=0; j<mDistances1[i].size(); j++) {
        totalsum+=mDistances1[i][j];
    }
  }
  ofstream *out1=vpOut[0], *out2=vpOut[1], *out3=vpOut[2], *out4=vpOut[3], *out5=vpOut[4], *out6=vpOut[5], *out7=vpOut[6], *out8=vpOut[7];
  string group3id, group1id;
  string group2id=mGroup2->getId();
  stringstream ss;
  switch (mGroup3Type) {
    case AngleRDFGroupType::Real: group3id=mGroup3->getId(); break;
    case AngleRDFGroupType::DummyVector:
        ss << mGroup2->getId() << "+" << mGroup3DummyVector[0] << "," << mGroup3DummyVector[1] << "," << mGroup3DummyVector[2];
        group3id=ss.str();
        break;
    case AngleRDFGroupType::CoordinationShellCenterOfMass:
        group3id="CSCOM";
        break;
  }
  ss.clear(); ss.str("");
  switch (mGroup1Type) {
    case AngleRDFGroupType::Real: group1id=mGroup1->getId(); break;
    case AngleRDFGroupType::DummyVector:
        ss << mGroup2->getId() << "+" << mGroup1DummyVector[0] << "," << mGroup1DummyVector[1] << "," << mGroup1DummyVector[2];
        group1id=ss.str();
        break;
    case AngleRDFGroupType::CoordinationShellCenterOfMass:
        group1id="CSCOM";
        break;
  }
  (*out1) << "#bond(1-" << group1id << "-2-" << group2id << ") angle(" << group1id << "-" << group2id << "-" << group3id << ") weight" << "\n";
  (*out2) << "#bond(2-" << group2id << "-3-" << group3id << ") angle(" << group1id << "-" << group2id << "-" << group3id << ") weight" << "\n";
  (*out3) << "#bond(1-" << group1id << "-2-" << group2id << ") bond(2-" << group2id << "-3-" << group3id << ") weight" << "\n";
  (*out4) << "#bond(1-" << group1id << "-2-" << group2id << ") weight(norm) rawcount #### summed over all angles ####" << "\n";
  (*out5) << "#bond(2-" << group2id << "-3-" << group3id << ") weight(norm) rawcount #### summed over all angles ####" << "\n";
  (*out6) << "#angle(1-" << group1id << "-2-" << group2id << "-3-" << group3id << ") weight(norm) rawcount weight(norm)/sin(theta)### summed over all allowed bond lengths #### totalsum: " << totalsum << "\n";
  (*out7) << "#bond(1-" << group1id << "-3-" << group3id << ") angle(1-" << group1id << "-2-" << group2id << "-3-" << group3id << ") weight" << "\n";
  (*out8) << "#bond(1-" << group1id << "-3-" << group3id << ") weight(norm) rawcount  #### summed over all angles ####" << "\n";

  float bond1coord=0, bond3coord=0, bond13coord=0;
  long long int sum=0;
  for (int i=0; i<mDistances1.size(); i++) { //for each bond 1
    sum=0;
    bond1coord=coord(i, mGroup1MinDist, mGroup1MaxDist, mDistanceResolution);
    //cout << i << " " << bond1coord << "\n";
    for (int j=0; j<mDistances1[i].size(); j++) { //for each angle (bond 1)
      (*out1) << bond1coord << " " << coord(j, mMinAngle, mMaxAngle, mAngleResolution) << " " << mDistances1[i][j] << "\n";
      sum+=mDistances1[i][j];
    }
    (*out4) << bond1coord << " " << sum*1.0/totalsum << " " << sum << "\n";

    for (int j=0; j<mDistanceDistance[i].size(); j++) { //mDistance1 always has same size as mDistanceDistance, so this is safe
      (*out3) << bond1coord << " " << coord(j, mGroup3MinDist, mGroup3MaxDist, mDistanceResolution) << " " << mDistanceDistance[i][j] << "\n";
    }
  }
  for (int i=0; i<mDistances3.size(); i++) { //for each bond 3, same thing as before
    sum=0;
    bond3coord=coord(i, mGroup3MinDist, mGroup3MaxDist, mDistanceResolution);
    for (int j=0; j<mDistances3[i].size(); j++) { //for each angle (bond 3)
      (*out2) << bond3coord << " " << coord(j, mMinAngle, mMaxAngle, mAngleResolution) << " " << mDistances3[i][j] << "\n";
      sum+=mDistances3[i][j];
    }
    (*out5) << bond3coord << " " << sum*1.0/totalsum << " " << sum << "\n";
  }
  for (int i=0; i<mDistances13.size(); i++) { //for each bond 13, same thing as before
    sum=0;
    bond13coord=coord(i, mGroup13MinDist, mGroup13MaxDist, mDistanceResolution);
    for (int j=0; j<mDistances13[i].size(); j++) { //for each angle (bond 13)
      (*out7) << bond13coord << " " << coord(j, mMinAngle, mMaxAngle, mAngleResolution) << " " << mDistances13[i][j] << "\n";
      sum+=mDistances13[i][j];
    }
    (*out8) << bond13coord << " " << sum*1.0/totalsum << " " << sum << "\n";
  }

  //now comes the actual interesting part.....
  //cout << getDescription() << "\n";
  for (int i=0; i<mDistances1[0].size(); i++) { //for each angle
    sum=0;
    for (int j=0; j<mDistances1.size(); j++) { //for each bond 1
      sum+=mDistances1[j][i];
    }
    float dividedbysintheta=0;
    float thisangle=coord(i, mMinAngle, mMaxAngle, mAngleResolution);
    if (thisangle > 0.01) {
        dividedbysintheta = (sum*1.0/totalsum)/sin(3.14159*thisangle/180.0);
    }
    else {
        dividedbysintheta = 0;
    }
    (*out6) << coord(i, mMinAngle, mMaxAngle, mAngleResolution) << " " << sum*1.0/totalsum << " " << sum <<  " " << dividedbysintheta << "\n";
  }
  //(*out1) << flush;
  //(*out2) << flush;
  //(*out3) << flush;
  //(*out4) << flush;
  //(*out5) << flush;
  //(*out6) << flush;
  //(*out7) << flush;
  //(*out8) << flush;
}

float AngleRDF::coord(int i, float min, float max, float res) {
  return min+(i+0.5)*res;
}



float AngleRDF::getMinDist23(float d12) const {
    if (mMinDist23Frac12 > 0) return d12*mMinDist23Frac12;
    else return mGroup3MinDist;
}
float AngleRDF::getMaxDist23(float d12) const {
    if (mMaxDist23Frac12 > 0) return d12*mMaxDist23Frac12;
    else return mGroup3MaxDist;
}

float AngleRDF::getMinDist13(float d12, float d23) const {
    if (mMinDist13Frac12 > 0) return d12*mMinDist13Frac12; //later one should implement AND
    else if (mMinDist13Frac23 > 0) return d23*mMinDist13Frac23;
    else return mGroup13MinDist;
}
float AngleRDF::getMaxDist13(float d12, float d23) const {
    if (mMaxDist13Frac12 > 0) return d12*mMaxDist13Frac12; //later one should implement AND
    else if (mMaxDist13Frac23 > 0) return d23*mMaxDist13Frac23;
    else return mGroup13MaxDist;
}

void AngleRDF::makeGroupDummyVector(GroupExternal *g, const vector<float>& dummyvec, const Timestep *t) {
    //g = new GroupExternal;
    for (int i=0; i<mGroup2->size(); i++) {
        const Atom *a = mGroup2->atom(i);
        Atom *b = new Atom;
        b->x = a->x + dummyvec[0];
        b->y = a->y + dummyvec[1];
        b->z = a->z + dummyvec[2];
        //cout << "dumadding atom b " << b << " " << b->x << " " << b->y << " " << b->z << endl;
        g->addAtom(b);
        //cout << "dumadded atom b " << b << " " << b->x << " " << b->y << " " << b->z << endl;
    }
}

void AngleRDF::makeGroupCoordinationShellCenterOfMass(GroupExternal *g, const Timestep *t) {
    //this is just the average, not the center of mass
    //g = new GroupExternal;
    //cout << "g is " << g <<endl;
    for (int i=0; i<mGroup2->size(); i++) {
        const Atom *a = mGroup2->atom(i);
        Atom *b = new Atom;
        double x=a->x, y=a->y, z=a->z;
        int count=1;
        //cout << " orig " << a->x << " " << a->y << " " << a->z << " ";
        const Group* coordinationtable = dynamic_cast<GroupCoordination*>(mGroup2)->getCoordinationTable(i);
        for (int j=0; j<coordinationtable->size(); j++) {
            float  newx=coordinationtable->atom(j)->x,
                   newy=coordinationtable->atom(j)->y,
                   newz=coordinationtable->atom(j)->z;
            TranslateAtomCoordinatesToVicinity(newx,newy,newz, a->x, a->y, a->z, t);
            x+=newx;
            y+=newy;
            z+=newz;
            count++;
            //cout << newx << " " << newy << " " << newz << " ";
        }
        b->x = x/count;
        b->y = y/count;
        b->z = z/count;
        //cout << " ADDDING atom b " << b << " " << b->x << " " << b->y << " " << b->z << endl;
        g->addAtom(b);
        //cout << "added atom b " << b << " " << b->x << " " << b->y << " " << b->z << endl;
    }
}


void AngleRDF::cleanUpPseudoGroup(GroupExternal *g) {
    for (int i=0; i<g->size(); i++) {
        delete g->atom(i);
    }
    delete g;
}



/*
void AngleRDF::printGnu(const vector<ofstream*> &v, const string & p, const string & s) {
    int i=ACTIONANGLERDF_NUMOUTPUTFILES;
    if (v.size() < i+6) { 
        throw string("ERROR! AngleRdf::printGnu() needs at least ACTIONANGLERDF_NUMOUTPUTFILES + 6 elements");
    }
    string header;
    string g1=mGroup1->getId();
    string g2=mGroup2->getId();
    string g3=mGroup3->getId();
    header=R"(
        #miny2 = ; maxy2 = ;
        #minx1 = ; maxx1 = ;
        mytitle='r_{12}()" + g1 + "-" + g2 + ") and ang_{123}(" + g1 + "-" + g2 + "-" + g3 + R"(';
        filexy=')" + p + "_1'; filex='" + p + "_4'; filey='" + p + R"(_6';
    )";
    (*v[i]) << header << endl;
}
*/
