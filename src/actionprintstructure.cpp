#include "actionprintstructure.h"
#include "formatter.h"
#include <stdexcept>
#include "utility.h"
#include "groupcoordination.h"
#include <iomanip>
#define ANGSTROMTOBOHR (1.0/0.5291772109)

ActionPrintStructure::ActionPrintStructure() 
: Action(ActionType::PrintStructure, 1, 1, 1, 1),
mPrintFormat(StructureFormat::mattixyz),
mPrintEmptyFrames(true),
mCenterOn(-1),
mCenterOnId(false),
mPrintIndividualCoordinationEnvironments(false),
mPrintPolyhedra(false),
mPrintUnwrapped(false),
mCenterAtOrigin(false),
mActualPrints(0), mMaxActualPrints(0),
mScaleBy(1), mScaleByMax(1),
mCenterOnX(0), mCenterOnY(0), mCenterOnZ(0), mCenterOnXYZ(false),
mFirstCenterOnStaticXYZ(false),
mCTScaleBy(0), mMinSize(0) //value of 0 means use mScaleBy also for the coordination table
//mFillCoordinationTableGroup(nullptr)
{
}

ActionPrintStructure::~ActionPrintStructure() { }

void ActionPrintStructure::internalFinalAction(const vector<Atom> *allatoms, const Timestep *t) {
    closeOutputFiles();
}
void ActionPrintStructure::actualPrint(const vector<Atom> *allatoms, const Timestep *t, const Group *g) {
    const Atom *centerAtom=nullptr;
    stringstream ss;
    Atom dummyatom; dummyatom.x=mCenterOnX; dummyatom.y=mCenterOnY; dummyatom.z=mCenterOnZ;
    if (mCenterOnXYZ) {
        centerAtom = &dummyatom;
    }
    //if the centeron variable is >= 0, translate coordinates around the atom with that id/groupindex
    else if (mCenterOn >= 0) {
        if (mCenterOnId == false) {
            if (mCenterOn >= g->size()) throw std::runtime_error(Formatter() << "Error in group " << g->getId() << ": centeron = " << mCenterOn << " but size() = " << g->size());
            centerAtom = g->atom(mCenterOn); 
        }
        else { //look through the group to find the atom with the ID that corresponds to "centeron"
            int ingroupid = g->getMemberId(mCenterOn);
            if (ingroupid < 0) throw std::runtime_error(Formatter() << "Error in group " << g->getId() << ": atom with ID " << mCenterOn << " is not a member, but was requested by CenterOn!");
            centerAtom = g->atom(ingroupid); 
        }
        if (mFirstCenterOnStaticXYZ) {
            setCenterOnXYZ(true, centerAtom->x, centerAtom->y, centerAtom->z);
        }
    }
    int numatoms=0;
    double scaleby=mScaleBy;
    if (mScaleByMax != 1) {
        scaleby=mScaleBy + (mScaleByMax-mScaleBy)*rand()*1.0/RAND_MAX;
    }
    for (int i=0; i<g->size(); i++) {
        const Atom* a=g->atom(i);
        float px=a->x, py=a->y, pz=a->z;
        if (mPrintUnwrapped) {
            px = a->xu; py = a->yu; pz = a->zu;
        }
        string typestring("");
        bool doprint=true;
        if (centerAtom != nullptr) {
            TranslateAtomCoordinatesToVicinity(px, py, pz, centerAtom->x, centerAtom->y, centerAtom->z, t);
            if (mCenterAtOrigin) {
                px-=centerAtom->x;
                py-=centerAtom->y;
                pz-=centerAtom->z;
            }
        }
        px*=scaleby; py*=scaleby; pz*=scaleby;
        //if (mFillCoordinationTableGroup != nullptr) {
        //    for (int j=0; j<mFillCoordinationTableGroup->size(); j++) {
        //        const Atom* aj=mFillCoordinationTableGroup->atom(j);
        //        // check if you're part of the COORDINATION TABLE of any of the atoms in mFillCoordinationTableGroup
        //        if (mFillCoordinationTableGroup->isMemberOfCoordinationShell(j, a)) {
        //            //now check if you're crossing the periodic boundary
        //            float npx=aj->x, npy=aj->y, npz=aj->z; //these are corodinates for the CENTRAL atom
        //            TranslateAtomCoordinatesToVicinity(npx, npy, npz, px, py, pz, t);
        //            if ( (npx < aj->x-0.001 || npx > aj->x+0.001) || (npy < aj->y-0.001 || npy > aj->y+0.001) || (npz < aj->z-0.001 || npz > aj->z+0.001) ) { //one of the cooridnates changed, so the coordination table is in a different periodic image from the central atom
        //                cout << "atom " << aj->id << " " << aj->type << " " << aj->x << " " << aj->y << " " << aj->z << " has cooridnation to " << a->id << " " << a->type << " " << a->x << " " << a->y << " " << a->z << endl;
        //                doprint=false;
        //                break;
        //            }
        //        }
        //    }
        //    //now check if you have a coordination table
        //    int ingroupid = mFillCoordinationTableGroup->getMemberId(a);
        //    if (ingroupid >= 0) {
        //        const Group* ct=mFillCoordinationTableGroup->getCoordinationTable(ingroupid);
        //        for (int j=0; j<ct->size(); j++) {
        //            const Atom* aj=ct->atom(j);
        //            float npx=aj->x, npy=aj->y, npz=aj->z; 
        //            TranslateAtomCoordinatesToVicinity(npx, npy, npz, px, py, pz, t);
        //            if ( (npx < aj->x-0.001 || npx > aj->x+0.001) || (npy < aj->y-0.001 || npy > aj->y+0.001) || (npz < aj->z-0.001 || npz > aj->z+0.001) ) { //one of the cooridnates changed, so the coordination table is in a different periodic image from the central atom
        //            //if (npx != aj->x || npy != aj->y || npz != aj->z) { //one of the cooridnates changed, so the coordination table is in a different periodic image from the central atom
        //                doprint=false;
        //                break;
        //            }
        //        }
        //    }
        //}
        typestring=getSpecial(a, mvSpecial);
        if (typestring=="") {
          typestring=a->type;
        }
        if (doprint) {
            numatoms++;
            if (mPrintFormat == StructureFormat::mattixyz || mPrintFormat == StructureFormat::xmolout) {
                ss << typestring << " " << px << " " << py << " " << pz << "\n";
            }
            if (mPrintFormat == StructureFormat::mattixyzdelta) {
                ss << typestring << " " << px << " " << py << " " << pz <<  " " << a->delta << "\n";
            }
            else if (mPrintFormat == StructureFormat::lammpsrdx) {
                string chargestring=getSpecial(a, mvSpecial2);
                if (chargestring=="") {
                    chargestring="0";
                }
                ss << i+1 << " " << typestring << " " << px << " " << py << " " << pz << " " << chargestring << "\n";

            }
            else if (mPrintFormat == StructureFormat::runnerwithforceweights) {
                string weightstring=getSpecial(a, mvSpecial2);
                ss << "atom " << px*ANGSTROMTOBOHR << " " << py*ANGSTROMTOBOHR << " " << pz*ANGSTROMTOBOHR << " " << typestring << " " << a->q;
                ss << " 0 " << a->fx*ANGSTROMTOBOHR << " " << a->fy*ANGSTROMTOBOHR << " " << a->fz*ANGSTROMTOBOHR;
                ss << " " << weightstring << "\n";

            }
        }
        if (mPrintPolyhedra && dynamic_cast<const GroupCoordination*>(g) != nullptr) {
            const Group *ct = static_cast<const GroupCoordination*>(g)->getCoordinationTable(i);
            for (int j=0; j<ct->size(); j++) {
                numatoms++;
                typestring=getSpecial(ct->atom(j), mvSpecial);
                if (typestring=="") typestring=ct->atom(j)->type;
                float npx=ct->atom(j)->x, npy=ct->atom(j)->y, npz=ct->atom(j)->z;
                TranslateAtomCoordinatesToVicinity(npx, npy, npz, px, py, pz, t);
                if (mCTScaleBy > 0) { npx*=mCTScaleBy; npy*=mCTScaleBy; npz*=mCTScaleBy; }
                else { npx*=mScaleBy; npy*=scaleby; npz*=scaleby; }
                if (mCTMinDist > 0) {
                    float dist=distance(px,py,pz, npx, npy, npz, t);
                    //cout << "before sclae dist = " << dist;
                    if (dist < mCTMinDist && dist>0) {
                        //cout << "  px = " << px << "  npx = " << npx;
                        npx=px + (npx-px)*mCTMinDist/dist;
                        npy=py + (npy-py)*mCTMinDist/dist;
                        npz=pz + (npz-pz)*mCTMinDist/dist;
                        //npx+=mCTMinDist-(npx-px)*mCTMinDist/dist; npy+=mCTMinDist-(npy-py)*mCTMinDist/dist; npz+=mCTMinDist-(npz-pz)*mCTMinDist/dist;
                        //cout << "    npx*= " << mCTMinDist/dist;
                    }
                    //cout << "   after: " << distance(px,py,pz,npx,npy,npz,t) << endl;
                }
                ss << typestring << " " <<  npx << " " << npy << " " << npz << "\n";
            }
        }
    }
    if (mPrintFormat == StructureFormat::lammpsrdx) {
        (*mpOut) << "ITEM: TIMESTEP" << endl << t->iteration << endl;
        (*mpOut) << "ITEM: NUMBER OF ATOMS" << endl << g->size() << endl;
        (*mpOut) << "ITEM: BOX BOUNDS pp pp pp" << endl;
        (*mpOut) << "0 " << t->cellx << endl;
        (*mpOut) << "0 " << t->celly << endl;
        (*mpOut) << "0 " << t->cellz << endl;
        (*mpOut) << "ITEM: ATOMS id type x y z q" << endl;
    }
    else if (mPrintFormat == StructureFormat::mattixyz || mPrintFormat == StructureFormat::mattixyzdelta) {
        (*mpOut) << numatoms << endl;
        (*mpOut) << "XYZ " << t->cellx*scaleby << " " << t->celly*scaleby << " " << t->cellz*scaleby << endl;
    }
    else if (mPrintFormat == StructureFormat::xmolout) {
        (*mpOut) << numatoms << endl;
        (*mpOut) << "XYZ " << t->number << " " << t->energy << " " << t->cellx << " " << t->celly << " " << t->cellz << " 90 90 90" << endl;
    }
    else if (mPrintFormat == StructureFormat::runnerwithforceweights || mPrintFormat == StructureFormat::runner) {
        (*mpOut) << "begin" << endl;
        (*mpOut) << "lattice " << t->cellx*scaleby*ANGSTROMTOBOHR << " 0.0 0.0" << endl;
        (*mpOut) << "lattice 0.0 " << t->celly*scaleby*ANGSTROMTOBOHR<< " 0.0" << endl;
        (*mpOut) << "lattice 0.0 0.0 " << t->cellz*scaleby*ANGSTROMTOBOHR << endl;
    }
    (*mpOut) << ss.str();
    if (mPrintFormat == StructureFormat::runnerwithforceweights || mPrintFormat == StructureFormat::runner) {
        std::streamsize ss = std::cout.precision();
        (*mpOut) << "energy " << std::fixed << std::setprecision(6) << t->energy << std::setprecision(ss) << endl;
        (*mpOut) << "end" << endl;

    }
}

