#include "vac.h"
#include <iostream>
#include <omp.h>
#include <complex.h>
//#include <fftw3.h>
#include <cmath>
#include "omp.h"
#include "utility.h"

using namespace std;

VAC::VAC(Group *g, int maxtime) 
:mMaxTime(maxtime), mContinuousMembers(false)
{
    mGroup = g;
    mInitialized = false;
    //mMaxTime = maxtime;
    //cout << "mMaxTime is " << mMaxTime << endl;
    //mAverage = false;
    //mMasCen = false;
    mTimeUnit = 1; //default value 0
    mIntelligent = false;
    mDimX = true;
    mDimY = true;
    mDimZ = true;
    mCalculateVelocities = true;
    mDoFT=false;
    mSigmaFraction=0.4;
    mPadd=15;
    //cout << "I'm in " << this << endl;
}

void VAC::calculateVelocities(float x1, float y1, float z1, float x2, float y2, float z2, vector<float>& vfloat)
{
   vfloat.push_back((x2-x1)/mTimeUnit);
   vfloat.push_back((y2-y1)/mTimeUnit);
   vfloat.push_back((z2-z1)/mTimeUnit);
}

void VAC::update_with_continuous_members(Timestep *timestep) {
    //this is basically the same function as update(), but I keep it separated while developing
    //this method should be slower than update(), since the mLastPositions, mVelocities etc. vectors
    //have to have the sizes of ALL atoms, and only the elements in mGroup need to be kept tracked of.
    //this means that I access the mVelocities vector by ORIGINAL atom id rather than the id within the group
    
    //the "+1" here is because atom ids start at 1 (and not 0), and I access mVelocities[id] rather than mVelocities[id-1]
    if (timestep->numatoms+1 > mVelocities.size()) mVelocities.resize(timestep->numatoms+1);
    if (timestep->numatoms+1 > mLastPositions.size()) mLastPositions.resize(timestep->numatoms+1);
    if (timestep->numatoms+1 > mEntered.size()) mEntered.resize(timestep->numatoms+1,0);
    //mExited.resize(timestep->numatoms+1,0);
    if (mInitialized == false) {
        mInitialized = true;
        mData.resize(mMaxTime,0);
        mDataCount.resize(mMaxTime,0);
        for (int i=0; i<mLastPositions.size(); i++) {
            mLastPositions[i].resize(3,0);
        }
        for (int i=0; i<mGroup->size(); i++) {
            int id=mGroup->atom(i)->id; 
            mLastPositions[id][0]=mGroup->atom(i)->xu;
            mLastPositions[id][1]=mGroup->atom(i)->yu;
            mLastPositions[id][2]=mGroup->atom(i)->zu;
            mVelocities[id].push_back(0);
            mVelocities[id].push_back(0);
            mVelocities[id].push_back(0);
            mEntered[id]=timestep->iteration;
        }
        return;
    }
    //update the member vector mEntered detailing the last times the atoms in mGroup entered the group
    for (int i=1; i<=timestep->numatoms; i++) { //all atoms in the current timestep
        bool foundmatch=false;
        for (int j=0; j<mGroup->size(); j++) { //all atoms in the group
            if (i==mGroup->atom(j)->id) {
                if (mEntered[i] == 0) {
                    mEntered[i]=timestep->iteration; //atom id i entered the group on the CURRENT timestep
                }
                else {
                    //do nothing --- keep the old value of mEntered[id]
                    ;
                }
                foundmatch=true;
                break;
            }
        }
        if (foundmatch==false) mEntered[i]=0; //mEntered[id]==0 means that the atom currently is not in the group
    }
    //now mEntered contains 0 if the atom is not in mGroup, and the last timestep it entered otherwise

    //calculate the VAC for all allowed timesteps that are smaller than mMaxTime AND that are smaller than ( timestep.iteration - mEntered[i] )
    //so that the VAC is only calculated for an atom that has been within the group for ENTIRE dt
    for (int i=0; i<mGroup->size(); i++) {
        float vx=0,vy=0,vz=0;
        float x=mGroup->atom(i)->xu;
        float y=mGroup->atom(i)->yu;
        float z=mGroup->atom(i)->zu;
        int id=mGroup->atom(i)->id;
        if (mCalculateVelocities == true) {
            //you cannot calculate velocities if the atom entered the group on the current timestep
            //it might actually be easier to calculate the velocities for ALL atoms and always save them, but I don't currently have access to the global atoms vector. 
            if (mEntered[id] < timestep->iteration) {
                if (mIntelligent == true && mVelocities[id].size() >= 3) { 
                    TranslateAtomCoordinatesToVicinity(x,y,z,mLastPositions[id][0],mLastPositions[id][1],mLastPositions[id][2],timestep);
                }
                calculateVelocities(mLastPositions[id][0], mLastPositions[id][1], mLastPositions[id][2], x, y, z, mVelocities[id]);
            }
            else {
                mVelocities[id].push_back(0); //dummy values 
                mVelocities[id].push_back(0);
                mVelocities[id].push_back(0);
            }
            //save the positions regardless if atom entered the group this timestep or not
            mLastPositions[id][0]=mGroup->atom(i)->xu;
            mLastPositions[id][1]=mGroup->atom(i)->yu;
            mLastPositions[id][2]=mGroup->atom(i)->zu;
            //cout << i << " " << mGroup->mAtoms[i]->id << " " << mEntered[i] << endl;
            if (mEntered[id] >= timestep->iteration) { //should never be greater than..... only < or == is possible.
                continue; //saving the position in mLastPositions is enough (as well as adding dummy values to mVelocites)
            }
        }
        else {
            mVelocities[id].push_back(mGroup->atom(i)->vx);
            mVelocities[id].push_back(mGroup->atom(i)->vy);
            mVelocities[id].push_back(mGroup->atom(i)->vz);
        }

        float vac=0;
        int mVelSize=0;
        int startcoordinate=0;
        if (timestep->iteration - mMaxTime < mEntered[id]) {
            startcoordinate = mVelocities[id].size() - (timestep->iteration - mEntered[id])*3;
        }
        else {
            startcoordinate = mVelocities[id].size() - mMaxTime*3;
        }
        if (startcoordinate < 3) startcoordinate=3; //never look at the three initial dummy values in mVelocities
        for (int j=startcoordinate; j<mVelocities[id].size(); j+=3) { 
            vac=0;
            mVelSize=mVelocities[id].size();
            if (mDimX) vac+=mVelocities[id][mVelSize-3] * mVelocities[id][j];
            if (mDimY) vac+=mVelocities[id][mVelSize-2] * mVelocities[id][j+1];
            if (mDimZ) vac+=mVelocities[id][mVelSize-1] * mVelocities[id][j+2];
            int dt=(mVelSize-j-3)/3;
            //cout << "timestep " << timestep->iteration << " id " << id << " dt " << dt << " vac " << vac << endl;
            mData[dt]+=vac; 
            mDataCount[dt]++;
        }
    }
}

