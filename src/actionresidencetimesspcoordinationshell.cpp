#include "actionresidencetimesspcoordinationshell.h"
#include "tcfresidencetimessp.h"
/*
 * the ParentGroup (mpGroup) contains all atoms that might at one point be either a Reactant or a Product ("center" of a reactant or product)
 * the ShellGroup contains all atoms that might at some point be coordinated by a Reactant or Product in a bond whose lifetime is to measured 
 * mpReactants is a group with a coordination shell. The members of mpReactants are the "center", and the members of the coordination shell the "shell".
 * mpProducts is also a group with a coordination shell. The members of mpProducts are the "center", and the members of the cooridnation shell the "shell".
 * SSP always uses "old must be member" (this is done in TCFResidenceTimeSSP class)
 * There are a number of possibilities for determining when to calculate the correlation, and when to count the "product" has having been formed
 * I. Once two specific atoms (center->shell) form a bond in mpReactants, follow these two atoms until they form a bond in mpProducts, regardless of any "chemical" changes to these two atoms
 *      this means that mpProducts must be defined as a bond from ParentGroup->ShellGroup in the DefineGroup command.
 *      EXAMPLE:
 *          DefineGroup WaterH__EigenO BOND WaterH EigenO MinDist 1.5 MaxDist 2.2
 *          DefineGroup H___O BOND H O MinDist 2.8 MaxDist 1000
 *          ResidenceTimeSSPCoordinationShell Reactants WaterH__EigenO Products H___O ParentGroup H ShellGroup O
 *      Once a WaterH__EigenO bond is formed, follow the specific *atom pair*, even if the WaterH were to become EigenH and/or the EigenO were to become WaterO
 * II. Demand that the "center" chemical identity is something specific (e.g. that it remains a WaterH). If the identity changes, then possibility a or b:
 *      a) Do not correlate for that time *or any later time (?)*
 *          we tell the TCF not to care by addCare(false)
 *          but we do NOT say that the product has been formed: add2(0)
 *          we're obviously not a reactant so add1(0) (this means that whenever this becomes t0, we're not gonna care because of oldmustbemember, AND we're not gonna care because of the previous addCare(false))
 *          EXAMPLE:
 *              DefineGroup WaterH__EigenO BOND WaterH EigenO MinDist 1.5 MaxDist 2.2
 *              ResidenceTimeSSPCoordinationShell Reactants WaterH__EigenO Products ??? ParentGroup H ShellGroup O CenterMustBeOtherwiseUncare  WaterH
 *      b) Count the product as having been formed
 *          add1(0), add2(1), addCare(true)
 *          EXAMPLE:
 *              DefineGroup WaterH__EigenO BOND WaterH EigenO MinDist 1.5 MaxDist 2.2
 *              ResidenceTimeSSPCoordinationShell Reactants WaterH__EigenO Products ??? ParentGroup H ShellGroup O CenterMustBeOtherwiseProduct  WaterH
 * III. Demand tha tthe "shell" chemical identity is something specific (e.g., that it remain an EigenO). If the identity changes, then possibility a or b:
 *      a) Do not correlate for that time *or any later time (?)*
 *          "If I'm not in the product, I CAN'T check the shell", so if you want the ShellMustBe to play its proper role you probably want to DefineGroup products BOND ParentGroup->someshell
 *              ResidenceTimeSSPCoordinationShell Reactants WaterH__EigenO Products ??? ParentGroup H ShellGroup O ShellMustBeOtherwiseUncare  WaterH
 *      b) Count the product as having been formed
 */

ActionResidenceTimeSSPCoordinationShell::ActionResidenceTimeSSPCoordinationShell() 
: ActionResidenceTimeSSP(), mpShellGroup(nullptr), mpCenterMustBe(nullptr), mpShellMustBe(nullptr), mCenterMustBeOtherwiseConsequence(Consequence::Product), mShellMustBeOtherwiseConsequence(Consequence::Product)
{
    setActionType(ActionType::ResidenceTimeSSPCoordinationShell);
    mMinGroups=4; mMaxGroups=6;
}
void ActionResidenceTimeSSPCoordinationShell::internalMoreAtomsThanExpected(int numatoms) {
    mNumItemsPerTimeStep = mpShellGroup->size();
}

