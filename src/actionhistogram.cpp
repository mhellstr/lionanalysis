#include "actionhistogram.h"
#include "groupcoordination.h"

ActionHistogram::ActionHistogram()
: Action(ActionType::Histogram,1,100,1,1), mMinVal(0), mResolution(1), mPrintHeader(true)
{
}

ActionHistogram::~ActionHistogram()
{
}

void ActionHistogram::add(double x, int v) {
    mHistogram.add(x, v);
}

void ActionHistogram::internalAction(const vector<Atom> *allatoms, const Timestep *timestep)
{
    for (int iGroup=0; iGroup<mvpGroups.size(); iGroup++) {
        Group *g=mvpGroups[iGroup];
        for (int j=0; j<g->size(); j++) {
            const Atom* a=g->atom(j);
            for (int k=0; k<mvProperties.size(); k++) {
                switch (mvProperties[k]) {
                    case GROUP_PROPERTY::X: add(a->x); break;
                    case GROUP_PROPERTY::Y: add(a->y); break;
                    case GROUP_PROPERTY::Z: add(a->z); break;
                    case GROUP_PROPERTY::DELTA: 
                        if (mDataType == HistogramDataType::CoordinationNumber) {
                            add(a->delta, dynamic_cast<GroupCoordination*>(g)->getCoordinationNumber(j));
                        }
                        else {
                            add(a->delta); 
                        }
                        break;
                    case GROUP_PROPERTY::COORDINATIONNUMBER: add(dynamic_cast<GroupCoordination*>(g)->getCoordinationNumber(j)); break;
                    case GROUP_PROPERTY::BONDS:
                        {
                            const Group* coordinationtable=dynamic_cast<GroupCoordination*>(g)->getCoordinationTable(j);
                            for (int l=0; l<coordinationtable->size(); l++) {
                                add(a->bonds[coordinationtable->atom(l)->id]);
                            }
                        }
                        break;
                    case GROUP_PROPERTY::BOND0: add(dynamic_cast<GroupCoordination*>(g)->getShortestBond(j, 0)); break;
                    case GROUP_PROPERTY::BOND1: add(dynamic_cast<GroupCoordination*>(g)->getShortestBond(j, 1)); break;
                    case GROUP_PROPERTY::BOND2: add(dynamic_cast<GroupCoordination*>(g)->getShortestBond(j, 2)); break;
                    case GROUP_PROPERTY::BOND3: add(dynamic_cast<GroupCoordination*>(g)->getShortestBond(j, 3)); break;
                }
            }
        }
        for (int k=0; k<mvProperties.size(); k++) {
            switch (mvProperties[k]) {
                case GROUP_PROPERTY::NUMATOMS: add(g->size()); break;
            }
        }
    }
}

void ActionHistogram::internalPrint(const vector<Atom> *allatoms, const Timestep *timestep) {
    mpOut->close();
    mpOut->open(mvOutFilenames.front());
    long long int sum=mHistogram.sum();
    long long int sumcount=mHistogram.sumCount();
    //long long int totalsize=0;
    //for (int i=0; i<mvpGroups.size(); i++) {
        //totalsize+=mvpGroups[i]->getTotalSize();
    //}

    //get the weighted average
    //double wavg=0; long long
    //for (int i=0; i<mHistogram.size(); i++) {
        //wavg+=mHistogram.indexToReal(i)
    //}
    if (mPrintHeader) {
        (*mpOut) << "#value #count #fractional #divbycount " + getDescription() + "# mCount = " << mCount << " # sum = " << sum << " # sumcount = " << sumcount << " # wavg = " << mHistogram.weightedAverage() << "\n";
    }
    for (int i=0; i<mHistogram.size(); i++) {
        (*mpOut) << mHistogram.indexToReal(i) << " " << mHistogram.getElement(i) << " " << mHistogram.getElement(i) * 1.0f / sum << " " << mHistogram.getElement(i) * 1.0f / mHistogram.getCount(i) << "\n";
    }
    mpOut->close();
}
void ActionHistogram::internalFinalAction(const vector<Atom> *allatoms, const Timestep *timestep)
{
    internalPrint(allatoms, timestep);
}

void ActionHistogram::setResolution(double x) {
    mHistogram.setResolution(x);
}
void ActionHistogram::setMinVal(double x) {
    mHistogram.setMinVal(x);
}
void ActionHistogram::setMaxVal(double x) {
    mHistogram.setMaxVal(x);
}
void ActionHistogram::setDynamicRange(bool a) {
    mHistogram.setDynamicRange(a);
}