void VAC::update(Timestep *timestep) {
    if (mContinuousMembers) {
        update_with_continuous_members(timestep);
        return;
    }
    //cout << mMaxTime << " " << this << endl;
    if (mInitialized == false) {
        mVelocities.resize(mGroup->size());
        //mData.resize(mGroup->mAtoms.size());
        mLastPositions.resize(mGroup->size());
        mData.resize(mMaxTime,0);
        mDataCount.resize(mMaxTime,0);
        //cout << mData.size() << " " << mDataCount.size() << "maxtime is " << mMaxTime << endl;
        //for (int i=0; i<mData.size(); i++) {
            //mData[i].resize(mMaxTime+3);
        //}
        //if (mIntelligent) { cout << "I'm intelligent" << endl; }
        //if (!mIntelligent) { cout << "I'm NOT intelligent" << endl; }
        mInitialized = true;
        for (int i=0; i<mGroup->size(); i++) {
            mLastPositions[i].push_back(mGroup->atom(i)->xu);
            mLastPositions[i].push_back(mGroup->atom(i)->yu);
            mLastPositions[i].push_back(mGroup->atom(i)->zu);
            mVelocities[i].push_back(0); //these are dummy values forthe first timestep,make sure that they are not read below!
            mVelocities[i].push_back(0);
            mVelocities[i].push_back(0);
        }
        return;
    }

    if (mGroup->size() == 0) {
        return;
    }



    int startcoordinate=mVelocities[0].size()-mMaxTime*3; //do not backtrack more than mMaxTime timesteps in the coordinate history
    if (startcoordinate < 3) startcoordinate=3;
  
  
    for (int i=0; i<mVelocities.size(); i++) { //loop the atoms in the group
        float vx=0,vy=0,vz=0;
        float x=mGroup->atom(i)->xu;
        float y=mGroup->atom(i)->yu;
        float z=mGroup->atom(i)->zu;
        if (mCalculateVelocities == true) {
            if (mIntelligent == true && mVelocities[i].size() >= 3) { //if "intelligent", translate the coordinates to around the coordinates of the previous frame.
                //cout << "translating atom " << i <<  " " << x << " " << y << " " << z << " " << mLastPositions[i][0] << " " << mLastPositions[i][1] << " " << mLastPositions[i][2] <<endl;
                //cout << timestep->cellx << " " << timestep->celly << " " << timestep->cellz << endl;
                TranslateAtomCoordinatesToVicinity(x,y,z,mLastPositions[i][0],mLastPositions[i][1],mLastPositions[i][2],timestep);
            }
            calculateVelocities(mLastPositions[i][0], mLastPositions[i][1], mLastPositions[i][2], x, y, z, mVelocities[i]);
            mLastPositions[i][0]=mGroup->atom(i)->xu;
            mLastPositions[i][1]=mGroup->atom(i)->yu;
            mLastPositions[i][2]=mGroup->atom(i)->zu;
        }
        else {
            mVelocities[i].push_back(mGroup->atom(i)->vx);
            mVelocities[i].push_back(mGroup->atom(i)->vy);
            mVelocities[i].push_back(mGroup->atom(i)->vz);
        }
  
        //#pragma omp parallel for schedule(static) default(shared)
        //cout << "startcoordinate= " << startcoordinate << " " << mCoordinates[i].size() << endl;
        float vac=0;
        int mVelSize=0;
        for (int j=startcoordinate; j<mVelocities[i].size(); j+=3) { //loop the coordinates for this atom
            //j<mVelocities[i].size() means that the last j will include the scalar product by itself (dt=0)
            //j<mVelocities[i].size()-3 means that only dt>0 will be included
          vac=0;
          mVelSize=mVelocities[i].size();
          if (mDimX) vac+=mVelocities[i][mVelSize-3] * mVelocities[i][j];
          if (mDimY) vac+=mVelocities[i][mVelSize-2] * mVelocities[i][j+1];
          if (mDimZ) vac+=mVelocities[i][mVelSize-1] * mVelocities[i][j+2];
          int dt=(mVelSize-j-3)/3;
          //if (mVelSize-1 == j+2) {
          //  cout << "its on "  << dt << endl;
          //}
          //cout << "adding vac " << vac << " to deltat " << dt << endl;
          mData[dt]+=vac; //for VAC it is not really necessary to store the data per atom? would make more sense to just store the total.
          mDataCount[dt]++;
        }
    }
}