void ActionResidenceTimeSSPCoordinationShell::internalAddToTCF(int zerobasedid, const Atom* a) {
    //this function is called from ActionTCF::push_back_in_data
    // in ActionTCF::push_back_in_data, there is a loop for all atoms in mpGroup (the ParentGroup, encompassing all possible reactants and products)
    //The present function is called for each atom in ParentGroup; each atom is associated with a TCF in mTCF, that is mTCF[zerobasedid]
    if (mpReactants->isMember(a)) {
        const Group* coordinationtable = static_cast<GroupCoordination*>(mpReactants)->getCoordinationTable(mpReactants->getMemberId(a));
        for (int j=0; j<mpShellGroup->size(); j++) { 
            if (coordinationtable->isMember(mpShellGroup->atom(j))) {
                //cout << "R mTCF[" << zerobasedid << "]->add(1) (j =" << j << endl;
                //if (mpShellGroup->atom(j)->id == 49) { cout << " atom 49: is REACTANT" << endl; }
                mTCF[zerobasedid]->add(1);
            }
            else {
                //cout << "R mTCF[" << zerobasedid << "]->add(0) (j=" << j << endl;
                //if (mpShellGroup->atom(j)->id == 49) { cout << " atom 49: is NOT reactant " << endl; }
                mTCF[zerobasedid]->add(0);
            }
        }
    }
    else {
        for (int j=0; j<mpShellGroup->size(); j++) {
            //cout << "R mTCF[" << zerobasedid << "]->add(0) ( j = " << j << endl;
            mTCF[zerobasedid]->add(0);
        }
    }
    if (mpProducts->isMember(a)) {
        const Group* coordinationtable = static_cast<GroupCoordination*>(mpProducts)->getCoordinationTable(mpProducts->getMemberId(a));
        for (int j=0; j<mpShellGroup->size(); j++) { 
            if (coordinationtable->isMember(mpShellGroup->atom(j))) {
                //if (mpShellGroup->atom(j)->id == 49) { cout << " atom 49: is PRODUCT " << endl; }
                mTCF[zerobasedid]->add2(1);
            }
            else if (mpShellMustBe != nullptr && ! mpShellMustBe->isMember(mpShellGroup->atom(j))) { 
                // mpShellGroup is a kind of "catch-all" group that includes ALL possible bonding partners
                // for example, mpShellGroup will be ALL O atoms
                // but suppose you want to calculate the lifetime of a HydroxideO bound to Na
                // then if the bound O changes from HydroxideO to WaterO, you might like to count this as the ``product'' having been formed
                // (where the ``product'' means that the reactant is GONE)
                // then set mpShellMustBe to HydroxideO, and as soon as the (bound) HydroxideO changes to a WaterO, the reactant will be GONE
                // (assuming no overlap between HydroxideO and WaterO)
                // even if ShellMustBeOtherwiseUncare is set, I can still add 1 to mTCF->mInData2; the uncaring is done below
                // "If i'm not in the product, I can't check the shell" (here we are in the product since mpProducts->isMember(a)
                mTCF[zerobasedid]->add2(1);
            }
            else if (mpCenterMustBe != nullptr && ! mpCenterMustBe->isMember(a)) {
                mTCF[zerobasedid]->add2(1);
            }
            else {
                //if (mpShellGroup->atom(j)->id == 49) { cout << " atom 49: is NOT PRODUCT " << endl; }
                mTCF[zerobasedid]->add2(0);
            }
            if (mpShellMustBe != nullptr && ! mpShellMustBe->isMember(mpShellGroup->atom(j)) && mShellMustBeOtherwiseConsequence == Consequence::Uncare) { 
                static_cast<TCFResidenceTimeSSP*>(mTCF[zerobasedid])->addCare(false);
            }
            else if (mpCenterMustBe != nullptr && ! mpCenterMustBe->isMember(a) && mCenterMustBeOtherwiseConsequence == Consequence::Uncare) {
                static_cast<TCFResidenceTimeSSP*>(mTCF[zerobasedid])->addCare(false);
            }
            else {
                static_cast<TCFResidenceTimeSSP*>(mTCF[zerobasedid])->addCare(true);
            }
        }
    }
    else { //this should only rarely (if ever?) happen.... at least should pretty much neverh appen in my NaOH simulations
        int val=0;
        bool care=true;
        if (mpCenterMustBe != nullptr && ! mpCenterMustBe->isMember(a)) {
            val=1;
            if (mCenterMustBeOtherwiseConsequence == Consequence::Uncare) {
                care=false;
            }
        }
        for (int j=0; j<mpShellGroup->size(); j++) {
            mTCF[zerobasedid]->add2(val);
            static_cast<TCFResidenceTimeSSP*>(mTCF[zerobasedid])->addCare(care);
        }
    }
}