void ActionPrintStructure::internalAction(const vector<Atom> *allatoms, const Timestep *t) {
    if (mpGroup->size() < mMinSize) return;
    if (mMaxActualPrints>0 && mActualPrints >= mMaxActualPrints ) return;
    if (mpGroup->size()==0) {
        if (mPrintEmptyFrames && (mPrintFormat == StructureFormat::mattixyz || mPrintFormat == StructureFormat::xmolout)) {
            mActualPrints++;
            (*mpOut) << "1" << endl << "XYZ" << endl << "H 0 0 0" << endl; //single H atom to satisfy gdis
        }
        return;
    }
    if (mPrintIndividualCoordinationEnvironments) {
        //cout << "FDKJFS";
        GroupExternal *g= new GroupExternal;
        for (int i=0; i<mpGroup->size(); i++) {
            g->clear();
            g->addAtom(mpGroup->atom(i));
            GroupCoordination *cg=dynamic_cast<GroupCoordination*>(mpGroup);
            if (cg != nullptr) {
                const Group *ct=cg->getCoordinationTable(i);
                for (int j=0; j<ct->size(); j++) {
                    g->addAtom(ct->atom(j));
                }
            }
            mActualPrints++;
            actualPrint(allatoms, t, g);
            if (mMaxActualPrints>0 && mActualPrints >= mMaxActualPrints ) break;
        }
        delete g;
    }
    else {
        mActualPrints++;
        actualPrint(allatoms, t, mpGroup);
    }

}

void ActionPrintStructure::updateDescription() {
    stringstream ss("");
    ss << "ActionPrintStructure for group " << mvpGroups[0]->getId() << endl;
    setDescription(ss.str());
}

string ActionPrintStructure::getSpecial(const Atom* a, const vector<GroupAndString>& v) {
    for (auto &gas : v) {
        const Group *group=gas.first;
        int ingroupid = group->getMemberId(a);
        if (ingroupid >= 0) {
            return gas.second;
        }
    }
    return "";
}

//void ActionPrintStructure::setFillCoordinationTableGroup(Group *g) {
//    GroupCoordination *a=dynamic_cast<GroupCoordination*>(g);
//    if (a==nullptr) throw string("ERROR! Expected group with coordination tbale for FillCoordinationTable");
//    mFillCoordinationTableGroup=a;
//}
