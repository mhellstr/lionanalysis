#include "rdf.h"
#include "utility.h"
#include "constants.h"
#include <cmath>
//#include <omp.h>

RDF::~RDF() {
}

RDF::RDF()
:mMinDist(0.01),mMaxDist(10.0),mResolution(0.01),anchorgroup(nullptr),targetgroup(nullptr),mUsePeriodicImages(false), mTotCountAnchorGroup(0), mTotCountTargetGroup(0), mCountTimesteps(0), mPrintHeader(true)
{ 
}


void RDF::update(const vector<Atom> *allatoms, const Timestep *timestep) {
    mCountTimesteps++;
    //cout << "mCountTimesteps = " << mCountTimesteps << endl;
    //cout << "fromgroup = " << anchorgroup->getId() << " size = " << anchorgroup->size() <<  " targetgroup = " << targetgroup->getId() << " size = " << targetgroup->size() << endl;
    if (mCountTimesteps == 1) { //on the first update, resize values vector
        //cout << "Resizing vector" << endl;
        int numelements=int((mMaxDist-mMinDist)/mResolution) + 1;
        values.resize(numelements, 0);
    }
    //mTotCountTargetGroup+=targetgroup->size();
    //mTotCountAnchorGroup+=anchorgroup->size();
    if (mUsePeriodicImages) {
        for (int i=0; i<anchorgroup->size(); i++) {
            //const Atom *a=(*anchoratoms)[i];
            const Atom *a=anchorgroup->atom(i);
            for (int j=0; j<targetgroup->size(); j++) {
                const Atom *b=targetgroup->atom(i);
                //Atom *b=(*targetatoms)[j];
                float arrdist[27];
                //cout << a->x << " " << a->y << " " << b->x << endl;
                periodicdistances(a->x, a->y, a->z, b->x, b->y, b->z, arrdist, timestep);
                for (int k=0; k<27; k++) {
                    float dist=arrdist[k];
                    //cout << "k is " << k << " dist is " << dist << endl;
                    if (dist > mMaxDist || dist < mMinDist) continue;
                    int bin = int( values.size()*(dist-mMinDist)/(mMaxDist-mMinDist) );
                    //cout << k << " " << dist << " " << bin << endl;
                    values[bin]++;
                }
            }
        }
    }
    else  {
        //cout << "anchorgroup id and size: " << anchorgroup->getId() << " " << anchorgroup->size() << " ; targ : " << targetgroup->getId() << " " << targetgroup->size() << endl;
        for (int i=0; i<anchorgroup->size(); i++) {
            const Atom *a=anchorgroup->atom(i);
            for (int j=0; j<targetgroup->size(); j++) {
                const Atom *b=targetgroup->atom(j);
                if (a->id == b->id) continue;
                float dist = a->bonds[b->id];
                if (dist >= mMaxDist || dist < mMinDist) continue;
                int bin = int( values.size()*(dist-mMinDist)/(mMaxDist-mMinDist) );
                if (bin < 0) continue;
                values[bin]++;
                if (bin ==0 && timestep->iteration < 10) {
                    cout << "WARNING: bin==0 for RDF from " << anchorgroup->getId() << " to " << targetgroup->getId() << " ; Did you set GlobalDistanceGroup to something strange?" << endl;
                }
            }
        }
    }
}


void RDF::print(ofstream *o, const Timestep *t) {
    // NOTE : if the sizes of anchorgroup or targetgroup change during the simulation
    // the normalization at the end might be strange

    float integratedrdf=0;
    float averageSizeAnchorGroup = anchorgroup->getAverageSize(); // (mTotCountAnchorGroup * 1.0 / mCountTimesteps);
    float averageSizeTargetGroup = targetgroup->getAverageSize(); //(mTotCountTargetGroup * 1.0 / mCountTimesteps);
    if (anchorgroup == targetgroup) averageSizeTargetGroup--;
    // TODO: numberdensity is not correct if cell volume changes during simulation?
    float numberdensity = averageSizeTargetGroup / (t->cellx * t->celly *t->cellz);

    if (mPrintHeader) {
        (*o) << "#r RDF iRDF PMF(kTunits) rawcount rawcountpertimestepperatom # from " << anchorgroup->getId() << " to " << targetgroup->getId() << "; nSteps = " << mCountTimesteps << " size(" << anchorgroup->getId() << ") = " << averageSizeAnchorGroup << " size(" << targetgroup->getId() << ") = " << averageSizeTargetGroup << " numberdens. = " << numberdensity << endl;
    }

    for (int i=0; i<values.size(); i++) {
      float meanr=mMinDist+(i+0.5)*mResolution;
      //the values[] essentially contains the coordination number for each bin, multiplied by the number of timesteps and atoms in the anchor group
      //so normalize with respect to these two things and add the resulting value to integratedrdf
      float normalizedvalue =  values[i]*1.0/ (mCountTimesteps * averageSizeAnchorGroup);
      integratedrdf+=normalizedvalue;
      float valrdf =  normalizedvalue / (numberdensity * 4 * 3.14159 * meanr * meanr * mResolution) ;
      float pmf=-std::log(valrdf);
      //float integrand = 4 * 3.14159 * meanr * meanr * numberdensity * valrdf; //https://en.wikipedia.org/wiki/Coordination_number

      //the x coordinate is the center of the bin
      (*o) << mMinDist+(i+0.5)*mResolution << " " << valrdf  << " " << integratedrdf << " " << pmf << " " << values[i] << " " << normalizedvalue << endl;
    }
}