/*
 * This idea didn't work
void ActionResidenceTimeSSPCoordinationShell::internalAddToTCF(int zerobasedid, const Atom* a) {
    //mTCF contains only ONE number. THAT IS
    //IMPORTANT: this assumes that the coordination tables are either empty or contain only ONE element
    //the user must figure out a way to define the groups so that this gives the desired result
    float reacvalue=0, prodvalue=0;
    if (mpReactants->isMember(a)) {
        const Group* coordinationtable = static_cast<GroupCoordination*>(mpReactants)->getCoordinationTable(mpReactants->getMemberId(a));
        if (coordinationtable->size() >=1) {
            reacvalue=coordinationtable->atom(0)->id;
            if (coordinationtable->size() >1) {
                cout << "WARNING: The coordinationtable for atom " << a->id << " " << a->type << " in Group " << mpReactants->getId() << " is >1 in Action: " << getDescription() << " this will lead to errors!"; << endl;
            }
        }
    }
    if (mpProducts->isMember(a)) {
        const Group* coordinationtable = static_cast<GroupCoordination*>(mpProducts)->getCoordinationTable(mpProducts->getMemberId(a));
        if (coordinationtable->size() >=1) {
            prodvalue=coordinationtable->atom(0)->id;
            if (coordinationtable->size() >1) {
                cout << "WARNING: The coordinationtable for atom " << a->id << " " << a->type << " in Group " << mpProducts->getId() << " is >1 in Action: " << getDescription() << " this will lead to errors!"; << endl;
            }
        }
    }
    mTCF[zerobasedid].add(reacvalue, prodvalue);
    if (reacvalue != 0 && prodvalue != 0) {
        cout << "WARNING: Atom " << a->type << " " << a->id << " is a member of BOTH reactants " << mpReactants->getId() << " and products " << mpProducts->getId() << " for action " << getDescription() << " - (i.e. coordination tables are equivalent) - check your input!" << endl;
    }
}
*/
void ActionResidenceTimeSSPCoordinationShell::addGroup(Group *g) {
    mvpGroups.push_back(g);
    mpGroup=mvpGroups[0]; //should be a CONST SIZE group (static). This is the group that includes all possible reactants and all possible products
    if (mvpGroups.size() > 1) {
        mpReactants = dynamic_cast<GroupCoordination*>(mvpGroups[1]); //can be variably sized, no problem
        if (mpReactants == nullptr) {
            throw string("Error in ActionResidenceTimeCoordinationShell::addGroup(); second group to be added needs to be of type GroupCoordination - " + mvpGroups[1]->getId());
        }
    }
    if (mvpGroups.size() > 2) {
        mpProducts = dynamic_cast<GroupCoordination*>(mvpGroups[2]); //can be variably sized, no problem
        if (mpProducts == nullptr) {
            throw string("Error in ActionResidenceTimeCoordinationShell::addGroup(); second group to be added needs to be of type GroupCoordination - " + mvpGroups[2]->getId());
        }
    }
    if (mvpGroups.size() > 3) {
        mpShellGroup = mvpGroups[3]; //should be a CONST SIZE group (static). This is the group that the reactants and products are coordinated to
        if (mvpGroups.size() > 4) {
            mpCenterMustBe = mvpGroups[4];
            if (mvpGroups.size() > 5) {
                mpShellMustBe = mvpGroups[5];
            }
        }
    }
}
