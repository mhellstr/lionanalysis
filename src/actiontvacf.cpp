#include "actiontvacf.h"
#include "ttcf.h"
#include "ttcfvacf.h"
#include <complex.h>
#include <cmath>
#ifndef NO_FFT
#include <fftw3.h>
#endif

ActionTVACF::ActionTVACF() 
: ActionTTCF<INTYPE,OUTTYPE>(ActionType::VACF,2,3,1,5, TCF_t::VACF), mManipulateMin(3200), mManipulateMax(3800), mManipulateType(0)
{
    mNumItemsPerTimeStep=3;
    mDoNormalize0=true;
    mDoPrintRawNumbers=true;
    mDoPrintHeader=true;
}
ActionTVACF::ActionTVACF(ActionType actiontype, TCF_t tcftype)
: ActionTTCF<INTYPE,OUTTYPE>(actiontype, 2, 3, 1, 5, tcftype)
{ 
    mNumItemsPerTimeStep=3;
}

ActionTVACF::~ActionTVACF()
{ }


void ActionTVACF::internalMoreAtomsThanExpected(int numatoms) {
}

void ActionTVACF::internalAtomLeftEnteredVector(int zerobasedid) {
    //mTCF[zerobasedid]->clear(); important to NOT clear this thing for ResidenceTime
}
void ActionTVACF::internalAddToTCF(int zerobasedid, const Atom* a) {
    //cout << mpSubGroup->getId() << endl;
    //cout << mpGroup->getId() << endl;
    if (mDimX) {
        //if (zerobasedid==1) cout << "adding x " << a->xu << endl;
        mTCF[zerobasedid]->add(a->vx);
    }
    if (mDimY) {
        //if (zerobasedid==1) cout << "adding y " << a->yu << endl;
        mTCF[zerobasedid]->add(a->vy);
    }
    if (mDimZ) {
        //if (zerobasedid==1) cout << "adding z " << a->zu << endl;
        mTCF[zerobasedid]->add(a->vz);
    }
    //if (mvBoolsToSave[BoolInfoIndex::IsMember1]) {
        //bool ismember1=mpSubGroup->isMember(a);
        //if (mDimX) mTCF[zerobasedid]->addBoolInfo(ismember1, BoolInfoIndex::IsMember1);
        //if (mDimY) mTCF[zerobasedid]->addBoolInfo(ismember1, BoolInfoIndex::IsMember1);
        //if (mDimZ) mTCF[zerobasedid]->addBoolInfo(ismember1, BoolInfoIndex::IsMember1);
    //}
}

TTCF<ActionTVACF::INTYPE, ActionTVACF::OUTTYPE>* ActionTVACF::newTCF() const {
    TTCF<INTYPE, OUTTYPE>* a = new TTCFVACF(this);
    return a;
}


void ActionTVACF::internalFinalAction(const vector<Atom>* allatoms, const Timestep *t) {
    ActionTTCF::internalFinalAction(allatoms, t);
}

void ActionTVACF::manipulateIndividualTCF(const vector<pair<OUTTYPE, long long int>>& data, int ingroupid, bool out_was_updated) {
#ifndef NO_FFT
    ofstream *o = mvpOuts[1];
    if (!out_was_updated) {
        (*o) << "?";
    }
    else {
        int mPadd = 50;
        double mSigmaFraction = 0.3;
        double actualtimeunit=0.0005;
        int origsize=data.size();
        int newsize=data.size()*mPadd;
        fftw_complex *Z, *dft_out;
        fftw_plan dft_plan;
        Z = (fftw_complex *) fftw_malloc(newsize * sizeof(fftw_complex));
        dft_out = (fftw_complex *) fftw_malloc(newsize * sizeof(fftw_complex));
        for (int i=0; i<newsize; i++) {
            if (i<origsize) {
                Z[i][0]=data[i].first;
                Z[i][1]=0;
            }
            else {
                Z[i][0]=0;
                Z[i][1]=0;
            }
        }
        float sigma = origsize * mSigmaFraction;
        double sqtp = 2.506628274631000; // sqrt of 2 Pi
        for (int i = 0; i < newsize; i++) {
            //cout << i << " " << creal(Z[i]);
            Z[i][0] *= exp(-i * i / (2 * sigma * sigma)) / (sigma * sqtp); //gaussian smoothing
            //the smoothing here is done from dt=0 to dt=mMaxTime, so that the
            //"peaks" in the VACF become smaller and smaller for greater dt:s.
            //cout << " " << creal(Z[i]) << endl;
        }
        for (int m = 1; m < newsize; m++) {
            Z[m][0] /= Z[0][0]; //rescale
        }
        Z[0][0] = 1.0f; //rescale

        // Discrete Fourier Transform (DFT)
        dft_plan = fftw_plan_dft_1d(newsize, Z, dft_out, FFTW_FORWARD, FFTW_ESTIMATE);
        fftw_execute(dft_plan);

        // Normalize max peak to 1
        //fftw_complex norm = {0.00f,0.00f};
        //for (int m = 0; m <= newsize / 2; m++)
            //if (creal(dft_out[m] * conj(dft_out[m])) > creal(norm))
                //norm = dft_out[m] * conj(dft_out[m]);
        //norm = 1/norm;

        // Print Output
        char s[200];
        double peakfreq=0; double peakcount=0;
        double maxintensity=0;
        for (int m = 0; m <= (newsize / 2); m++) {
            //this is the "frequency" in reciprocal centimeters. speed of light in m/s; mTimeUnit is in ps so 1E-12
            double freq = m / (2.99792458E10 * newsize * actualtimeunit * 1E-12);
            if (freq > 5000) break; //don't print stuff for freqeuencies greater than 10000 cm-1
            if (freq >= mManipulateMin && freq < mManipulateMax && dft_out[m][0] > 0) {
            //if (freq >= mManipulateMin && freq < mManipulateMax) {
                if (mManipulateType == 0) { //peak
                    if (dft_out[m][0] > maxintensity) {
                        maxintensity=dft_out[m][0];
                        peakfreq=freq;
                        peakcount=1;
                    }
                }
                else {
                    peakfreq+=freq*dft_out[m][0];
                    peakcount+=dft_out[m][0]; //weighted average
                }
            }
            //(*o) << freq << " " << dft_out[m][0] << endl;
            //sprintf(s,"%17.9E %17.9E %17.9E %17.9E",
                  //freq,
                  //creal(dft_out[m]),
                  //cimag(dft_out[m]),
                  //norm * dft_out[m] * conj(dft_out[m]));
            //
            //cout << s << endl;
        }
        fftw_free(Z);
        fftw_free(dft_out);
        fftw_destroy_plan(dft_plan);
        if (peakcount == 0) (*o) << (mManipulateMin+mManipulateMax)/2;
        else (*o) << peakfreq/peakcount;
    }

    if (ingroupid < mpGroup->size()-1) (*o) << " ";
    else (*o) << endl;
#endif
}



void ActionTVACF::setManipulateIndividualTCFs(bool a, double min, double max, int type) {
    mManipulateIndividualTCFs=a;
    mManipulateMin=min;
    mManipulateMax=max;
    mManipulateType=type;
}
