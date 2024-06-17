#include "actiondensity.h"
#include "coordinateaxis.h"

ActionDensity::ActionDensity() 
:Action(ActionType::Density,1,1,1,1), 
mDirection(CoordinateAxis::z), 
mResolution(0.1), mMinValue(-1), mMaxValue(10), 
mReadRangeFromTimestep(true), mPeriodic(true)
{ }

ActionDensity::~ActionDensity() {
    /*
    for (auto& x : mvpOuts) {
        cout << "actiondensity destructor: deleting " << mpOut << endl;
        x->close();
        delete x;
    }
    */
}

void ActionDensity::newAddOut(const string& filename) {
    cout << " creating ActionDensity:: ofstream :: " << filename << endl;
    mvpOuts.push_back(new ofstream(filename));
    mpOut = mvpOuts.front();
    cout << mpOut << endl;
}

void ActionDensity::internalAction(const vector<Atom>* allatoms, const Timestep *timestep) {
    float cell;
    switch (mDirection) {
        case CoordinateAxis::x: cell=timestep->cellx; break;
        case CoordinateAxis::y: cell=timestep->celly; break;
        case CoordinateAxis::z: cell=timestep->cellz; break;
        default: throw string("undefined coordinate axis");
    }
    if (mReadRangeFromTimestep == true) {
        if (mPeriodic==true) {
            mMinValue=0;
            mMaxValue=cell;
        }
        else {
            mMinValue=-10; //give some leeway for when atoms cross the periodic boundary
            mMaxValue=cell+10; //give some leeway for when atoms cross the periodic boundary
        }
    }
    int numbins=(mMaxValue - mMinValue)/mResolution + 1;
    if (numbins > mData.size()) mData.resize(numbins, 0);
    for (int i=0; i<mpGroup->size(); i++) {
        float val;
        if (mDirection == CoordinateAxis::z) val=mpGroup->atom(i)->z;
        else if (mDirection == CoordinateAxis::x) val=mpGroup->atom(i)->x;
        else if (mDirection == CoordinateAxis::y) val=mpGroup->atom(i)->y;
        if (mPeriodic==true) {
            if (val < 0) val+=cell;
            if (val >= cell) val-=cell;
        }
        int bin=int((val- mMinValue)/mResolution);
        if (bin < mData.size()) mData[bin]++;
        //if (bin < mData.size()) mData[bin]+=mpGroup->atom(i)->mass;
    }
}


void ActionDensity::internalPrint(const vector<Atom>* allatoms, const Timestep *timestep) {
    for (int i=0; i<mvpOuts.size(); i++) {
        mvpOuts[i]->close(); 
        mvpOuts[i]->open(mvOutFilenames[i]);
    }
    float area=0;
    if (mDirection == CoordinateAxis::x) area=(timestep->sumcelly*1.0/timestep->totaltimesteps)*(timestep->sumcellz*1.0/timestep->totaltimesteps);
    else if (mDirection == CoordinateAxis::y) area=(timestep->sumcellx*1.0/timestep->totaltimesteps)*(timestep->sumcellz*1.0/timestep->totaltimesteps);
    else if (mDirection == CoordinateAxis::z) area=(timestep->sumcellx*1.0/timestep->totaltimesteps)*(timestep->sumcelly*1.0/timestep->totaltimesteps);
    (*mpOut) << "#xyz raw rawpertimestep distfuncpertstep distfuncpertstepperarea " 
             << "# axis: " << coordinateAxisToString(mDirection) << " group: " << mpGroup->getId()
             << " timesteps: " << mCount << " avgarea: " << area << endl;
    for (int i=0; i<mData.size(); i++) {
        int rawcount=mData[i];
        float divided_by_timesteps=mData[i]*1.0/mCount;
        float divided_by_timesteps_and_resolution = divided_by_timesteps / mResolution;
        float divided_by_timesteps_and_resolution_and_area = divided_by_timesteps_and_resolution / area;
        (*mpOut) << mMinValue + i*mResolution << " " << rawcount << " " << divided_by_timesteps << " " << divided_by_timesteps_and_resolution << " " << divided_by_timesteps_and_resolution_and_area << "\n";
    }
}
void ActionDensity::internalFinalAction(const vector<Atom>* allatoms, const Timestep *timestep) {
    internalPrint(allatoms, timestep);
}
