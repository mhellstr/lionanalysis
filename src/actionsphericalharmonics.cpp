#include "actionsphericalharmonics.h"
#include "groupcoordination.h"
#include "utility.h"
#include <iomanip>
#ifndef NO_BOOST
#include <boost/math/special_functions/spherical_harmonic.hpp>
#endif

ActionSphericalHarmonics::ActionSphericalHarmonics() 
: Action(ActionType::SphericalHarmonics, 1, 1, 1, 1), mOutputLines(0), mMaxOrderParameter(10), mCompareToLibrary(false), mMaxLibraryError(-1)
{ }

void ActionSphericalHarmonics::internalAction(const vector<Atom> *allatoms, const Timestep *timestep) {
#ifndef NO_BOOST
    //cout << "inside action!" << endl;
    for (int j=0; j<mvGroupRanges.size(); j++) {
        mvGroupRanges[j].group->clear();
    }
    for (int i=0; i<mpGroup->size(); i++) {
        //vector vOrderParameters(mMaxOrderParameter, 0);
        const Atom *a=mpGroup->atom(i);
        float ax=a->x, ay=a->y, az=a->z;
        const Group *ct=dynamic_cast<GroupCoordination*>(mpGroup)->getCoordinationTable(i);
        //cout << " i = " << i << " ct = " << ct->size() << endl;
        if (ct->size() <=0) continue;
        (*mpOut) << mOutputLines << " " << timestep->iteration << " " << a->id;
        vector<double> vFinalValues(1,0); //initialize zero-element, so that you can access vFinalValues[orderparameter] later
        for (int order_parameter=1; order_parameter <=10; order_parameter++) {
            double sum_over_m=0;
            for (int m=-order_parameter; m<=order_parameter; m++) {
                std::complex<double> qlm=0;
                for (int j=0; j<ct->size(); j++) {
                    const Atom *b=ct->atom(j);
                    float bx=b->x, by=b->y, bz=b->z;
                    TranslateAtomCoordinatesToVicinity(bx, by, bz, ax, ay, az, timestep);
                    float r=sqrt( (ax-bx)*(ax-bx) + (ay-by)*(ay-by) + (az-bz)*(az-bz) );
                    float z=bz-az;
                    float y=by-ay;
                    float x=bx-ax;
                    float polar_colatidunal = acos(z/r);
                    float azimuthal_longitudinal = atan2(y,x);
                    std::complex<double> res=boost::math::spherical_harmonic(order_parameter, m, polar_colatidunal, azimuthal_longitudinal);
                    qlm += res;
                    //cout << ct->atom(j)->id << " " << x << " " << y << " " << z << " ; " << polar_colatidunal << " " << azimuthal_longitudinal << " " << res <<  " j = " << j << endl;
                } //for j in ct
                //cout << endl;
                qlm /= ct->size();
                sum_over_m += abs(qlm)*abs(qlm);
            } // for m
            double final_value = sqrt( sum_over_m * (4*M_PI)/(2*order_parameter+1));
            (*mpOut) << " " << final_value;
            vFinalValues.push_back(final_value);
        } //for order_parameter

        if (mCompareToLibrary) compareToLibrary(vFinalValues, i);
        else doGroupRanges(vFinalValues, i);

        (*mpOut) << endl; mOutputLines++;

    } //for i in mpGroup
    for (int j=0; j<mvGroupRanges.size(); j++) {
        mvGroupRanges[j].group->update(nullptr, nullptr);
    }
#endif
} //ActionSphericalHarmonics::internalAction



void ActionSphericalHarmonics::doGroupRanges(const vector<double>& vFinalValues, int i) { //i is the ingroupid of the atom in mpGroup
    for (int j=0; j<mvGroupRanges.size(); j++) {
        //cout << "i = " << i << " mvGroupRanges.size() = " << mvGroupRanges.size();
        bool success=true;
        for (int k=0; k<mvGroupRanges[j].range.size(); k++) {
            //cout << " op = " << mvGroupRanges[j].range[k].op << " min = " << mvGroupRanges[j].range[k].min << " max = " << mvGroupRanges[j].range[k].max << " actual3 " << vFinalValues[3] << " actual4 " << vFinalValues[4] << endl;
            if ( (vFinalValues[mvGroupRanges[j].range[k].op] < mvGroupRanges[j].range[k].min) ||
                 (vFinalValues[mvGroupRanges[j].range[k].op] > mvGroupRanges[j].range[k].max) ) {
                success=false;
                //cout << "BREAKING" << endl;
                break;
            }
        }
        if (success) {
            //cout << "SUCCESS" << endl;
            int newingroupid=mvGroupRanges[j].group->addAtom(mpGroup->atom(i));
            mvGroupRanges[j].group->addCoordinationFromOtherGroup(newingroupid, static_cast<GroupCoordination*>(mpGroup), i);
            (*mpOut) << " " << mvGroupRanges[j].group->getId();
        }
        else {
            //cout << " NO SUCCESS" << endl;
            //break;
            continue;
        }
    }
}
void ActionSphericalHarmonics::compareToLibrary(const vector<double>& vFinalValues, int i) { //i is the ingroupid of the atom in mpGroup
    double minerror=100000000000;
    int suggestion=0;
    for (int j=0; j<mvGroupRanges.size(); j++) { //each element contains one group and one vector "range"
        double totalerror=0;
        double thissize=0, refsize=0;
        for (int k=0; k<mvGroupRanges[j].range.size(); k++) {
            int op=mvGroupRanges[j].range[k].op;
            double refval=mvGroupRanges[j].range[k].min;
            double thisval=vFinalValues[op];
            double err=(thisval-refval)*(thisval-refval);
            //if (err<0) err=-err;
            totalerror+=err;
            thissize+=thisval*thisval;
            refsize+=refval*refval;
        }
        //double mae = totalerror/mvGroupRanges[j].range.size();
        double mae = sqrt(totalerror)/sqrt(thissize+refsize);
        if (mae < minerror) {
            minerror=mae;
            suggestion=j;
        }
    }
    if (minerror < mMaxLibraryError || mMaxLibraryError < 0) {
        int j=suggestion;
        int newingroupid=mvGroupRanges[j].group->addAtom(mpGroup->atom(i));
        mvGroupRanges[j].group->addCoordinationFromOtherGroup(newingroupid, static_cast<GroupCoordination*>(mpGroup), i);
        (*mpOut) << " " << mvGroupRanges[j].group->getId()  << " " << minerror;
    }
    else {
        (*mpOut) << " " << "???" << minerror;
    }
}
