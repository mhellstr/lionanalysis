#include "actionmsdfollow.h"
#include "tcf.h"
#include "tcfmsd.h"

ActionMSDFollow::ActionMSDFollow() 
: ActionMSD()
{
    mNumItemsPerTimeStep=3;
}

float ActionMSDFollow::getShortDistance(int atomindex, const Timestep *t, int& switchindex, const vector<int> &vIgnore) {
    int i=atomindex;
    float mindist=10000;
    for (int j=0; j<mOldPositions.size(); j+=3) {
        bool allowed=true;
        for (int k=0; k<vIgnore.size(); k++) {
            if (vIgnore[k] == j/3) {
                allowed=false;
                break;
            }
        }
        if (allowed) {
            float d=distance(mOldPositions[j], mOldPositions[j+1], mOldPositions[j+2], mpGroup->atom(i)->xu, mpGroup->atom(i)->yu, mpGroup->atom(i)->zu, t);
            if (d<mindist) {
                mindist=d;
                switchindex=j/3;
            }
        }
    }
    return mindist;
}

void ActionMSDFollow::push_back_in_data(const Timestep *t) {
    bool found_conflict=false;
    vector<float> mNewPositions;
    if (mOldPositions.empty()) {
        found_conflict=true;
    }
    else if (mOldPositions.size() == mpGroup->size()*3) {
        vector<int> vOccupiedSwitchIndices(mpGroup->size(), -1);
        vector<int> vSwitchIndices(mpGroup->size(), -1);
        vector<float> vMinDists(mpGroup->size(), -1);
        vector<int> vIgnore;
        vIgnore.clear();
        for (int i=0; i<mpGroup->size(); i++) { //i is new index
            float mindist=1000;
            int switchindex=0; //for the i:th element in mpGroup, what is the "position(/3)" that is the corresponding one in mOldPositions?
            mindist=getShortDistance(i, t, switchindex, vIgnore);
            vMinDists[i]=mindist;
            //for (int j=0; j<mOldPositions.size(); j+=3) {
            //    float d=distance(mOldPositions[j], mOldPositions[j+1], mOldPositions[j+2], mpGroup->atom(i)->xu, mpGroup->atom(i)->yu, mpGroup->atom(i)->zu, t);
            //    if (d < mindist) {
            //        mindist=d;
            //        switchindex=j/3;
            //        vMinDists[i]=mindist;
            //    }
            //}
            if (vOccupiedSwitchIndices[switchindex] >= 0) { //already found this one before
                //cout << "FUNNY! " << endl;
                //assume that not more than two would go to the same place, so check if the present one or the previous one is closer
                //mOldPositions contain many coordinates: 1x, 1y, 1z, 2x, 2y, 2z, 3x, 3y, 3z...
                //vOccupiedSwitchIndices contains 1..n elements that is analogous to mOldPositions, that is,
                //vOccupiedSwitchIndices[1] is the groupindex (inside mpGroup), that the coordinates 2x, 2y, and 2z refer to
                //vSwitchIndices is the reverse, i.e.
                //vSwitchIndices[1] gives the position inside mOldPositions that the second atom in mpGroup "is"
                //
                //if vOccupiedSwitchIndices[switchindex] >=0, this means that a previous atom wanted to refer to the same position (switchindex) in mOldPositions
                //that we want to refer to now
                //
                //we need to solve this conflict, so find out what the "second best" options for the previous (previous in the sense it appears in mpGroup at a lower groupindex) atom is
                //and find out what the second best option for "this" atom i is
                // if the second best option cannot be used (because it is already taken by a THIRD (unknown) atom) for the previous one, use the second best option for this one
                // if the second best option cannot be used (because it is already taken by a THIRD (unknown) atom) for this one, use the second best option for the previous one
                // if both second best options can be used, determine which gives the *smaller* movement
                int previndex=vOccupiedSwitchIndices[switchindex];
                float prevdist=vMinDists[previndex];
                int altprevswitchindex=-1;
                float altprevdist=getShortDistance(previndex, t, altprevswitchindex, vector<int>(1, vSwitchIndices[previndex]));
                bool altprevallowed=true;
                if (vOccupiedSwitchIndices[altprevswitchindex] >=0) {
                    altprevallowed=false;
                }
                int altthisswitchindex=-1;
                float altthisdist=getShortDistance(i, t, altthisswitchindex, vector<int>(1, switchindex));
                bool altthisallowed=true;
                if (vOccupiedSwitchIndices[altthisswitchindex] >=0) {
                    altthisallowed=false;
                }

                if (!altprevallowed && !altthisallowed) {
                    cerr << "CONFLICT (iteration " << t->iteration << "): I couldn't switch previous " << previndex << " from " << vSwitchIndices[previndex] << " to " << altprevswitchindex;
                    cerr << ", nor could I switch the current " << i << " from " << switchindex << " to " << altthisswitchindex << endl;
                    found_conflict=true;
                    break;
                }
                else if (!altprevallowed && altthisallowed) {
                    //cout << "newallwoed" << endl;
                    vOccupiedSwitchIndices[altthisswitchindex]=i;
                    vSwitchIndices[i]=altthisswitchindex;
                }
                else if (altprevallowed && ! altthisallowed) {
                    //cout << "olallowed" << endl;
                    //change the previous one
                    vOccupiedSwitchIndices[altprevswitchindex]=previndex;
                    vSwitchIndices[previndex]=altprevswitchindex;
                    //set the new one
                    vOccupiedSwitchIndices[switchindex]=i;
                    vSwitchIndices[i]=switchindex;
                }
                else if (altprevallowed && altthisallowed) {
                    //cout << "bothallowed" << endl;
                    float prevdelta=altprevdist - prevdist; if (prevdelta < 0) prevdelta=-prevdelta;
                    float thisdelta=altthisdist - mindist; if (thisdelta < 0) thisdelta=-thisdelta;
                    if (prevdelta < thisdelta) {
                        //cout << "prev < this: " << prevdelta << " < " << thisdelta << " " << i << " " << previndex << endl;
                        //change the previous one
                        vOccupiedSwitchIndices[altprevswitchindex]=previndex;
                        vSwitchIndices[previndex]=altprevswitchindex;
                        //set the new one
                        vOccupiedSwitchIndices[switchindex]=i;
                        vSwitchIndices[i]=switchindex;
                    }
                    else {
                        //cout << "this < prev " << thisdelta << " < " << prevdelta << " " << i << " " << previndex << endl;
                        vOccupiedSwitchIndices[altthisswitchindex]=i;
                        vSwitchIndices[i]=altthisswitchindex;
                    }
                }



                //cerr << "vOccupiedSwitchindices[switchindex] already set to " << vOccupiedSwitchIndices[switchindex] << " whikle attempting to set it to " << i << endl;
                //found_conflict=true;
                //break;
            }
            else {
                //cout << "NOT funny! ";
                //there was no conflict, so simply store which position in mOldPositions (switchindex) that this atom correpsonds to
                vOccupiedSwitchIndices[switchindex]=i;
                vSwitchIndices[i]=switchindex;
            }
            //vOccupiedSwitchIndices[switchindex]=i;
            //vSwitchIndices[i]=switchindex;
        }
        if (!found_conflict) {
            for (int i=0; i<mOldPositions.size(); i+=3) {
                int groupindex=vOccupiedSwitchIndices[i/3];
                Atom a;
                //important to set the unwrapped coordinates here, since internalAddtoTCF will use the unwrapped onces
                a.xu=mpGroup->atom(groupindex)->xu;
                a.yu=mpGroup->atom(groupindex)->yu;
                a.zu=mpGroup->atom(groupindex)->zu;
                TranslateAtomCoordinatesToVicinity(a.xu, a.yu, a.zu, mOldPositions[i], mOldPositions[i+1], mOldPositions[i+2], t);
                //mOldPositions[i+0]=(mpGroup->atom(groupindex)->xu);
                //mOldPositions[i+1]=(mpGroup->atom(groupindex)->yu);
                //mOldPositions[i+2]=(mpGroup->atom(groupindex)->zu);
                mOldPositions[i+0]=a.xu;
                mOldPositions[i+1]=a.yu;
                mOldPositions[i+2]=a.zu;
                //cout << mpGroup->atom(groupindex)->id << " " << mOldPositions[i] << " " << mOldPositions[i+1] << " " << mOldPositions[i+2] << " ";
                internalAddToTCF(i/3, &a);
            }
            //cout << endl;
        }
    }
    else {
        cerr << "CONFLICT (iteration " << t->iteration << "): size changed from " << mOldPositions.size()/3 << " to " << mpGroup->size() << endl;
        found_conflict=true;
    }

    if (found_conflict) {
        for (int i=0; i<mTCF.size(); i++) {
            mTCF[i]->clear();
        }
        mOldPositions.clear();
        for (int i=0; i<mpGroup->size(); i++) {
            mOldPositions.push_back(mpGroup->atom(i)->xu);
            mOldPositions.push_back(mpGroup->atom(i)->yu);
            mOldPositions.push_back(mpGroup->atom(i)->zu);
            internalAddToTCF(i, mpGroup->atom(i));
        }
    }

    for (int i=0; i<mTCF.size(); i++) {
        mTCF[i]->correlateWithLatest();
    }
    
}

