#include "tcfresidencetimesspcoordinationshell.h"

/*
TCFResidenceTimeSSPCoordinationShell::TCFResidenceTimeSSPCoordinationShell()
: TCFResidenceTime(TCF_t::ResidenceTimeSSPCoordinationShell)
{
}

float TCFResidenceTimeSSPCoordinationShell::internalCorrelateWithLatest(unsigned int oldindex, unsigned int newindex) const {
    // mInData1 will contain the ATOM ID of the coordination table (which is assumed to consist of only one atom) of the reactant atom
    // if mInData1[oldindex] == 0, then this means that the atom was not a member of  the group at the time
    float val=0;
    if (mInData1[oldindex] == mInData2[newindex] && mInData1[oldindex]!=0) { //these are floating point numbers! bit of a dangerous comparison, but should work fine unless we have MANY atoms in the system (stackoverflow suggests 16777216 atoms is the limit)
        //the reactant at time oldindex successfully converted into the product at time newindex
        val = 0;
    }
    else {
        val = 1;
    }
    return val;
    //val+=1 - mInData1[oldindex] * mInData2[newindex];
    return val;
}

bool TCFResidenceTimeSSPCoordinationShell::allowedIndices(int oldindex, int newindex) { //called from void TCF::correlateWithLatest(int externalmaxdt) 
    if (mOldMustBeMember == true) { 
        return mInData1[oldindex] >= 1; //atomic indices start at 1. if mInData1[oldindex]==0, then the atom was not a member of the group at the time
    }
    return true;
}
*/
