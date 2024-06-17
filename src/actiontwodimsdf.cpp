#include "actiontwodimsdf.h"

ActionTwoDimSDF::ActionTwoDimSDF()
:Action(ActionType::TwoDimSDF,2,2,1,1), mMaxX(10), mMaxY(10), mXResolution(0.1), mYResolution(0.1), mAngleType(AngleType::XY) 
{
}

void ActionTwoDimSDF::addGroup(Group *g) {
    mvpGroups.push_back(g);
    mpGroup=mvpGroups[0];
    if (mvpGroups.size() > 1) {
        mpToGroup = mvpGroups[1];
    }
}

void ActionTwoDimSDF::internalAction(const vector<Atom> *atoms, const Timestep *timestep) {

    //switch (mAngleType) {
    //    case AngleType::XY: cout << "XY ! "; break;
    //    case AngleType::YZ: cout << "YZ ! "; break;
    //    case AngleType::XZ: cout << "XZ ! "; break;
    //}
    for (int i=0; i<mpGroup->size(); i++) {
        float x1=0; float y1=0;
        switch (mAngleType) {
            case AngleType::XY: x1=mpGroup->atom(i)->x; y1=mpGroup->atom(i)->y; break;
            case AngleType::YZ: x1=mpGroup->atom(i)->y; y1=mpGroup->atom(i)->z; break;
            case AngleType::XZ: x1=mpGroup->atom(i)->x; y1=mpGroup->atom(i)->z; break;
            case AngleType::ZX: x1=mpGroup->atom(i)->z; y1=mpGroup->atom(i)->x; break;
        }
        for (int j=0; j<mpToGroup->size(); j++) {
            if (mpToGroup->atom(j) == mpGroup->atom(i)) continue;
            float rawx=mpToGroup->atom(j)->x, rawy=mpToGroup->atom(j)->y, rawz=mpToGroup->atom(j)->z;
            float x2=0, y2=0, dummy=0;
            TranslateAtomCoordinatesToVicinity(rawx, rawy, rawz, mpGroup->atom(i)->x, mpGroup->atom(i)->y, mpGroup->atom(i)->z, timestep);
            switch (mAngleType) {
                case AngleType::XY: x2=rawx; y2=rawy; break;
                case AngleType::YZ: x2=rawy; y2=rawz; break;
                case AngleType::XZ: x2=rawx; y2=rawz; break;
                case AngleType::ZX: x2=rawz; y2=rawx; break;
            }
            float dx=x2-x1;
            float dy=y2-y1;
            if (dx >= -mMaxX && dx < mMaxX && dy >=-mMaxY && dy < mMaxY) {
                int xbin=int(mData.size()*(dx-(-mMaxX))/(mMaxX-(-mMaxX)));
                //if (dx < 0 && xbin > 0) xbin--;
                int ybin=int(mData[xbin].size()*(dy-(-mMaxY))/(mMaxY-(-mMaxY)));
                //if (dy < 0 && ybin > 0) ybin--;
                mData[xbin][ybin]++;
            }
        }
    }
}
void ActionTwoDimSDF::internalFinalAction(const vector<Atom> *atoms, const Timestep *timestep) { 
    internalPrint(atoms, timestep);
}
void ActionTwoDimSDF::internalInitialize(const vector<Atom>*, const Timestep *timestep) {
    int numxbins=(mMaxX-(-mMaxX))/mXResolution + 1;
    int numybins=(mMaxY-(-mMaxY))/mYResolution + 1;
    mData.resize(numxbins);
    for (int i=0; i<mData.size(); i++) {
        mData[i].resize(numybins, 0);
    }
}
void ActionTwoDimSDF::internalPrint(const vector<Atom>*, const Timestep *timestep) { 
    mpOut->close();
    mpOut->open(mvOutFilenames.front());
    for (int i=0; i<mData.size(); i++) {
        for (int j=0; j<mData[i].size(); j++) {
            long long int x = mData[i][j];
            //if (mpGroup == mpToGroup) x/=2;
            (*mpOut) << -mMaxX+(i+0.5)*mXResolution << " " << -mMaxY+(j+0.5)*mYResolution << " " << x << "\n";
        }
    }
    (*mpOut) << flush;
    mpOut->close();
}
