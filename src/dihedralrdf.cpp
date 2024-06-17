#include "dihedralrdf.h"

DihedralRDF::DihedralRDF()
: mGroup1MinDist(0.0), mGroup1MaxDist(0.0), mGroup3MinDist(0.0), mGroup3MaxDist(0.0), mGroup4MinDist(0.0), mGroup4MaxDist(0.0), mGroup1(nullptr), mGroup2(nullptr), mGroup3(nullptr), mGroup4(nullptr) {}

DihedralRDF::DihedralRDF(Group *g1, float min1, float max1, Group *g2, Group *g3, float min3, float max3, Group *g4, float min4, float max4, float angleres, float minangle, float maxangle)
: mGroup1(g1), mGroup2(g2), mGroup3(g3), mGroup4(g4), mGroup1MinDist(min1), mGroup1MaxDist(max1), mGroup3MinDist(min3), mGroup3MaxDist(max3), mGroup4MinDist(min4), mGroup4MaxDist(max4), mAngleResolution(angleres), mMinAngle(minangle), mMaxAngle(maxangle)
{ 
  int numelements=int((maxangle-minangle)/angleres) + 1;
  mValues.resize(numelements);
}

void DihedralRDF::update(const std::vector<Atom> *allatoms, const Timestep *t) {
    //cout << " in the update" << endl;
  if (mValues.size() == 0) return;
  for (int i=0; i<mGroup2->size(); i++) { //loop through center group atoms
    const Atom *atom2=mGroup2->atom(i);
    for (int j=0; j<mGroup1->size(); j++) { //find atoms in group 1 that obey distance condition
      const Atom *atom1=mGroup1->atom(j);
      if (atom2->id == atom1->id) continue;
      float bond1=atom2->bonds[atom1->id];
      //cout << "bond1 = " << bond1;
      if (bond1 >= mGroup1MinDist && bond1 <= mGroup1MaxDist) {
        for (int k=0; k<mGroup3->size(); k++) { //find atoms in group 3 that obey distance condition
          const Atom *atom3=mGroup3->atom(k);
          if (atom2->id == atom3->id || atom1->id == atom3->id) continue;
          float bond3=atom2->bonds[atom3->id];
          //cout << " bond3 = " << bond3 << " " << mGroup3MinDist << " " << mGroup3MaxDist;
          if (bond3 >= mGroup3MinDist && bond3 <=mGroup3MaxDist) {
            for (int l=0; l<mGroup4->size(); l++) { //find atoms in group4 that obey distance condition (to group 3 atom)
              const Atom *atom4=mGroup4->atom(l);
              if (atom4->id == atom1->id || atom4->id == atom2->id || atom4->id == atom3->id) continue;
              float bond4=atom3->bonds[atom4->id];
              //cout << " bond4 = " << bond4;
              if (bond4 >= mGroup4MinDist && bond4 <= mGroup4MaxDist) {
                float ang=dihedral(atom1, atom2, atom3, atom4, t);
                //float ang=dihedral(mGroup1->mAtoms[j], mGroup2->mAtoms[i], mGroup3->mAtoms[k], mGroup4->mAtoms[l], t);
                int bin=int( mValues.size()*(ang-mMinAngle)/(mMaxAngle-mMinAngle) );
                if (bin < 0) bin =0;
                cout << "ang= " << ang << " bin= " << bin << " mValues.size()= " << mValues.size() << " " << atom1->x << " " << atom1->y << " " << atom1->z << " " << atom2->x << " " << atom2->y << " " << atom2->z << " " << atom3->x << " " << atom3->y << " " << atom3->z << " " << atom4->x << " " << atom4->y << " " << atom4->z << endl;
                if (ang < mMinAngle || ang >= mMaxAngle) continue;
                mValues[bin]++;
              } //bond4 condition
            } //group4 loop
          } //bond3 condition
        } //group3 loop
      } //bond1 condition
      //cout << endl;
    } //group1 loop
  } //group2 loop
} //incrdf()
    
void DihedralRDF::print(ofstream *out, const Timestep *t) {
  for (int i=0; i<mValues.size(); i++) {
    (*out) << mMinAngle+(i+0.5)*mAngleResolution << " " << mValues[i] << endl;
  }
}