void VAC::normalize() {
    for (int i=0; i<mData.size(); i++) {
        if (mDataCount[i] > 0) {
            mData[i]/=mDataCount[i];
        }
        else {
            mData[i]=0;
        }
    }
    for (int i=1; i<mData.size(); i++) {
        mData[i]/=mData[0];
    }
    mData[0]=1.0f;
}

void VAC::print(ofstream *o, Timestep *t) {
    float s=0; int numcont=0;
    normalize();
    for (int i=0; i<mData.size(); i++) {
        //s=0; numcont=0;
        //if (mDataCount[i] > 0) {
            (*o) << i << " " << mData[i] << endl;
        //}
        //for (int j=0; j<mData.size(); j++) {
            //s+=mData[j][i].f;
            //numcont+=mData[j][i].i;
            //cout << "s " << s << " numcont " << numcont << endl;
        //}
        //if (numcont > 0) {
            //(*o) << i << " " << s/numcont << endl;
        //}
    }
    //cout << "hello" << endl;
    //mData.clear();
}


void VAC::printFT(ofstream *o, Timestep *timestep) {
   /*
    * some stuff from the tfreq program on 
    * http://www.timteatro.net/2010/09/29/velocity-autocorrelation-and-vibrational-spectrum-calculation/
    * M is the total number of timesteps
    */
   // the original script normalizes the VAC with respect to the first entry. I don't know if this is necessary, but this is done in the normalize() function called by print() (which should be called BEFORE printFT()!!)
   // Zero our padding and apply gaussian smoothing, then rescale
    /*
    int origsize=mData.size();
    int newsize=mData.size()*mPadd;
    fftw_complex *Z, *dft_out;
    fftw_plan dft_plan;
    Z = (fftw_complex *) fftw_malloc(newsize * sizeof(fftw_complex));
    dft_out = (fftw_complex *) fftw_malloc(newsize * sizeof(fftw_complex));
    for (int i=0; i<newsize; i++) {
        if (i<origsize) Z[i]=mData[i];
        else Z[i]=0;
    }
    float sigma = origsize * mSigmaFraction;
    double sqtp = 2.506628274631000; // sqrt of 2 Pi
    for (int i = 0; i < newsize; i++) {
        //cout << i << " " << creal(Z[i]);
        Z[i] *= exp(-i * i / (2 * sigma * sigma)) / (sigma * sqtp); //gaussian smoothing
        //the smoothing here is done from dt=0 to dt=mMaxTime, so that the
        //"peaks" in the VACF become smaller and smaller for greater dt:s.
        //it is actually a Gaussian TAPERING of the VACF
        //cout << " " << creal(Z[i]) << endl;
    }
    for (int m = 1; m < newsize; m++) {
        Z[m] /= Z[0]; //rescale
    }
    Z[0] = 1.0f; //rescale

    // Discrete Fourier Transform (DFT)
    dft_plan = fftw_plan_dft_1d(newsize, Z, dft_out, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(dft_plan);

    // Normalize max peak to 1
    fftw_complex norm = 0.00f;
    for (int m = 0; m <= newsize / 2; m++)
        if (creal(dft_out[m] * conj(dft_out[m])) > creal(norm))
            norm = dft_out[m] * conj(dft_out[m]);
    norm = 1/norm;

    // Print Output
    char s[200];
    for (int m = 0; m <= (newsize / 2); m++) {
        //this is the "frequency" in reciprocal centimeters. speed of light in m/s; mTimeUnit is in ps so 1E-12
        double freq = m / (2.99792458E10 * mMaxTime * mTimeUnit * mPadd * 1E-12);
        if (freq > 6000) break; //don't print stuff for freqeuencies greater than 10000 cm-1
        sprintf(s,"%17.9E %17.9E %17.9E %17.9E",
              freq,
              creal(dft_out[m]),
              cimag(dft_out[m]),
              norm * dft_out[m] * conj(dft_out[m]));
        
        (*o) << s << endl;
    }
    return;
    */
}

VAC::~VAC() {
    //cout << "before clearance" << endl;
    //mData.clear();
    //cout << "after clearance" << endl;
}


