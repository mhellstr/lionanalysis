#include "readconfigfile.h"
#include <complex.h>
#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <omp.h>
#include "constants.h"
#include "rdf.h"
#include "atom.h"
#include "timestep.h"
#include "utility.h"
#include "anglerdf.h"
#include "dihedralrdf.h"
#include "msd.h"
#include "vac.h"
#include <tclap/CmdLine.h>
#include "tcf.h"
#include "formatter.h"
#include "actionincludes.h"
#include "groupincludes.h"
#include <signal.h>
#include <unistd.h>
#include "options.h"
#include "readconfigfile.h"


void ReadConfigFile::ReadConfigFile(string configfilename, CMainOptions &o, vector<Action*>& vpActions, vector<Group*>& vpGroups, vector<CObject*>& vpObjects, vector<NamedConst>& vNamedConsts, vector<ifstream*>& vInputfiles) {
    ifstream config(configfilename);
    string s;
    int numconfiglines=0;

    bool keepreading=true;
    string origs;
    try {
        while (keepreading && getline(config,s)) {
            //ss.str(""); ss.clear(); ss << s << endl;
            //while (keepreading && getline(ss,s)) {
            numconfiglines++;
            trim(s);
            if (s.length() == 0) continue;
            if (s[0] == '#') continue; //only lines BEGINNING with # are comments
            origs=s;
            string s2=s;
            //if line ends with backslash, join with the next line
            while (s2.find_last_of("\\") == s2.length()-1) { //trim function makes sure that no spaces come after the backslash
                //if (!getline(ss,s2)) {
                    if (!getline(config, s2)) {
                        break;
                    }
                //};
                numconfiglines++;
                trim(s2);
                origs = origs.substr(0,origs.length()-1) + string(" ") + s2;
            }
            ReadConfigFile::ProcessSingleLine(origs, configfilename, o, vpActions, vpGroups, vpObjects, vNamedConsts, vInputfiles, keepreading);
        }
    }
    catch (string errorstring) {
        throw string(Formatter() << "ERROR! " << configfilename << " at line " << numconfiglines << " : " << errorstring << "\n" << " Original line: " << origs << "\n");
    }
}





void ReadConfigFile::ProcessSingleLine(const string& origorigs, string configfilename, CMainOptions &o, vector<Action*>& vpActions, vector<Group*>& vpGroups, vector<CObject*>& vpObjects, vector<NamedConst>& vNamedConsts, vector<ifstream*>& vInputfiles, bool& keepreading) {
    Group              *pGroup              =  nullptr;
    GroupAtomicNumber  *pGroupAtomicNumber  =  nullptr;
    GroupBond          *pGroupBond          = nullptr;
    GroupExternal      *pGroupExternal      =  nullptr;
    GroupList          *pGroupList          =  nullptr;
    GroupRead          *pGroupRead          =  nullptr;
    GroupRegion        *pGroupRegion        =  nullptr;
    GroupSubGroup      *pGroupSubGroup      =  nullptr;
    GroupSubGroupCN    *pGroupSubGroupCN    =  nullptr;
    GroupSubGroupRandom    *pGroupSubGroupRandom    =  nullptr;
    GroupMemberHistory *pGroupMemberHistory = nullptr;
    GroupFindShortest *pGroupFindShortest = nullptr;
    GroupInvertCoordination *pGroupInvertCoordination = nullptr;
    GroupAddCoordination *pGroupAddCoordination = nullptr;
    GroupIncludeCoordination *pGroupIncludeCoordination = nullptr;
    Action                 *pAction                 =  nullptr;
    ActionPrintStructure   *pActionPrintStructure(nullptr);
    ActionPrintProperties  *pActionPrintProperties  =  nullptr;
    ActionFindVacuum *pActionFindVacuum = nullptr;
    ActionFinalShell       *pActionFinalShell       =  nullptr;
    ActionRDF *pActionRDF = nullptr;
    ActionAngleRDF *pActionAngleRDF = nullptr;
    ActionDihedralRDF *pActionDihedralRDF = nullptr;
    ActionMSD *pActionMSD = nullptr;
    ActionVACF *pActionVACF = nullptr;
    ActionResidenceTime *pActionResidenceTime = nullptr;
    ActionResidenceTimeSSP *pActionResidenceTimeSSP = nullptr;
    ActionResidenceTimeContinuous *pActionResidenceTimeContinuous = nullptr;
    ActionResidenceTimeCoordinationShell *pActionResidenceTimeCoordinationShell = nullptr;
    ActionResidenceTimeSwitchBond *pActionResidenceTimeSwitchBond = nullptr;
    ActionSumVelocities *pActionSumVelocities = nullptr;
    ActionDensity *pActionDensity = nullptr;
    ActionTimeDensity *pActionTimeDensity = nullptr;
    ActionCheckOverlap *pActionCheckOverlap = nullptr;
    ActionShortDelta *pActionShortDelta = nullptr;
    ActionDoubleCoordinationShortDelta *pActionDoubleCoordinationShortDelta = nullptr;
    ActionSphericalHarmonics *pActionSphericalHarmonics = nullptr;
    ActionTResidenceTime *pActionTResidenceTime  = nullptr;
    ActionTMSD *pActionTMSD = nullptr;
    ActionTVACF *pActionTVACF = nullptr;
    ActionTResidenceTimeSSP *pActionTResidenceTimeSSP = nullptr;
    ActionTTCFParent *pActionTTCFParent = nullptr;
    ActionTwoDimSDF *pActionTwoDimSDF = nullptr;
    ActionChangeGroupTime *pActionChangeGroupTime = nullptr;
    ActionHistogram *pActionHistogram = nullptr;
    ActionWhenGroupChanged *pActionWhenGroupChanged = nullptr;
    ActionWhenGroupChangedDefineGroup *pActionWhenGroupChangedDefineGroup = nullptr;
    ActionDoubleShortDelta *pActionDoubleShortDelta = nullptr;
    ActionSharedLigands *pActionSharedLigands = nullptr;
    ActionMoveCenter *pActionMoveCenter = nullptr;
    RDF *pRDF = nullptr;
    AngleRDF *pAngleRDF = nullptr;
    DihedralRDF *pDihedralRDF = nullptr;
    ofstream *pOut = nullptr;
    string s1("");

    vector<string> w, origw;
    string s,s2;
    int numconfiglines=0;
    int i1,i2,i3,i4;
    float f1,f2,f3,f4;
    int mincoord,maxcoord,coord,every,printevery=0;
    float mindist,maxdist;


    Group *group1(nullptr);
    Group *group2(nullptr);
    Group *group3(nullptr);
    Group *group4(nullptr), *group5(nullptr), *group6(nullptr);
    string grouptypestring,groupname,groupname1,groupname2,groupname3,groupname4;
    string totalfilename;
    string myindent("     ");
    GROUP_TYPE grouptype;
    bool bstatic=false; //static group?
    bool bool1=false,bool2=false,bool3=false,bool4=false;
    vector<bool> vtBool(64, false);
    vector<int>  vtInt(64, 0);
    vector<float> vtFloat(64, 0.0);
    vector<string> vtString(64, "");
    vector<Group *> vtGroup(64, nullptr);

    string origs = origorigs;
    trim(origs);

    //convert to lowercase, everything is case insensitive
    s = origs;
    for (int i=0; i<origs.length(); i++) {
        s[i] = tolower(origs[i]);
    }
    w.clear(); origw.clear();
    stow(s, w); stow(origs, origw);
    if (w.size() == 0) return;
    if (w[0] == "readfile" || w[0] == "rf" || (w[0] == "loop" && w[1] != "varindex") || w[0] == "singleenvironment" || w[0] == "hydrogenbond" || w[0] == "averagesize") cout << "    ### ";
    if (w[0] != "loop" || w[1] != "varindex") cout << origs << endl; //print the original line (origs, origw are case sensitive)
    //cout << "w[0] = " << w[0] << endl;
    if (w[0] == "definegroup" || w[0] == "modifygroup") {
        string originalgroupname=w[1];
        groupname=w[1];
        grouptypestring=w[2];
        if (w[0] == "modifygroup") {
            Group *tempg = FindGroupWithId(vpGroups, groupname);
            groupname="XXMODIFYXX"+groupname;;
        }
        ActionType actiontype=ActionType::DefineGroup;
        if (grouptypestring == "atomicnumber") {
            pGroupAtomicNumber = new GroupAtomicNumber(groupname);
            pGroupAtomicNumber->setTargetAtomType(origw[3]);
            for (int i=4; i<w.size(); i++) {
                if (w[i] == "static") { pGroupAtomicNumber->setStatic(true); }
                else if (w[i] == "mass" && w.size()>i+1) { float x; from_string<float>(x, w[i+1]); pGroupAtomicNumber->setTargetAtomMass(x); i++; }
                else throw string("Something is wrong with atomicnumber command: ")+origw[4];
            }
            addGroup(vpGroups, pGroupAtomicNumber);
        }
        else if (grouptypestring == "external") {
            grouptype=GROUP_TYPE::EXTERNAL;
            addGroup(vpGroups, new GroupExternal(groupname));
        }
        else if (grouptypestring == "findshortest") {
            pGroupFindShortest = new GroupFindShortest(groupname);
            group1=nullptr; group2=nullptr;
            for (int i=3; i<w.size(); i++) {
                if (w[i] == "fromgroup" && w.size()>i+1) {
                    group1=FindGroupWithId(vpGroups, w[i+1]);
                    i++;
                }
                else if (w[i] == "togroup" && w.size()>i+1) {
                    group2=FindGroupWithId(vpGroups, w[i+1]);
                    i++;
                }
                else if (w[i] == "static") {
                    pGroupFindShortest->setStatic(true);
                }
            }
            if (group1 == nullptr || group2 == nullptr) throw string("ERROR! GroupFindShortest requires FromGroup AND ToGroup!");
            pGroupFindShortest->addConstituentGroup(group1);
            pGroupFindShortest->addConstituentGroup(group2);
            addGroup(vpGroups, pGroupFindShortest);
        }
        else if (grouptypestring == "invertcoordination") {
            grouptype=GROUP_TYPE::INVERTCOORDINATION;
            group1=dynamic_cast<GroupCoordination*>(FindGroupWithId(vpGroups, w[3]));
            if (group1 == nullptr) throw string("invertcoordination needs group of type groupcoordination");
            pGroupInvertCoordination = new GroupInvertCoordination(groupname);
            pGroupInvertCoordination->addConstituentGroup(group1);
            addGroup(vpGroups, pGroupInvertCoordination);
        }
        else if (grouptypestring == "addcoordination") {
            grouptype=GROUP_TYPE::ADDCOORDINATION;
            group1=dynamic_cast<GroupCoordination*>(FindGroupWithId(vpGroups, w[3]));
            if (group1 == nullptr) throw string("invertcoordination needs group of type groupcoordination");
            group2=dynamic_cast<GroupCoordination*>(FindGroupWithId(vpGroups, w[4]));
            if (group2 == nullptr) throw string("invertcoordination needs group of type groupcoordination");
            pGroupAddCoordination = new GroupAddCoordination(groupname);
            pGroupAddCoordination->addConstituentGroup(group1);
            pGroupAddCoordination->addConstituentGroup(group2);
            addGroup(vpGroups, pGroupAddCoordination);
        }
        else if (grouptypestring == "includecoordination") {
            grouptype=GROUP_TYPE::INCLUDECOORDINATION;
            group1=dynamic_cast<GroupCoordination*>(FindGroupWithId(vpGroups, w[3]));
            if (group1 == nullptr) throw string("includecoordination needs group of type groupcoordination");
            pGroupIncludeCoordination = new GroupIncludeCoordination(groupname);
            pGroupIncludeCoordination->addConstituentGroup(group1);
            addGroup(vpGroups, pGroupIncludeCoordination);
        }
        else if (grouptypestring == "list") {
            pGroupList = new GroupList(groupname);
            for (int i=3; i<w.size(); i++) {
                int atomid;
                if (w[i] == "static") { pGroupList->setStatic(true); }
                else {
                    from_string<int>(atomid, w[i]);
                    if (atomid <=0) throw string("DefineGroup type LIST requires atomic ids >=1");
                    pGroupList->addAtomId(atomid);
                }
            }
            addGroup(vpGroups, pGroupList);
            //cout << myindent << "Created list group " << groupname << endl;
        }
        else if (grouptypestring == "read") {
            string gfilename(origw[3]);
            vInputfiles.push_back(new ifstream(gfilename.c_str()));
            pGroupRead = new GroupRead(groupname);
            pGroupRead->setReadStream(vInputfiles.back());
            addGroup(vpGroups, pGroupRead);
        }
        else if (grouptypestring == "memberhistory") {
            pGroupMemberHistory = new GroupMemberHistory(groupname);
            group1=nullptr; group2=nullptr; int maxhistory=10; int minmembertime=0; int maxmembertime=1000000;
            group3=nullptr;
            for (int i=3; i<w.size(); i++) {
                if (w[i] == "parentgroup") {
                    group1=FindGroupWithId(vpGroups, w[i+1]);
                    i++;
                }
                else if (w[i] == "memberof" || w[i]=="historygroup") {
                    group2=FindGroupWithId(vpGroups, w[i+1]);
                    i++;
                }
                else if (w[i] == "drawfrom") {
                    group3=FindGroupWithId(vpGroups, w[i+1]);
                    i++;
                }
                else if (w[i] == "maxhistory") {
                    from_string<int>(maxhistory, w[i+1], &vNamedConsts);
                    i++;
                }
                else if (w[i] == "minmembertime") {
                    from_string<int>(minmembertime, w[i+1], &vNamedConsts);
                    i++;
                }
                else if (w[i] == "maxmembertime") {
                    from_string<int>(maxmembertime, w[i+1], &vNamedConsts);
                    i++;
                }
            }
            if (group1 == nullptr) throw string("Must specify ParentGroup for MEMBERHISTORY");
            if (group2 == nullptr) throw string("Must specify MemberOf for MEMBERHISTORY");
            if (group3 == nullptr) throw string("Must specify DrawFrom for MEMBERHISTORY");
            pGroupMemberHistory->setParentGroup(group1);
            pGroupMemberHistory->setMemberOfGroup(group2);
            pGroupMemberHistory->setDrawFromGroup(group3);
            pGroupMemberHistory->setMaxHistory(maxhistory);
            pGroupMemberHistory->setMinMemberTime(minmembertime);
            pGroupMemberHistory->setMaxMemberTime(maxmembertime);
            addGroup(vpGroups, pGroupMemberHistory);
        }
        else if (grouptypestring == "subgroup") {
            groupname1=w[3];
            group1=FindGroupWithId(vpGroups, groupname1);
            pGroupSubGroup = new GroupSubGroup(groupname);
            pGroupSubGroup->setParentGroup(group1);

            for (int i=4; i<w.size(); i++) {
                int atomid;
                if (w[i] == "static") { pGroupSubGroup->setStatic(true); }
                else {
                    from_string<int>(atomid, w[i]);
                    pGroupSubGroup->addAtomId(atomid);//these "atom ids" ARE NOT the atom ids! but a number list 0,1,2,3 that correspond to the corresponding elements in the parent group
                }
            }
            addGroup(vpGroups, pGroupSubGroup);
        }
        else if (grouptypestring == "subgrouprandom") {
            groupname1=w[3];
            group1=FindGroupWithId(vpGroups, groupname1);
            pGroupSubGroupRandom = new GroupSubGroupRandom(groupname);
            pGroupSubGroupRandom->setParentGroup(group1);
            int nummembers=1;
            for (int i=4; i<w.size(); i++) {
                if (w[i] == "static") { pGroupSubGroupRandom->setStatic(true); }
                else if (w[i] == "nummembers") {
                    from_string<int>(nummembers, w[i+1], &vNamedConsts);
                    pGroupSubGroupRandom->setNumElements(nummembers);
                    i++;
                }
            }
            addGroup(vpGroups, pGroupSubGroupRandom);
        }
        else if (grouptypestring == "subgroupcn") {
            groupname1=w[3];
            group1=FindGroupWithId(vpGroups, groupname1);
            if (nullptr == dynamic_cast<GroupCoordination*>(group1)) {
                throw string("ERROR! specified parent group " + group1->getId() + " for SUBGROUPCN " + groupname + " does not contain any coordination information. The parent group should be defined either via a BOND-type directive or from an AngleRDF (NewGroup/CopyGroup)");
            }
            pGroupSubGroupCN = new GroupSubGroupCN(groupname);
            pGroupSubGroupCN->setParentGroup(dynamic_cast<GroupCoordination*>(group1));
            mincoord=0; maxcoord=10000;
            for (int i=4; i<w.size(); i++) {
              if (w[i] == "static") { pGroupSubGroupCN->setStatic(true); }
              else if (w[i] == "mincoord") { from_string<int>(mincoord, w[i+1], &vNamedConsts); i++; }
              else if (w[i] == "maxcoord") { from_string<int>(maxcoord, w[i+1], &vNamedConsts); i++; }
              else if (w[i] == "coord") { from_string<int>(mincoord, w[i+1], &vNamedConsts); maxcoord=mincoord; i++; }
              else throw string("Something is wrong with SUBGROUPCN command");
            }
            pGroupSubGroupCN->setMinCoord(mincoord);
            pGroupSubGroupCN->setMaxCoord(maxcoord);
            addGroup(vpGroups, pGroupSubGroupCN);
        }
        else if (grouptypestring == "bond") {
            groupname1=w[3];
            groupname2=w[4];
            groupname3="";
            //groupname4="";
            group1=FindGroupWithId(vpGroups, groupname1); //FROM GROUP
            group2=FindGroupWithId(vpGroups, groupname2);// TO GROUP
            //group3=nullptr; //CORRESPONDING GROUP
            pGroupExternal=nullptr; //CORRESPONDING GROUP
            mindist=0.0; maxdist=2.0; mincoord=1; maxcoord=10000;
            bstatic=false;
            bool inheritdelta=false, includetogroup=false;
            bool bsavecoordinationtable=false;
            bool bcopygroup=false;
            for (int i=5; i<w.size(); i++) {
                if ( (w[i] == "mindist"  || w[i] == ">") && w.size()>i+1) {        from_string<float>(mindist, w[i+1], &vNamedConsts); i++; }
                else if ( (w[i] == "maxdist" || w[i] == "<") && w.size()>i+1) {   from_string<float>(maxdist, w[i+1], &vNamedConsts); i++; }
                else if (w[i] == "static") { bstatic=true; }
                else if ( (w[i] == "mincoord" || w[i] == "#>") && w.size()>i+1) {  from_string<int>(mincoord, w[i+1]); i++; }
                else if ( (w[i] == "maxcoord" || w[i] == "#<") && w.size()>i+1) {  from_string<int>(maxcoord, w[i+1]); i++; }
                else if ( (w[i] == "coord" || w[i] == "#") && w.size()>i+1) {     from_string<int>(coord, w[i+1]);
                                                                mincoord=coord; maxcoord=coord; i++; 
                                                          }
                //else if (w[i] == "savecoordinationtable") { bsavecoordinationtable=true; }
                else if (w[i] == "corresponding" && w.size()>i+1) { groupname3=w[i+1]; i++; }
                else if (w[i] == "inheritdelta") { inheritdelta=true; }
                else if (w[i] == "includetogroup") { includetogroup = true; }
                //else if (w[i] == "corrsum" && w.size()>i+1) { groupname4=w[i+1]; i++; }
                else { throw string(" Something is wrong with the DefineGroup command...")+origw[i]; }
            }
            if (groupname3!="") { //corresponding group
              pGroupExternal = new GroupExternal(groupname3);
              pGroupExternal->setStatic(bstatic); 
              addGroup(vpGroups, pGroupExternal);
            }
            pGroupBond = new GroupBond(groupname);
            pGroupBond->setFromGroup(group1);
            pGroupBond->setToGroup(group2);
            pGroupBond->setMinDist(mindist);
            pGroupBond->setMaxDist(maxdist);
            pGroupBond->setMinCoord(mincoord);
            pGroupBond->setMaxCoord(maxcoord);
            pGroupBond->setInheritDelta(inheritdelta);
            pGroupBond->setIncludeToGroup(includetogroup);
            //pGroupBond->setSaveCoordinationTable(bsavecoordinationtable);
            pGroupBond->setStatic(bstatic);
            pGroupBond->setCorrespondingGroup(pGroupExternal);
            addGroup(vpGroups, pGroupBond);
        } //if grouptypestring == "bond"
        else if (grouptypestring == "sum" || grouptypestring=="diff" || grouptypestring=="intersection" || grouptypestring=="coordinationtableintersection") {
            if (grouptypestring == "sum") pGroup = new GroupSum(groupname);
            else if (grouptypestring == "diff") {
                cout << "diff group1" << endl;
                pGroup = new GroupDiff(groupname);
            }
            else if (grouptypestring == "intersection") pGroup = new GroupIntersection(groupname);
            else if (grouptypestring == "coordinationtableintersection") pGroup = new GroupCoordinationTableIntersection(groupname);
            bool bstatic=true, requiresdistances=false;
            //cout << "BEFORE loop" << endl;
            for (int i=3; i<w.size(); i++) {
                group1=FindGroupWithId(vpGroups, w[i]);
                pGroup->addConstituentGroup(group1);
                if (group1->isStatic() == false) bstatic=false;
                if (group1->getRequiresDistances() == true) requiresdistances=true;
            }
            //cout << "BEFORE static" << endl;
            pGroup->setStatic(bstatic);
            //cout << "ADTEr static" << endl;
            pGroup->setRequiresDistances(requiresdistances);
            addGroup(vpGroups, pGroup);
        }
        else if (grouptypestring == "region") {
            float maxx=100000,maxy=100000,maxz=100000,minx=-100000,miny=-100000,minz=-100000,mindelta=-10000,maxdelta=10000;
            bool bstatic=false, normalcoordinatebounds=false;
            for (int i=3; i<w.size(); i++) {
                if (w[i] == "static") { bstatic=true; }
                else if (w[i] == "maxx" && w.size()>i+1) { from_string<float>(maxx, w[i+1], &vNamedConsts); i++; }
                else if (w[i] == "maxy" && w.size()>i+1) { from_string<float>(maxy, w[i+1], &vNamedConsts); i++; }
                else if (w[i] == "maxz" && w.size()>i+1) { from_string<float>(maxz, w[i+1], &vNamedConsts); i++; }
                else if (w[i] == "minx" && w.size()>i+1) { from_string<float>(minx, w[i+1], &vNamedConsts); i++; }
                else if (w[i] == "miny" && w.size()>i+1) { from_string<float>(miny, w[i+1], &vNamedConsts); i++; }
                else if (w[i] == "minz" && w.size()>i+1) { from_string<float>(minz, w[i+1], &vNamedConsts); i++; }
                else if (w[i] == "mindelta" && w.size()>i+1) { from_string<float>(mindelta, w[i+1], &vNamedConsts); i++; }
                else if (w[i] == "maxdelta" && w.size()>i+1) { from_string<float>(maxdelta, w[i+1], &vNamedConsts); i++; }
                else if (w[i] == "normalcoordinatebounds") {normalcoordinatebounds=true;}
                else throw string("Something is wrong in DefineGroup REGION: " + origw[i]);
            }
            pGroupRegion = new GroupRegion(groupname);
            pGroupRegion->setMinX(minx);
            pGroupRegion->setMinY(miny);
            pGroupRegion->setMinZ(minz);
            pGroupRegion->setMaxX(maxx);
            pGroupRegion->setMaxY(maxy);
            pGroupRegion->setMaxZ(maxz);
            pGroupRegion->setMinDelta(mindelta);
            pGroupRegion->setMaxDelta(maxdelta);
            pGroupRegion->setNormalCoordinateBounds(normalcoordinatebounds);
            pGroupRegion->setStatic(bstatic);
            addGroup(vpGroups, pGroupRegion);
        }
        else {
            throw string("Unknown group type: ")+grouptypestring;
        }
        //pAction = new ActionSimple(actiontype);
        pAction = new ActionDefineGroup;
        //pAction->addObject(&vGroups[vGroups.size()-1]);
        //cout << "adding group " << vpGroups[vpGroups.size()-1]->getId() << " to action ";
        pAction->addObject(vpGroups.back());
        pAction->setDescription(origs);
        addAction(vpActions, pAction);

        if (w[0] == "modifygroup") {
            Group *tempg = FindGroupWithId(vpGroups, originalgroupname);
            long long int modcount=0;
            while (FindGroupWithIdNoThrow(vpGroups, originalgroupname + std::to_string(modcount)) != nullptr) {
                modcount++;
            }
            tempg->setId(originalgroupname + std::to_string(modcount));
            vpGroups.back()->setId(originalgroupname);
        }

    } //end if w[0] == definegroup || modifygroup
    else if (w[0] == "quickdefine" || w[0] == "qd") {
        groupname=w[1];
        pGroupAtomicNumber = new GroupAtomicNumber(groupname);
        int atomicnumber=stringAtomToNumAtom(groupname);
        pGroupAtomicNumber->setTargetAtomType(origw[1]);
        pGroupAtomicNumber->setTargetAtomMass(getAtomMass(atomicnumber));
        pGroupAtomicNumber->setStatic(true);
        addGroup(vpGroups, pGroupAtomicNumber);
        pAction = new ActionDefineGroup;
        pAction->addObject(vpGroups.back());
        pAction->setDescription(origs);
        addAction(vpActions, pAction);
    }
    else if (w[0] == "checkoverlap") {
        groupname1=w[1];
        group1=FindGroupWithId(vpGroups, groupname1);
        if (dynamic_cast<GroupSum*>(group1) == nullptr) {
            throw string("ERROR! Need IntersectionGroup for CheckOverlap");
        }
        string msdfilename=origw[2];
        totalfilename=o.outputfileprefix+msdfilename+o.outputfilesuffix;
        if (!permission_to_write(totalfilename, o.overwritefiles)) throw string("no permission to write to " + totalfilename);
        every=1;
        bool1=false; //printwarning
        for (int i=3; i<w.size(); i++) {
            if (w[i] == "every" && w.size()>i+1) { from_string<int>(every, w[i+1]); i++; }
            else if (w[i] == "warning") { bool1=true; }
            else throw string("Unknown option to CheckOverlap: "+ w[i]);
        }
        pActionCheckOverlap = new ActionCheckOverlap;
        pActionCheckOverlap->addGroup(group1);
        pActionCheckOverlap->addOut(totalfilename);
        pActionCheckOverlap->setEvery(every);
        pActionCheckOverlap->setPrintWarning(bool1);
        pActionCheckOverlap->setDescription(origs);
        addAction(vpActions, pActionCheckOverlap);
    }
    else if (w[0] == "printgroup") {
        groupname=w[1];
        string printgroupformatstring=w[2];
        StructureFormat printgroupformat;
        if (printgroupformatstring == "xyz" || printgroupformatstring == "mattixyz") printgroupformat=StructureFormat::mattixyz;
        else if (printgroupformatstring == "mattixyzdelta") printgroupformat=StructureFormat::mattixyzdelta;
        else if (printgroupformatstring == "xmolout") printgroupformat=StructureFormat::xmolout;
        else if (printgroupformatstring == "asexyz") printgroupformat=StructureFormat::asexyz;
        else if (printgroupformatstring == "lammpsrdx") printgroupformat=StructureFormat::lammpsrdx;
        else if (printgroupformatstring == "runner") printgroupformat=StructureFormat::runner;
        else if (printgroupformatstring == "runnerwithforceweights") printgroupformat=StructureFormat::runnerwithforceweights;
        else {
            throw string("ERROR! unknown PrintGroup format: " + printgroupformatstring);
        }
        string printgroupfilename=origw[3];
        vector<GroupAndString> vGAS;
        vector<GroupAndString> vGAScharge;
        int centeron=-1;
        every=1;
        bool bdistdiffgroup=false,centeronid=false,printindividualcoordinationenvironments=false;
        bool1=true;
        Group *fillcoordinationtablegroup=nullptr;
        bool printpolyhedra=false;
        double scaleby=1;
        double scalebymax=1;
        double ctscaleby=0;
        double ctmindist=0;
        bool centeratorigin=false;
        int minsize=0, maxsize=0;
        int maxactualprint=0;
        bool centeronxyz=false;
        float centeronx=0, centerony=0, centeronz=0;
        Group *ifgroupnotempty=nullptr, *ifgroupempty=nullptr;
        bool firstcenteronstaticxyz=false;
        bool printunwrapped=false;
        for (int i=4; i<w.size(); i++) {
            if (w[i] == "centeron" && w.size()>i+1) { from_string<int>(centeron, w[i+1], &vNamedConsts); i++; }
            if (w[i] == "every" && w.size()>i+1) { from_string<int>(every, w[i+1], &vNamedConsts); i++; }
            if (w[i] == "noemptyframes") { bool1=false; }
            if (w[i] == "minsize") { from_string<int>(minsize, w[i+1], &vNamedConsts); i++; }
            if (w[i] == "maxactualprint") { from_string<int>(maxactualprint, w[i+1], &vNamedConsts); i++; }
            if (w[i] == "centeronid") { centeronid = true; }
            if (w[i] == "centeratorigin") {centeratorigin=true;}
            if (w[i] == "centeronxyz") {
                centeronxyz=true;
                from_string<float>(centeronx, w[i+1], &vNamedConsts); i++;
                from_string<float>(centerony, w[i+1], &vNamedConsts); i++;
                from_string<float>(centeronz, w[i+1], &vNamedConsts); i++;
            }
            if (w[i] == "ifgroupnotempty") {ifgroupnotempty=FindGroupWithId(vpGroups, w[i+1]); ++i; }
            if (w[i] == "ifgroupempty") {ifgroupempty=FindGroupWithId(vpGroups, w[i+1]); ++i; }
            if (w[i] == "firstcenteronstaticxyz") { firstcenteronstaticxyz=true; }
            if (w[i] == "printindividualcoordinationenvironments") { printindividualcoordinationenvironments=true; }
            if (w[i] == "printpolyhedra") {printpolyhedra=true;}
            if (w[i] == "unwrapped") {printunwrapped=true;}
            if (w[i] == "scaleby" && w.size()>i+1) { from_string<double>(scaleby, w[i+1]); i++; }
            if (w[i] == "scalebymax" && w.size()>i+1) { from_string<double>(scalebymax, w[i+1]); i++; }
            if (w[i] == "ctscaleby" && w.size()>i+1) { from_string<double>(ctscaleby, w[i+1]); i++; }
            if (w[i] == "ctmindist" && w.size()>i+1) { from_string<double>(ctmindist, w[i+1]); i++; }
            //if (w[i] == "fillcoordinationtablegroup" && w.size()>i+1) { fillcoordinationtablegroup=FindGroupWithId(vpGroups, w[i+1]); i++; }
            if (w[i] == "special" && w.size()>i+1) { //special must be last setting on line
                int stepsize=2;
                if (printgroupformat == StructureFormat::lammpsrdx) stepsize=3;
                if (printgroupformat == StructureFormat::runnerwithforceweights) stepsize=5;
                for (int j=i+1; j<w.size(); j+=stepsize) {
                    groupname1=w[j];
                    group1=FindGroupWithId(vpGroups, groupname1);
                    string code=origw[j+1];
                    vGAS.push_back(GroupAndString(group1, code));
                    if (printgroupformat == StructureFormat::lammpsrdx) {
                        string chargecode=origw[j+2];
                        vGAScharge.push_back(GroupAndString(group1, chargecode));
                    }
                    else if (printgroupformat == StructureFormat::runnerwithforceweights) {
                        string weightcode = origw[j+2] + " " + origw[j+3] + " " + origw[j+4]; //these are the weights that will be printed after the forces
                        vGAScharge.push_back(GroupAndString(group1, weightcode));
                    }
                    //cout << "pushing back groupandstring " << groupname1 << " " << group1 << " " << code << endl;
                }
                break;
            }  
        }
        group1=FindGroupWithId(vpGroups, groupname);

        totalfilename=o.outputfileprefix+printgroupfilename+o.outputfilesuffix;
        if (!permission_to_write(totalfilename, o.overwritefiles)) throw string("no permission to write to " + totalfilename);
        pActionPrintStructure = new ActionPrintStructure;
        pActionPrintStructure->setEvery(every);
        pActionPrintStructure->addGroup(group1);
        pActionPrintStructure->addOut(totalfilename);
        pActionPrintStructure->setCenterOn(centeron);
        pActionPrintStructure->setCenterOnId(centeronid);
        pActionPrintStructure->setCenterAtOrigin(centeratorigin);
        pActionPrintStructure->setCenterOnXYZ(centeronxyz, centeronx, centerony, centeronz);
        pActionPrintStructure->setPrintEmptyFrames(bool1);
        pActionPrintStructure->setMinSize(minsize);
        pActionPrintStructure->setPrintUnwrapped(printunwrapped);
        pActionPrintStructure->setPrintFormat(printgroupformat);
        pActionPrintStructure->setPrintIndividualCoordinationEnvironments(printindividualcoordinationenvironments);
        pActionPrintStructure->setPrintPolyhedra(printpolyhedra);
        pActionPrintStructure->setMaxActualPrint(maxactualprint);
        pActionPrintStructure->setIfGroupNotEmpty(ifgroupnotempty);
        pActionPrintStructure->setIfGroupEmpty(ifgroupempty);
        pActionPrintStructure->setFirstCenterOnStaticXYZ(firstcenteronstaticxyz);
        pActionPrintStructure->setScaleBy(scaleby);
        pActionPrintStructure->setScaleByMax(scalebymax);
        pActionPrintStructure->setCTScaleBy(ctscaleby); //0 means use the the scaleby for the central atoms as for the coordination table
        pActionPrintStructure->setCTMinDist(ctmindist); //0 means use the the scaleby for the central atoms as for the coordination table
        //pActionPrintStructure->setFillCoordinationTableGroup(fillcoordinationtablegroup);
        for (auto x : vGAS) {
            pActionPrintStructure->addSpecial(x);
        }
        for (auto x : vGAScharge) {
            pActionPrintStructure->addSpecial2(x);
        }
        pActionPrintStructure->setDescription(origs);
        addAction(vpActions, pActionPrintStructure);
    } //end if w[0] == printgroup
    else if (w[0] == string("printproperties") || w[0] == string("finalprintproperties") || w[0] == string("histogram")) {
        vector<Group*> mygroups;
        vector<GROUP_PROPERTY> myproperties;
        string ppfilename("");
        ppfilename=origw[1];
        bool1=false; //print timestep iteration
        bool2=false; //print timestep number
        bool3=false; //distdiffgroup
        bool4=false; //print number of entries per group for easier post-processing
        bool printcell=false;
        ActionPrintFormat printpropertiesformat=ActionPrintFormat::SingleLines;
        every=1;
        string printseparator("");
        double histominval=0, histomaxval=1, historesolution=1;
        bool histodynamic=false;
        int printevery=0;
        double flushevery=1000;
        Group *ifgroupnotempty=nullptr;
        HistogramDataType histodatatype=HistogramDataType::Count;
        for (int i=2; i<w.size(); i++) {
            if (w[i] == "every" && w.size()>i+1) { from_string<int>(every, w[i+1]); ++i; }
            else if (w[i] == "distdiffgroup") { bool3=true; }
            else if (w[i] == "numentries") { bool4=true; }
            else if (w[i] == "multiplelines") { printpropertiesformat = ActionPrintFormat::MultipleLines;}
            else if (w[i] == "newlineseparator") { printseparator="\n"; }
            else if (w[i] == "tabseparator") { printseparator="\t"; }
            else if (w[i] == "ifgroupnotempty") { 
                ifgroupnotempty=FindGroupWithId(vpGroups, w[i+1]);
                ++i;
            }
            else if (w[i] == "flushevery") { from_string<double>(flushevery, w[i+1], &vNamedConsts); ++i; }
            else if ((w[0] == "finalprintproperties" || w[0] == "histogram") && w[i] == "printevery" && w.size()>i+1) { from_string<int>(printevery, w[i+1], &vNamedConsts); ++i; }
            else if (w[0] == "histogram" && w[i] == "minval") { from_string<double>(histominval, w[i+1], &vNamedConsts); ++i; }
            else if (w[0] == "histogram" && w[i] == "maxval") { from_string<double>(histomaxval, w[i+1], &vNamedConsts); ++i; }
            else if (w[0] == "histogram" && w[i] == "resolution") { from_string<double>(historesolution, w[i+1], &vNamedConsts); ++i; }
            else if (w[0] == "histogram" && w[i] == "dynamicrange") { histodynamic=true; }
            else if (w[0] == "histogram" && w[i] == "datatypecoordinationnumber") { histodatatype = HistogramDataType::CoordinationNumber; }
            else if (w[i] == "groups") { //groups must appear before properties on the input line
                for (int j=i+1; j<w.size(); j++) {
                    if (w[j] == "properties") {
                        i=j;
                        break;
                    }
                    else {
                        groupname1=w[j];
                        group1=FindGroupWithId(vpGroups, w[j]);
                        mygroups.push_back(group1);
                    }
                }
            }
            else if (w[i] == "allgroups") {
                mygroups = vpGroups;
            }
            if (w[i] == "properties") { //important not to "else if" since i changes in previous if
                for (int j=i+1; j<w.size(); j++) {
                    GROUP_PROPERTY myproperty=StringToProperty(w[j]);
                    if (myproperty == GROUP_PROPERTY::TIMESTEPITERATION) bool1=true;
                    else if (myproperty == GROUP_PROPERTY::TIMESTEPNUMBER) bool2=true;
                    else if (myproperty == GROUP_PROPERTY::CELL) printcell=true;
                    else {
                        myproperties.push_back(myproperty);
                    }
                }
                break;
            }
        }
        totalfilename=o.outputfileprefix+ppfilename+o.outputfilesuffix;
        if (!permission_to_write(totalfilename, o.overwritefiles)) throw string("no permission to write to " + totalfilename);
        //ofstream *pOut = new ofstream(ppfilename.c_str());
        //vOutputfiles.push_back(pOut);
        if (w[0] == "printproperties") {
            pActionPrintProperties = new ActionPrintProperties;
            pActionPrintProperties->setFlushEvery(flushevery);
            pActionPrintProperties->setIfGroupNotEmpty(ifgroupnotempty);
        }
        else if (w[0] == "finalprintproperties") {
            pActionPrintProperties = new ActionFinalPrintProperties;
        }
        if (w[0] == "printproperties" || w[0] == "finalprintproperties") {
            pActionPrintProperties->addOut(totalfilename);
            for (auto x : mygroups) pActionPrintProperties->addGroup(x);
            for (auto x : myproperties) pActionPrintProperties->addProperty(x);
            pActionPrintProperties->setPrintFormat(printpropertiesformat);
            pActionPrintProperties->setPrintSeparator(printseparator);
            pActionPrintProperties->setPrintPropertiesTimestepIteration(bool1);
            pActionPrintProperties->setPrintPropertiesTimestepNumber(bool2);
            pActionPrintProperties->setPrintPropertiesCell(printcell);
            pActionPrintProperties->setDistDiffGroup(bool3);
            pActionPrintProperties->setPrintNumEntriesPerGroup(bool4);
            pActionPrintProperties->setDescription(origs);
            if (w[0] == "finalprintproperties") {
                pActionPrintProperties->setPrintEvery(printevery);
            }
            vpActions.push_back(pActionPrintProperties);
        }
        else if (w[0] == "histogram") {
            pActionHistogram = new ActionHistogram;
            pActionHistogram->addOut(totalfilename);
            for (auto x : mygroups) pActionHistogram->addGroup(x);
            for (auto x : myproperties) pActionHistogram->addProperty(x);
            pActionHistogram->setMinVal(histominval);
            pActionHistogram->setMaxVal(histomaxval);
            pActionHistogram->setResolution(historesolution);
            pActionHistogram->setDynamicRange(histodynamic);
            pActionHistogram->setDataType(histodatatype);
            pActionHistogram->setPrintEvery(printevery);
            pActionHistogram->setDescription(origs);
            vpActions.push_back(pActionHistogram);
        }
    }
    else if (w[0] == "whengroupchangeddefinegroup") {
        group3=nullptr; 
        groupname3=w[1];
        string id("");
        bstatic=false;
        int duration=1, delay=0;
        group1=nullptr; 
        for (int i=2; i<w.size(); i++) {
            if (w[i] == "id" && w.size()>i+1) {
                id=w[i+1]; i++;
            }
            else if (w[i] == "duration" && w.size()>i+1) { // NOT realtime
                from_string<int>(duration, w[i+1], &vNamedConsts);
                i++;
            }
            else if (w[i] == "delay" && w.size()>i+1) { // NOT realtime
                from_string<int>(delay, w[i+1], &vNamedConsts);
                i++;
            }
            else if (w[i] == "parentgroup" && w.size()>i+1) {
                group1=FindGroupWithId(vpGroups, w[i+1]);
                i++;
            }
            else throw string("Unknown option to WhenGroupChangedDefineGroup");
        }
        if (groupname3!="") { //corresponding group
          pGroupExternal = new GroupExternal(groupname3);
          addGroup(vpGroups, pGroupExternal);
          group3=FindGroupWithId(vpGroups, groupname3);
        }
        pActionWhenGroupChangedDefineGroup = new ActionWhenGroupChangedDefineGroup;
        pActionWhenGroupChangedDefineGroup->setId(id);
        pActionWhenGroupChangedDefineGroup->addGroup(group3);
        pActionWhenGroupChangedDefineGroup->setDuration(duration);
        pActionWhenGroupChangedDefineGroup->setDelay(delay);
        if (group1!=nullptr) {
            pActionWhenGroupChangedDefineGroup->setParentGroup(group1);
        }
        addAction(vpActions, pActionWhenGroupChangedDefineGroup);
    }
    else if (w[0] == "twodimsdf") {
        group1=nullptr; group2=nullptr;
        string msdfilename("");
        every=1;
        pActionTwoDimSDF = new ActionTwoDimSDF;
        double maxx=10, maxy=10, resolutionx=0.1, resolutiony=0.1;
        AngleType angletype=AngleType::XY;
        int printevery=1000;
        for (int i=1; i<w.size(); i++) {
            if (w[i] == "every" && w.size()>i+1) { from_string<int>(every, w[i+1], &vNamedConsts); i++; }
            else if (w[i] == "maxx") { from_string<double>(maxx, w[i+1], &vNamedConsts); ++i; }
            else if (w[i] == "maxy") { from_string<double>(maxy, w[i+1], &vNamedConsts); ++i; }
            else if (w[i] == "filename") { msdfilename=origw[i+1]; ++i; }
            else if (w[i] == "resolution") { from_string<double>(resolutionx, w[i+1], &vNamedConsts); resolutiony=resolutionx; ++i; }
            else if (w[i] == "xresolution") { from_string<double>(resolutionx, w[i+1], &vNamedConsts); ++i; }
            else if (w[i] == "yresolution") { from_string<double>(resolutiony, w[i+1], &vNamedConsts); ++i; }
            else if (w[i] == "xy") { angletype=AngleType::XY; }
            else if (w[i] == "yz") { angletype=AngleType::YZ; }
            else if (w[i] == "xz") { angletype=AngleType::XZ; }
            else if (w[i] == "fromgroup") { group1=FindGroupWithId(vpGroups, w[i+1]); ++i; }
            else if (w[i] == "togroup") { group2=FindGroupWithId(vpGroups, w[i+1]); ++i; }
            else if (w[i] == "printevery") { from_string<int>(printevery, w[i+1], &vNamedConsts); ++i; }
            else throw string("unknown option to TwoDimSDF!" + origw[i]);
        }
        pActionTwoDimSDF->setEvery(every);
        pActionTwoDimSDF->setMaxX(maxx);
        pActionTwoDimSDF->setMaxY(maxy);
        pActionTwoDimSDF->setXResolution(resolutionx);
        pActionTwoDimSDF->setYResolution(resolutiony);
        pActionTwoDimSDF->setAngleType(angletype);
        pActionTwoDimSDF->setPrintEvery(printevery);
        if (group1!=nullptr) pActionTwoDimSDF->addGroup(group1);
        if (group2!=nullptr) pActionTwoDimSDF->addGroup(group2);
        pActionTwoDimSDF->addOut(msdfilename);
        pActionTwoDimSDF->setDescription(origs);
        addAction(vpActions, pActionTwoDimSDF);
    }
    else if (w[0] == "movecenter") {
        double minval=0.9, maxfrac=0.5; double probabilitytomove=0.1, probabilitytochangeframe=1;
        group1=nullptr; string msdfilename("");
        every=1;
        for (int i=1; i<w.size(); i++) {
            if (w[i] == "every" && w.size()>i+1) { from_string<int>(every, w[i+1], &vNamedConsts); i++; }
            //else if (w[i] == "filename") { msdfilename=origw[i+1]; ++i; }
            else if (w[i] == "minval") { from_string<double>(minval, w[i+1], &vNamedConsts); ++i; }
            else if (w[i] == "maxfrac") { from_string<double>(maxfrac, w[i+1], &vNamedConsts); ++i; }
            else if (w[i] == "probabilitytomove") { from_string<double>(probabilitytomove, w[i+1], &vNamedConsts); ++i; }
            else if (w[i] == "probabilitytochangeframe") { from_string<double>(probabilitytochangeframe, w[i+1], &vNamedConsts); ++i; }
            else if (w[i] == "group") { group1=FindGroupWithId(vpGroups, w[i+1]); ++i; }
        }
        if (group1 == nullptr) throw string ("MoveCenter NEEEDS group");
        //if (msdfilename=="") throw string("MoveCenter NEEDS filename");
        pActionMoveCenter = new ActionMoveCenter;
        pActionMoveCenter->setEvery(every);
        pActionMoveCenter->setMinVal(minval);
        pActionMoveCenter->setMaxFrac(maxfrac);
        pActionMoveCenter->setProbabilityToMove(probabilitytomove);
        pActionMoveCenter->setProbabilityToChangeFrame(probabilitytochangeframe);
        pActionMoveCenter->addGroup(group1);
        //pActionMoveCenter->addOut(msdfilename);
        pActionMoveCenter->setDescription(origs);
        addAction(vpActions, pActionMoveCenter);
    }
    else if (w[0] == "whengroupchanged") {
        group1=nullptr;
        group2=nullptr;
        group3=nullptr;
        string msdfilename(""), successfulout("");
        string id="";
        every=1;
        double beforetime=0, aftertime=0, resolution=0;
        bool realtime=false, explicitly_set_timeunit=false, explicitly_set_groupproperty;
        int printevery=0; float timeunit=1; int minproducttime=-1;
        GROUP_PROPERTY prop;
        pActionWhenGroupChanged = new ActionWhenGroupChanged;
        for (int i=1; i<w.size(); i++) {
            if (w[i] == "every" && w.size()>i+1) {
                from_string<int>(every, w[i+1], &vNamedConsts);
                i++;
            }
            else if (w[i] == "id" && w.size()>i+1) {
                id=w[i+1];
                i++;
            }
            //else if (w[i] == "resolution" && w.size()>i+1) { //control the resolution using Every instead
            //    from_string<double>(resolution, w[i+1], &vNamedConsts);
            //    i++;
            //}
            else if (w[i] == "beforetime" && w.size()>i+1) {
                from_string<double>(beforetime, w[i+1], &vNamedConsts);
                i++;
            }
            else if (w[i] == "aftertime" && w.size()>i+1) {
                from_string<double>(aftertime, w[i+1], &vNamedConsts);
                i++;
            }
            else if (w[i] == "printevery" && w.size()>i+1) {
                from_string<int>(printevery, w[i+1], &vNamedConsts);
                i++;
            }
            else if (w[i] == "filename" && w.size()>i+1) {
                msdfilename=w[i+1];
                i++;
            }
            else if (w[i] == "successfulout" && w.size()>i+1) {
                successfulout=w[i+1];
                i++;
            }
            else if (w[i] == "realtime" && w.size()>i+1) {
                realtime=true;
            }
            else if (w[i] == "property" && w.size()>i+1) {
                prop = StringToProperty(w[i+1]);
                explicitly_set_groupproperty=true;
                i++;
            }
            else if (w[i] == "productmustbe" && w.size()>i+1) {
                group2 = FindGroupWithId(vpGroups, w[i+1]);
                i++;
            }
            else if (w[i] == "reactantmustbe" && w.size()>i+1) {
                group3 = FindGroupWithId(vpGroups, w[i+1]);
                i++;
            }
            else if (w[i] == "minproducttime" && w.size()>i+1) {
                from_string<int>(minproducttime, w[i+1], &vNamedConsts);
                i++;
            }
            else if (w[i] == "timeunit" && w.size()>i+1) { 
                from_string<float>(timeunit, w[i+1], &vNamedConsts); 
                pActionWhenGroupChanged->setTimeUnit(timeunit);
                explicitly_set_timeunit=true;
                i++; 
            }
            else if (w[i] == "group" && w.size()>i+1) {
                group1=FindGroupWithId(vpGroups, w[i+1]);
                i++;
            }
            else {
                throw string("Error in config file for command " + origw[0]);
            }
        }
        if (group1 == nullptr) throw string ("ERROR! You must specify Group! ");
        if (!explicitly_set_groupproperty) throw string ("ERROR! You must specify pActionWhenGroupChanged property");
        if (!explicitly_set_timeunit) {
            pActionWhenGroupChanged->setTimeUnit(o.basictimeunit, o.superevery, every);
        }
        pActionWhenGroupChanged->setBeforeTime(beforetime, realtime);
        pActionWhenGroupChanged->setAfterTime(aftertime, realtime);
        //pActionWhenGroupChanged->setEvery(every);
        //pActionWhenGroupChanged->setPrintEvery(printevery, realtime);
        pActionWhenGroupChanged->setPrintEvery(printevery);
        pActionWhenGroupChanged->addGroup(group1);
        if (group2 != nullptr) pActionWhenGroupChanged->addGroup(group2); //"productmustbe"
        if (group3 != nullptr) pActionWhenGroupChanged->setReactantMustBe(group3);
        if (group2 == nullptr && minproducttime > 0) throw string("ERROR! MinProductTime can only be specified together with ProductMustBe!");
        pActionWhenGroupChanged->setMinProductTime(minproducttime);
        pActionWhenGroupChanged->setProperty(prop);
        pActionWhenGroupChanged->setId(id);
        //pActionWhenGroupChanged->setResolution(resolution);
        totalfilename=o.outputfileprefix+msdfilename+o.outputfilesuffix;
        if (!permission_to_write(totalfilename, o.overwritefiles)) throw string("no permission to write to " + totalfilename);
        pActionWhenGroupChanged->addOut(totalfilename);
        if (successfulout != "") {
            totalfilename=o.outputfileprefix+successfulout+o.outputfilesuffix;
            if (!permission_to_write(totalfilename, o.overwritefiles)) throw string("no permission to write to " + totalfilename);
            pActionWhenGroupChanged->addOut(totalfilename);
            pActionWhenGroupChanged->setWriteSuccessfulOut(true);
        }
        pActionWhenGroupChanged->setDescription(origs);
        addAction(vpActions, pActionWhenGroupChanged);
    }
    else if (w[0] == "changegrouptime") {
        group1=FindGroupWithId(vpGroups, "all");
        group2=nullptr;
        group3=nullptr;
        group4=FindGroupWithId(vpGroups, "all"); //shellgroup
        group5=nullptr; //cannotbe
        bool checkcoordinationtable=false;
        string msdfilename("");
        int minreactanttime=0, minproducttime=0, maxreactiontime=10000000;
        ActionWhenGroupChangedParent *mypAWGC=nullptr;
        pActionChangeGroupTime = new ActionChangeGroupTime;
        for (int i=1; i<w.size(); i++) {
            if (w[i] == "every") {
                from_string<int>(every, w[i+1], &vNamedConsts);
                i++;
            }
            else if  (w[i] == "parentgroup") {
                group1=FindGroupWithId(vpGroups,w[i+1]);
                i++;
            }
            else if  (w[i] == "reactants") {
                group2=FindGroupWithId(vpGroups,w[i+1]);
                i++;
            }
            else if (w[i] == "products") {
                group3=FindGroupWithId(vpGroups,w[i+1]);
                i++;
            }
            else if (w[i] == "cannotbe") {
                group5=FindGroupWithId(vpGroups,w[i+1]);
                i++;
            }
            else if (w[i] == "filename") {
                msdfilename=w[i+1];
                i++;
            }
            else if (w[i] == "minreactanttime") {
                from_string<int>(minreactanttime, w[i+1], &vNamedConsts);
                i++;
            }
            else if (w[i] == "maxreactiontime") {
                from_string<int>(maxreactiontime, w[i+1], &vNamedConsts);
                i++;
            }
            else if (w[i] == "checkcoordinationtable") { checkcoordinationtable=true; }
            else if (w[i] == "shellgroup") {
                group4=FindGroupWithId(vpGroups, w[i+1]);
                i++;
            }
            //else if (w[i] == "minproducttime") {
            //    from_string<int>(minproducttime, w[i+1], &vNamedConsts);
            //    i++;
            //}
            else if (w[i] == "action") {
                mypAWGC=dynamic_cast<ActionWhenGroupChangedParent*>(FindActionWithId(vpActions, w[i+1]));
                if (mypAWGC == nullptr) {
                    throw string("Need ActionWhenGroupChanged kind of action for ChangeGroupTime");
                }
                pActionChangeGroupTime->addActionWhenGroupChanged(mypAWGC);
                i++;
            }
            else throw string("SOmething is wrong with " + origw[0] + " command: " + origw[i]);
        }
        if (group2 == nullptr || group3 == nullptr) throw string("Must specify reactants and products for command " + origw[0]);
        totalfilename=o.outputfileprefix+msdfilename+o.outputfilesuffix;
        if (!permission_to_write(totalfilename, o.overwritefiles)) throw string("no permission to write to " + totalfilename);
        pActionChangeGroupTime->addOut(totalfilename);
        pActionChangeGroupTime->addGroup(group1);
        pActionChangeGroupTime->addGroup(group2);
        pActionChangeGroupTime->addGroup(group3);
        pActionChangeGroupTime->addGroup(group5); //cannotbe
        pActionChangeGroupTime->setMinReactantTime(minreactanttime);
        pActionChangeGroupTime->setMaxReactionTime(maxreactiontime);
        pActionChangeGroupTime->setShellGroup(group4);
        pActionChangeGroupTime->setCheckCoordinationTable(checkcoordinationtable);
        //pActionChangeGroupTime->setMinProductTime(minproducttime);
        //pActionChangeGroupTime->setActionWhenGroupChanged(mypAWGC);
        addAction(vpActions, pActionChangeGroupTime);
    }

    else if (w[0] == string("msd") || w[0] == "msdfollow" || w[0] == "msdinitial" ) {
        groupname1=w[1];
        group1=FindGroupWithId(vpGroups, w[1]);
        string msdfilename(""); float msdmaxtime=0;
        msdfilename=origw[2];
        from_string<float>(msdmaxtime, w[3], &vNamedConsts);
        totalfilename=o.outputfileprefix+msdfilename+o.outputfilesuffix;
        if (!permission_to_write(totalfilename, o.overwritefiles)) throw string("no permission to write to " + totalfilename);
        //vOutputfiles.push_back(new ofstream(msdfilename));
        if ( w[0] == "msd" ) {
            pActionMSD = new ActionMSD; // no object associated with ActionMSD, it creates the TCF objects and (hopefully!) handles the cleanup
        }
        else if (w[0] == "msdfollow") {
            pActionMSD = new ActionMSDFollow;
        }
        else if (w[0] == "msdinitial") {
            pActionMSD = new ActionMSDInitial;
        }
        pActionMSD->addGroup(group1);
        pActionMSD->addOut(totalfilename);
        pActionMSD->setNumItemsPerTimeStep(3);
        pActionMSD->setContinuousMembers(false);
        every=1; float timeunit=1;
        bool explicitly_set_timeunit=false;
        bool realtime=false;
        group2=nullptr;
        for (int i=4; i<w.size(); i++) {
            if (w[i] == "every" && w.size()>i+1) { 
                from_string<int>(every, w[i+1]); 
                pActionMSD->setEvery(every); 
                i++; 
            }
            else if (w[i] == "timeunit" && w.size()>i+1) { 
                from_string<float>(timeunit, w[i+1], &vNamedConsts); 
                pActionMSD->setTimeUnit(timeunit);
                explicitly_set_timeunit=true;
                i++; 
            }
            else if (w[i] == "average") { pActionMSD->setAverage(true); }
            //else if (w[i] == "mascen") { vMSD[vMSD.size()-1].setMasCen(true); }
            else if (w[i] == "continuousmembers") { pActionMSD->setContinuousMembers(true); }
            else if (w[i] == "noz") { pActionMSD->setDimZ(false); pActionMSD->decNumItemsPerTimeStep(); }
            else if (w[i] == "nox") { pActionMSD->setDimX(false); pActionMSD->decNumItemsPerTimeStep(); }
            else if (w[i] == "noy") { pActionMSD->setDimY(false); pActionMSD->decNumItemsPerTimeStep(); }
            else if (w[i] == "realtime") { realtime=true; }
            else if (w[i] == "subgroup" && w.size()>i+1) { 
                if (w[0] != "msdinitial") {
                    throw string("subgroup can only be specified for MSDInitial and not for " +  origw[0]);
                }
                group2=FindGroupWithId(vpGroups, w[i+1]); 
                i++;
            }
            else throw string("Unknown flags for MSD command: " + w[i]);
        }
        if (!explicitly_set_timeunit) {
            pActionMSD->setTimeUnit(o.basictimeunit, o.superevery, every);
        }
        if (w[0] == "msdinitial") {
            if (group2 == nullptr) {
                throw string("Must provide Subgroup for MSDInitial");
            }
            else {
                pActionMSD->addGroup(group2);
            }
        }
        pActionMSD->setMaxTime(msdmaxtime, realtime);
        pActionMSD->setDescription(origs);
        addAction(vpActions, pActionMSD);
    }
    else if (w[0] == string("vac") || w[0] == string("vacf")) {
        groupname1=w[1];
        group1=FindGroupWithId(vpGroups, w[1]);
        string msdfilename(""); int msdmaxtime=0;
        msdfilename=origw[2];
        from_string<int>(msdmaxtime, w[3], &vNamedConsts);
        totalfilename=o.outputfileprefix+msdfilename+o.outputfilesuffix;
        if (!permission_to_write(totalfilename, o.overwritefiles)) throw string("no permission to write to " + totalfilename);
        //vOutputfiles.push_back(new ofstream(msdfilename));
        pActionVACF = new ActionVACF; // no object associated with ActionVACF, it creates the TCF objects and (hopefully!) handles the cleanup
        pActionVACF->addGroup(group1);
        pActionVACF->setMaxTime(msdmaxtime);
        pActionVACF->addOut(totalfilename);
        pActionVACF->setNumItemsPerTimeStep(3); //decreased if nox/noy/noz specified
        pActionVACF->setContinuousMembers(false); //possibly overridden below
        every=1; float timeunit=1;
        bool explicitly_set_timeunit=false;
        bool realtime=false;
        for (int i=4; i<w.size(); i++) {
            if (w[i] == "every" && w.size()>i+1) { 
                from_string<int>(every, w[i+1]); 
                pActionVACF->setEvery(every); 
                i++; 
            }
            else if (w[i] == "timeunit" && w.size()>i+1) { 
                from_string<float>(timeunit, w[i+1], &vNamedConsts); 
                pActionVACF->setTimeUnit(timeunit);
                explicitly_set_timeunit=true;
                i++; 
            }
            else if (w[i] == "average") { pActionVACF->setAverage(true); }
            else if (w[i] == "continuousmembers") { pActionVACF->setContinuousMembers(true); }
            else if (w[i] == "noz") { pActionVACF->setDimZ(false); pActionVACF->decNumItemsPerTimeStep(); }
            else if (w[i] == "nox") { pActionVACF->setDimX(false); pActionVACF->decNumItemsPerTimeStep(); }
            else if (w[i] == "noy") { pActionVACF->setDimY(false); pActionVACF->decNumItemsPerTimeStep(); }
            else if (w[i] == "realtime") { realtime=true; }
            else throw string("Unknown flags for VACF command: " + w[i]);
        }
        if (!explicitly_set_timeunit) {
            pActionVACF->setTimeUnit(o.basictimeunit, o.superevery, every);
        }
        pActionVACF->setMaxTime(msdmaxtime, realtime);
        pActionVACF->setDescription(origs);
        addAction(vpActions, pActionVACF);
    }
    else if (w[0] == "residencetimessp" || w[0]=="residencetimesspcoordinationshell") {
        group1=FindGroupWithId(vpGroups, "all");
        group4=FindGroupWithId(vpGroups, "all");
        group2=nullptr; group3=nullptr; group4=nullptr; float msdmaxtime=0; string msdfilename("");
        group5=nullptr;
        bool oldmustbemember=false;
        if (w[0] == "residencetimessp") {
            pActionResidenceTimeSSP = new ActionResidenceTimeSSP;
        }
        else {
            pActionResidenceTimeSSP = new ActionResidenceTimeSSPCoordinationShell;
        }
        pActionResidenceTimeSSP->setOldMustBeMember(true); //the SSP doesn't make much sense unless this is set to true?
        float timeunit=1;
        every=1;
        bool explicitly_set_timeunit=false;
        bool realtime=false;
        for (int i=1; i<w.size(); i++) {
            if (w[i] == "reactants" && w.size()>i+1) {
                group2=FindGroupWithId(vpGroups, w[++i]);
            }
            else if (w[i] == "products" && w.size()>i+1) {
                group3=FindGroupWithId(vpGroups, w[++i]);
            }
            else if (w[i] == "parentgroup" && w.size()>i+1) {
                group1=FindGroupWithId(vpGroups, w[++i]);
            }
            else if (w[i] == "shellgroup" && w.size()>i+1 && w[0]=="residencetimesspcoordinationshell") {
                group4=FindGroupWithId(vpGroups, w[++i]);
            }
            else if (w[i] == "centermustbeotherwiseuncare" && w.size()>i+1 && w[0]=="residencetimesspcoordinationshell") {
                group5=FindGroupWithId(vpGroups, w[++i]);
                static_cast<ActionResidenceTimeSSPCoordinationShell*>(pActionResidenceTimeSSP)->setCenterMustBeOtherwiseConsequence(Consequence::Uncare);
            }
            else if (w[i] == "centermustbeotherwiseproduct" && w.size()>i+1 && w[0]=="residencetimesspcoordinationshell") {
                group5=FindGroupWithId(vpGroups, w[++i]);
                static_cast<ActionResidenceTimeSSPCoordinationShell*>(pActionResidenceTimeSSP)->setCenterMustBeOtherwiseConsequence(Consequence::Product);
            }
            else if (w[i] == "shellmustbeotherwiseuncare" && w.size()>i+1 && w[0]=="residencetimesspcoordinationshell") {
                group6=FindGroupWithId(vpGroups, w[++i]);
                static_cast<ActionResidenceTimeSSPCoordinationShell*>(pActionResidenceTimeSSP)->setShellMustBeOtherwiseConsequence(Consequence::Uncare);
            }
            else if (w[i] == "shellmustbeotherwiseproduct" && w.size()>i+1 && w[0]=="residencetimesspcoordinationshell") {
                group6=FindGroupWithId(vpGroups, w[++i]);
                static_cast<ActionResidenceTimeSSPCoordinationShell*>(pActionResidenceTimeSSP)->setShellMustBeOtherwiseConsequence(Consequence::Product);
            }
            else if (w[i] == "filename" && w.size()>i+1) {
                totalfilename=o.outputfileprefix+w[++i]+o.outputfilesuffix;
                if (!permission_to_write(totalfilename, o.overwritefiles)) throw string("no permission to write to " + totalfilename);
                pActionResidenceTimeSSP->addOut(totalfilename);
            }
            else if ((w[i] == "maxhistory" || w[i] == "maxtime") && w.size()>i+1) {
                from_string<float>(msdmaxtime, w[++i], &vNamedConsts);
            }
            else if (w[i] == "every" && w.size()>i+1) { 
                from_string<int>(every, w[++i]); 
                pActionResidenceTimeSSP->setEvery(every);
            }
            else if (w[i] == "timeunit" && w.size()>i+1) { 
                from_string<float>(timeunit, w[++i], &vNamedConsts); 
                explicitly_set_timeunit=true;
                pActionResidenceTimeSSP->setTimeUnit(timeunit);
            }
            else if (w[i] == "average" ) {
                pActionResidenceTimeSSP->setAverage(true);
            }
            else if (w[i] == "oldmustbemember") {
                pActionResidenceTimeSSP->setOldMustBeMember(true);
            }
            else if (w[i] == "realtime") {
                realtime=true;
            }
            else if (w[i] == "tidyold" && w.size()>i+1) {
                int tidyold; from_string<int>(tidyold, w[++i]);
                pActionResidenceTimeSSP->setTidyOld(tidyold);
            }
            else {
                throw string("Unknown flag for " + origw[0] + "command: " + origw[1]);
            }
        }
        if (group2==nullptr  || group3==nullptr) throw string(origw[0] + " requires that you specify Reactants and Products!");
        pActionResidenceTimeSSP->addGroup(group1); //parentgroup
        pActionResidenceTimeSSP->addGroup(group2); //reactants
        pActionResidenceTimeSSP->addGroup(group3); //products
        if (w[0] == "residencetimesspcoordinationshell") {
            pActionResidenceTimeSSP->addGroup(group4); //shellgroup
            pActionResidenceTimeSSP->addGroup(group5); //reactantsmustbe
            pActionResidenceTimeSSP->addGroup(group6); //shellmustbe
        }
        if (!explicitly_set_timeunit) {
            pActionResidenceTimeSSP->setTimeUnit(o.basictimeunit, o.superevery, every);
        }
        pActionResidenceTimeSSP->setMaxTime(msdmaxtime, realtime);
        pActionResidenceTimeSSP->setDescription(origs);
        addAction(vpActions,pActionResidenceTimeSSP);
    }
    else if (w[0] == "residencetimeswitchbond") {
        groupname1=w[1];
        group1=FindGroupWithId(vpGroups, w[1]);
        group2=FindGroupWithId(vpGroups, "all");
        group3=FindGroupWithId(vpGroups, "all");
        string msdfilename(""); int msdmaxtime=0;
        msdfilename=origw[2];
        bool oldmustbemember=false; bool continuous=false; bool doaverage=false; bool allowrecrossings=false;
        int oldmaxtotalescapetime=-1, oldmaxcontinuousescapetime=-1, oldmustbememberfor=-1; 
        int newmaxtotalescapetime=-1, newmaxcontinuousescapetime=-1;
        from_string<int>(msdmaxtime, w[3], &vNamedConsts);
        totalfilename=o.outputfileprefix+msdfilename+o.outputfilesuffix;
        if (!permission_to_write(totalfilename, o.overwritefiles)) throw string("no permission to write to " + totalfilename);
        //vOutputfiles.push_back(new ofstream(msdfilename));
        every=1; float timeunit=1; bool explicitly_set_timeunit=false;
        bool overwriteindata=false;
        pActionResidenceTimeSwitchBond = new ActionResidenceTimeSwitchBond;
        for (int i=4; i<w.size(); i++) {
            if (w[i] == "every" && w.size()>i+1) { 
                from_string<int>(every, w[i+1]); 
                pActionResidenceTimeSwitchBond->setEvery(every);
                i++; 
            }
            else if (w[i] == "timeunit" && w.size()>i+1) { 
                from_string<float>(timeunit, w[i+1], &vNamedConsts); 
                pActionResidenceTimeSwitchBond->setTimeUnit(timeunit);
                explicitly_set_timeunit=true;
                i++; 
            }
            else if (w[i] == "parentgroup") { 
                group2=FindGroupWithId(vpGroups, w[i+1]);  //added to action below
                i++; 
            }
            else if (w[i] == "shellgroup") {
                group3=FindGroupWithId(vpGroups, w[i+1]);
                i++;
            }
            else if (w[i] == "tidyold" && w.size()>i+1) {
                int tidyold; from_string<int>(tidyold, w[++i]);
                pActionResidenceTimeSwitchBond->setTidyOld(tidyold);
            }
            else if (w[i] == "overwriteindata") {
                overwriteindata=true;
            }
            else if (w[i] == "average") { pActionResidenceTimeSwitchBond->setAverage(true); }
            else throw string("Unknown flags for ResidenceTimeSwitchBond command: " + w[i]);
        }
        pActionResidenceTimeSwitchBond->setMaxTime(msdmaxtime);
        pActionResidenceTimeSwitchBond->addOut(totalfilename);
        pActionResidenceTimeSwitchBond->addOut("gnu_"+totalfilename+".gnu");
        pActionResidenceTimeSwitchBond->addGroup(group2);
        pActionResidenceTimeSwitchBond->addGroup(group1);
        pActionResidenceTimeSwitchBond->addGroup(group3);
        pActionResidenceTimeSwitchBond->setOverwriteInData(overwriteindata);
        pActionResidenceTimeSwitchBond->setDescription(origs);
        if (!explicitly_set_timeunit) {
            pActionResidenceTimeSwitchBond->setTimeUnit(o.basictimeunit, o.superevery, every);
        }
        addAction(vpActions, pActionResidenceTimeSwitchBond);
    }
    else if (w[0] == "tmsd" || w[0] == "tmsdfollow" || w[0] == "tmsdcm" || w[0] == "tvacf" || w[0] == "tresidencetime" || w[0] == "tresidencetimessp" || w[0] == "tresidencetimecoordinationshell" || w[0] == "tresidencetimesspcoordinationshell" || w[0] == "tchargeflux") {
        if (w[0] == "tresidencetime") pActionTTCFParent = new ActionTResidenceTime;
        else if (w[0] == "tresidencetimessp") pActionTTCFParent = new ActionTResidenceTimeSSP;
        else if (w[0] == "tresidencetimecoordinationshell") pActionTTCFParent = new ActionTResidenceTimeCoordinationShell;
        else if (w[0] == "tresidencetimesspcoordinationshell") pActionTTCFParent = new ActionTResidenceTimeSSPCoordinationShell;
        else if (w[0] == "tmsd") pActionTTCFParent = new ActionTMSD;
        else if (w[0] == "tmsdfollow") pActionTTCFParent = new ActionTMSDFollow;
        else if (w[0] == "tmsdcm") pActionTTCFParent = new ActionTMSDCM;
        else if (w[0] == "tvacf") pActionTTCFParent = new ActionTVACF;
        else if (w[0] == "tchargeflux") pActionTTCFParent = new ActionTChargeFlux;
        else pActionTTCFParent = nullptr;
        parseTCFOptions(w, origw, pActionTTCFParent, vpActions, vpGroups, vNamedConsts, o.basictimeunit, o.superevery, o.outputfileprefix, o.outputfilesuffix, o.overwritefiles);
        pActionTTCFParent->setDescription(origs);
        addAction(vpActions, pActionTTCFParent);
    }
    else if (w[0] == "residencetime" || w[0] == "residencetimecoordinationshell") { //ResidenceTime groupname filename maxhistory [every x] [timeunit x] [parentgroup g] [oldmustbemember] [average] [maxescapetime]
        groupname1=w[1];
        group1=FindGroupWithId(vpGroups, w[1]);
        group2=FindGroupWithId(vpGroups, "all");
        group3=FindGroupWithId(vpGroups, "all");
        string msdfilename(""); float msdmaxtime=0;
        msdfilename=origw[2];
        bool oldmustbemember=false; bool continuous=false; bool doaverage=false; bool allowrecrossings=false;
        float oldmaxtotalescapetime=-1, oldmaxcontinuousescapetime=-1, oldmustbememberfor=0;
        float newmaxtotalescapetime=-1, newmaxcontinuousescapetime=-1;
        from_string<float>(msdmaxtime, w[3], &vNamedConsts);
        totalfilename=o.outputfileprefix+msdfilename+o.outputfilesuffix;
        if (!permission_to_write(totalfilename, o.overwritefiles)) throw string("no permission to write to " + totalfilename);
        //vOutputfiles.push_back(new ofstream(msdfilename));
        every=1; float timeunit=1; bool explicitly_set_timeunit=false; bool realtime=false;
        if (w[0] == "residencetime") {
            pActionResidenceTimeContinuous = new ActionResidenceTimeContinuous;
        }
        else if (w[0] == "residencetimecoordinationshell") {
            pActionResidenceTimeContinuous = new ActionResidenceTimeCoordinationShell;
        }

        for (int i=4; i<w.size(); i++) {
            if (w[i] == "every" && w.size()>i+1) { 
                from_string<int>(every, w[i+1]); 
                pActionResidenceTimeContinuous->setEvery(every);
                i++; 
            }
            else if (w[i] == "timeunit" && w.size()>i+1) { 
                from_string<float>(timeunit, w[i+1], &vNamedConsts); 
                pActionResidenceTimeContinuous->setTimeUnit(timeunit);
                explicitly_set_timeunit=true;
                i++; 
            }
            else if (w[i] == "parentgroup") { 
                group2=FindGroupWithId(vpGroups, w[i+1]);  //added to action below
                i++; 
            }
            else if (w[i] == "shellgroup" && w[0]=="residencetimecoordinationshell") {
                group3=FindGroupWithId(vpGroups, w[i+1]);
                i++;
            }
            else if (w[i] == "average") { pActionResidenceTimeContinuous->setAverage(true); }
            else if (w[i] == "allowrecrossings") { pActionResidenceTimeContinuous->setAllowRecrossings(true); }
            else if (w[i] == "oldmustbemember") { pActionResidenceTimeContinuous->setOldMustBeMember(true);  }
            else if (w[i] == "oldmustbememberfor") { 
                from_string<float>(oldmustbememberfor, w[i+1]); 
                i++; 
            }
            else if (w[i] == "oldmaxtotalescapetime") { 
                from_string<float>(oldmaxtotalescapetime, w[i+1]); 
                i++; 
            }
            else if (w[i] == "oldmaxcontinuousescapetime") { 
                from_string<float>(oldmaxcontinuousescapetime, w[i+1]); 
                i++; 
            }
            else if (w[i] == "newmustbemember") { pActionResidenceTimeContinuous->setNewMustBeMember(true); }
            else if (w[i] == "newmaxtotalescapetime") { 
                from_string<float>(newmaxtotalescapetime, w[i+1]); 
                i++; 
            }
            else if (w[i] == "newmaxcontinuousescapetime") { 
                from_string<float>(newmaxcontinuousescapetime, w[i+1]); 
                i++; 
            }
            else if (w[i] == "tidyold" && w.size()>i+1) {
                int tidyold; from_string<int>(tidyold, w[++i]);
                pActionResidenceTimeContinuous->setTidyOld(tidyold);
            }
            else if (w[i] == "realtime") {
                realtime=true;
            }
            else throw string("Unknown flags for ResidenceTime command: " + w[i]);
        }
        pActionResidenceTimeContinuous->addOut(totalfilename);
        pActionResidenceTimeContinuous->addOut("gnu_"+totalfilename+".gnu");
        pActionResidenceTimeContinuous->addGroup(group2);
        pActionResidenceTimeContinuous->addGroup(group1);
        if (w[0] == "residencetimecoordinationshell") {
            dynamic_cast<ActionResidenceTimeCoordinationShell*>(pActionResidenceTimeContinuous)->addGroup(group3);
        }
        if (!explicitly_set_timeunit) {
            pActionResidenceTimeContinuous->setTimeUnit(o.basictimeunit, o.superevery, every);
        }
        pActionResidenceTimeContinuous->setMaxTime(msdmaxtime, realtime);
        pActionResidenceTimeContinuous->setOldMustBeMemberFor(oldmustbememberfor, realtime);
        pActionResidenceTimeContinuous->setOldMaxTotalEscapeTime(oldmaxtotalescapetime, realtime);
        pActionResidenceTimeContinuous->setOldMaxContinuousEscapeTime(oldmaxcontinuousescapetime, realtime);
        if ( newmaxtotalescapetime >=0 ) pActionResidenceTimeContinuous->setNewMaxTotalEscapeTime(newmaxtotalescapetime, realtime);
        if ( newmaxcontinuousescapetime >=0 ) pActionResidenceTimeContinuous->setNewMaxContinuousEscapeTime(newmaxcontinuousescapetime, realtime);
        pActionResidenceTimeContinuous->setDescription(origs);
        addAction(vpActions, pActionResidenceTimeContinuous);
    }
    else if (w[0] == string("findvacuum")) {
      groupname1=w[1];
      group1=FindGroupWithId(vpGroups, w[1]);
      string vacfilename(origw[2]); float vacresolution=0.5;
      every=1; 
      for (int i=3; i<w.size(); i++) {
        if (w[i] == "every" && w.size()>i+1) { from_string<int>(every, w[i+1]); i++; }
        if (w[i] == "resolution" && w.size()>i+1) { from_string<float>(vacresolution, w[i+1], &vNamedConsts); i++; }
      }
      totalfilename=o.outputfileprefix+vacfilename+o.outputfilesuffix;
      if (!permission_to_write(totalfilename, o.overwritefiles)) throw string("no permission to write to " + totalfilename);
      //ofstream *pOut = new ofstream(vacfilename.c_str());
      //vOutputfiles.push_back(pOut);
      pActionFindVacuum = new ActionFindVacuum;
      pActionFindVacuum->setEvery(every);
      pActionFindVacuum->setResolution(vacresolution);
      pActionFindVacuum->addGroup(group1);
      pActionFindVacuum->addOut(totalfilename);
      pActionFindVacuum->setDescription(origs);
      addAction(vpActions, pActionFindVacuum); 
    }
    else if (w[0] == string("rdf")) {
        groupname1=w[1];
        groupname2=w[2];
        group1=FindGroupWithId(vpGroups, w[1]);
        group2=FindGroupWithId(vpGroups, w[2]);
        string rdffilename(""); float rdfresolution;
        rdffilename=origw[3];
        mindist=0.0; maxdist=10.0; rdfresolution=0.01; every=1; printevery=0; //default values
        bool1=false; //periodicimages
        bool2=true; //printheader
        //bool rdfautoscale=true;
        for (int i=4; i<w.size(); i++) {
            if      ( (w[i] == "mindist" || w[i] == ">") && w.size()>i+1) { from_string<float>(mindist, w[i+1], &vNamedConsts); i++; }
            else if ( (w[i] == "maxdist" || w[i] == "<") && w.size()>i+1) { from_string<float>(maxdist, w[i+1], &vNamedConsts); i++; }
            else if (w[i] == "resolution" && w.size()>i+1) {  from_string<float>(rdfresolution, w[i+1], &vNamedConsts); i++; }
            else if (w[i] == "every" && w.size()>i+1) {  from_string<int>(every, w[i+1]); i++; }
            else if (w[i] == "printevery" && w.size()>i+1) {  from_string<int>(printevery, w[i+1], &vNamedConsts); i++; }
            //else if (w[i] == "autoscale") { rdfautoscale=true; }
            //else if (w[i] == "noautoscale") { rdfautoscale=false; }
            else if (w[i] == "periodicimages") { bool1=true; }
            else if (w[i] == "printheader") { bool2=true; }
            //else if (w[i] == "filename" && w.size()>i+1) { rdffilename=origw[i+1]; i++; }
            else { throw string("Something is wrong with the RDF command...")+origw[i]; }
        }
        if (rdffilename=="") throw string("Must give filename for RDF command!");
        totalfilename=o.outputfileprefix+rdffilename+o.outputfilesuffix;
        if (!permission_to_write(totalfilename, o.overwritefiles)) throw string("no permission to write to " + totalfilename);
        //vOutputfiles.push_back(new ofstream(totalfilename.c_str()));
        pRDF = new RDF;
        pRDF->setFromGroup(group1);
        pRDF->setToGroup(group2);
        pRDF->setMinDist(mindist);
        pRDF->setMaxDist(maxdist);
        pRDF->setResolution(rdfresolution);
        pRDF->setPeriodicImages(bool1);
        pRDF->setPrintHeader(bool2);
        addObject(vpObjects, pRDF);
        //pOut = new ofstream(rdffilename);
        //vOutputfiles.push_back(pOut);
        pActionRDF = new ActionRDF;
        pActionRDF->addObject(pRDF);
        pActionRDF->addOut(totalfilename);
        pActionRDF->setEvery(every);
        pActionRDF->setPrintEvery(printevery);
        pActionRDF->setDescription(origs);
        addAction(vpActions, pActionRDF);
    } //end if w[0] = rdf
    else if (w[0] == string("anglerdf")) {
        string ardffilename("/dev/null");
        float mindist12=0, maxdist12=10, mindist23=0, maxdist23=10, mindist13=0, maxdist13=10;
        int minhits12=0, maxhits12=0;
        float minangle=0, maxangle=180, angleresolution=1.0, distanceresolution=0.01;
        float mindist23frac12=-1, mindist13frac12=-1, mindist13frac23=-1;
        float maxdist23frac12=-1, maxdist13frac12=-1, maxdist13frac23=-1;
        int coordinationnewgroup1, coordinationnewgroup2, coordinationnewgroup3;
        coordinationnewgroup1=coordinationnewgroup2=coordinationnewgroup3=ANGLERDF_NEWGROUPCOORDINATION_BOTH;
        every=1; printevery=0;
        bool1=false; bool2=false; bool3=false; bool4=false;
        bool copygroup1=false, copygroup2=false, copygroup3=false;
        vtBool.clear(); vtBool.resize(10, false); 
        GroupExternal *newgroup1=nullptr, *newgroup2=nullptr, *newgroup3=nullptr;
        group1=group2=group3=group4=nullptr;
        bool dognu=true;
        AngleType angletype(AngleType::XYZ);
        float group3dummyx=0, group3dummyy=0, group3dummyz=0, group1dummyx=0, group1dummyy=0, group1dummyz=0;
        bool group3dummyvector=false, group1dummyvector=false, group1coordinationshellcenterofmass=false, group3coordinationshellcenterofmass=false;
        bool unique12=false;
        vector<std::pair<int,int>> vGroupMustBeInCoordinationShellOf;
        vGroupMustBeInCoordinationShellOf.clear();
        bool negategroupmustbeincoordinationshellof=false;
        bool signedangle=false;
        for (int i=1; i<w.size(); i++) {
            if (w[i] == "minangle" && w.size()>i+1) { from_string<float>(minangle, w[i+1], &vNamedConsts); i++; }
            else if (w[i] == "maxangle" && w.size()>i+1) { from_string<float>(maxangle, w[i+1], &vNamedConsts); i++; }
            else if (w[i] == "angleresolution" && w.size()>i+1) { from_string<float>(angleresolution, w[i+1], &vNamedConsts); i++; }
            else if (w[i] == "distanceresolution" && w.size()>i+1) { from_string<float>(distanceresolution, w[i+1], &vNamedConsts); i++; }
            else if (w[i] == "group1" && w.size()>i+1) { groupname1=w[i+1]; group1=FindGroupWithId(vpGroups, groupname1); ++i; }
            else if (w[i] == "group2" && w.size()>i+1) { groupname2=w[i+1]; group2=FindGroupWithId(vpGroups, groupname2); ++i; }
            else if (w[i] == "group3" && w.size()>i+1) { groupname3=w[i+1]; group3=FindGroupWithId(vpGroups, groupname3); ++i; }
            else if (w[i] == "every" && w.size()>i+1) { from_string<int>(every, w[i+1]); i++; }
            else if (w[i] == "printevery" && w.size()>i+1) { from_string<int>(printevery, w[i+1], &vNamedConsts); i++; }
            else if (w[i] == "filename" && w.size()>i+1) { ardffilename=origw[i+1]; i++; }
            else if (w[i] == "groupmustbeincoordinationshellof" && w.size()>i+2) {
                int groupmustbe=0; int coordinationshellof=0;
                from_string<int>(groupmustbe, w[i+1], &vNamedConsts);
                from_string<int>(coordinationshellof, w[i+2], &vNamedConsts);
                vGroupMustBeInCoordinationShellOf.push_back(std::pair<int,int>(groupmustbe,coordinationshellof));
                //cout << vGroupMustBeInCoordinationShellOf.size() << endl;
                i+=2;
            }
            else if (w[i] == "negategroupmustbeincoordinationshellof") {
                negategroupmustbeincoordinationshellof=true;
            }
            else if (w[i] == "mindist13" && w.size()>i+1) { from_string<float>(mindist13, w[i+1], &vNamedConsts); i++; }
            else if (w[i] == "maxdist13" && w.size()>i+1) { from_string<float>(maxdist13, w[i+1], &vNamedConsts); i++; }
            else if (w[i] == "mindist12" && w.size()>i+1) { from_string<float>(mindist12, w[i+1], &vNamedConsts); i++; }
            else if (w[i] == "maxdist12" && w.size()>i+1) { from_string<float>(maxdist12, w[i+1], &vNamedConsts); i++; }
            else if (w[i] == "mindist23" && w.size()>i+1) { from_string<float>(mindist23, w[i+1], &vNamedConsts); i++; }
            else if (w[i] == "maxdist23" && w.size()>i+1) { from_string<float>(maxdist23, w[i+1], &vNamedConsts); i++; }
            else if (w[i] == "newgroup1" && w.size()>i+1) { 
                newgroup1 = static_cast<GroupExternal*>(addGroup(vpGroups, new GroupExternal(w[i+1])));
                ++i;
            }
            else if (w[i] == "newgroup2" && w.size()>i+1) { 
                newgroup2 = static_cast<GroupExternal*>(addGroup(vpGroups, new GroupExternal(w[i+1])));
                ++i;
            }
            else if (w[i] == "newgroup3" && w.size()>i+1) { 
                newgroup3 = static_cast<GroupExternal*>(addGroup(vpGroups, new GroupExternal(w[i+1])));
                ++i;
            }
            else if (w[i] == "copygroup1" && w.size()>i+1) { 
                newgroup1 = static_cast<GroupExternal*>(addGroup(vpGroups, new GroupExternal(w[i+1])));
                copygroup1=true;
                ++i;
            }
            else if (w[i] == "copygroup2" && w.size()>i+1) { 
                newgroup2 = static_cast<GroupExternal*>(addGroup(vpGroups, new GroupExternal(w[i+1])));
                copygroup2=true;
                ++i;
            }
            else if (w[i] == "copygroup3" && w.size()>i+1) { 
                newgroup3 = static_cast<GroupExternal*>(addGroup(vpGroups, new GroupExternal(w[i+1])));
                copygroup3=true;
                ++i;
            }
            else if (w[i] == "mindist23frac12" && w.size()>i+1)  { from_string<float>(mindist23frac12, w[i+1], &vNamedConsts); ++i; }
            else if (w[i] == "mindist13frac12" && w.size()>i+1)  { from_string<float>(mindist13frac12, w[i+1], &vNamedConsts); ++i; }
            else if (w[i] == "mindist13frac23" && w.size()>i+1)  { from_string<float>(mindist13frac23, w[i+1], &vNamedConsts); ++i; }
            else if (w[i] == "maxdist23frac12" && w.size()>i+1)  { from_string<float>(maxdist23frac12, w[i+1], &vNamedConsts); ++i; }
            else if (w[i] == "maxdist13frac12" && w.size()>i+1)  { from_string<float>(maxdist13frac12, w[i+1], &vNamedConsts); ++i; }
            else if (w[i] == "maxdist13frac23" && w.size()>i+1)  { from_string<float>(maxdist13frac23, w[i+1], &vNamedConsts); ++i; }
            else if (w[i] == "newgroup1coordinationgroup" && w.size()>i+1) { from_string<int>(coordinationnewgroup1, w[i+1]); ++i; }
            else if (w[i] == "newgroup2coordinationgroup" && w.size()>i+1) { from_string<int>(coordinationnewgroup2, w[i+1]); ++i; }
            else if (w[i] == "newgroup3coordinationgroup" && w.size()>i+1) { from_string<int>(coordinationnewgroup3, w[i+1]); ++i; }
            else if (w[i] == "minhits12" && w.size()>i+1)  { from_string<int>(minhits12, w[i+1]); ++i; }
            else if (w[i] == "maxhits12" && w.size()>i+1)  { from_string<int>(maxhits12, w[i+1]); ++i; }
            else if (w[i] == "anglexy") { angletype=AngleType::XY; }
            else if (w[i] == "angleyz") { angletype=AngleType::YZ; }
            else if (w[i] == "anglexz") { angletype=AngleType::XZ; }
            else if (w[i] == "anglezx") { angletype=AngleType::ZX; }
            else if (w[i] == "signedangle") { signedangle=true; }
            else if (w[i] == "unique12") { unique12=true; }
            else if (w[i] == "group1coordinationshellcenterofmass") {
                group1coordinationshellcenterofmass=true;
            }
            else if (w[i] == "group3coordinationshellcenterofmass") {
                group3coordinationshellcenterofmass=true;
            }
            else if (w[i] == "group1dummyvector" && w.size()>i+3) {
                group1dummyvector=true;
                from_string<float>(group1dummyx, w[i+1], &vNamedConsts); i++;
                from_string<float>(group1dummyy, w[i+1], &vNamedConsts); i++;
                from_string<float>(group1dummyz, w[i+1], &vNamedConsts); i++;
            }
            else if (w[i] == "group3dummyvector" && w.size()>i+3) {
                group3dummyvector=true;
                from_string<float>(group3dummyx, w[i+1], &vNamedConsts); i++;
                from_string<float>(group3dummyy, w[i+1], &vNamedConsts); i++;
                from_string<float>(group3dummyz, w[i+1], &vNamedConsts); i++;
            }
            //else if (w[i] == "nognu") { dognu=false; }
            else { throw string("Something is wrong with the AngleRDF command...")+w[i]; }
        }
        if (group1 == nullptr && group1dummyvector == false && group1coordinationshellcenterofmass == false) throw string("Must specify group1 for AngleRDF command!");
        if (group2 == nullptr) throw string("Must specify group2 for AngleRDF command!");
        if (group3 == nullptr && group3dummyvector == false && group3coordinationshellcenterofmass == false) throw string("Must specify group3 for AngleRDF command!");
        if (ardffilename == "") throw string("Must give filename for AngleRDF command!");
        if (group3dummyvector) {
            
        }
        pAngleRDF = new AngleRDF;
        pAngleRDF->setGroup1(group1);
        pAngleRDF->setGroup2(group2);
        pAngleRDF->setGroup3(group3);
        pAngleRDF->setMinAngle(minangle);
        pAngleRDF->setMaxAngle(maxangle);
        pAngleRDF->setMinDist12(mindist12);
        pAngleRDF->setMinDist23(mindist23);
        pAngleRDF->setMinDist13(mindist13);
        pAngleRDF->setMaxDist12(maxdist12);
        pAngleRDF->setMaxDist23(maxdist23);
        pAngleRDF->setMaxDist13(maxdist13);
        pAngleRDF->setUnique12(unique12);
        pAngleRDF->setAngleResolution(angleresolution);
        pAngleRDF->setDistanceResolution(distanceresolution);
        pAngleRDF->setSignedAngle(signedangle);
        pAngleRDF->setNewGroup1(newgroup1);
        pAngleRDF->setNewGroup2(newgroup2);
        //cout << "main: setting newgroup3 to " << newgroup3 << " " << newgroup3->getId() << endl;
        pAngleRDF->setNewGroup3(newgroup3);
        pAngleRDF->setCopyGroup1(copygroup1);
        pAngleRDF->setCopyGroup2(copygroup2);
        pAngleRDF->setCopyGroup3(copygroup3);
        pAngleRDF->setMinDist23Frac12(mindist23frac12);
        pAngleRDF->setMinDist13Frac12(mindist13frac12);
        pAngleRDF->setMinDist13Frac23(mindist13frac23);
        pAngleRDF->setMaxDist23Frac12(maxdist23frac12);
        pAngleRDF->setMaxDist13Frac12(maxdist13frac12);
        pAngleRDF->setMaxDist13Frac23(maxdist13frac23);
        pAngleRDF->setMinHits12(minhits12);
        pAngleRDF->setMaxHits12(maxhits12);
        if (group3dummyvector) pAngleRDF->setGroupDummyVector(3, group3dummyx, group3dummyy, group3dummyz);
        if (group1dummyvector) pAngleRDF->setGroupDummyVector(1, group1dummyx, group1dummyy, group1dummyz);
        if (group1coordinationshellcenterofmass) pAngleRDF->setGroupCoordinationShellCenterOfMass(1);
        if (group3coordinationshellcenterofmass) pAngleRDF->setGroupCoordinationShellCenterOfMass(3);
        pAngleRDF->setNewGroup1CoordinationGroup(coordinationnewgroup1);
        pAngleRDF->setNewGroup2CoordinationGroup(coordinationnewgroup2);
        //cout << "main: setting newgroup3coordinationgroup to " << coordinationnewgroup3 << endl;
        pAngleRDF->setNewGroup3CoordinationGroup(coordinationnewgroup3);
        for (int i=0; i<vGroupMustBeInCoordinationShellOf.size(); i++) {
            pAngleRDF->setGroupMustBeInCoordinationShellOf(vGroupMustBeInCoordinationShellOf[i].first, vGroupMustBeInCoordinationShellOf[i].second, true);
            if ( (vGroupMustBeInCoordinationShellOf[i].second == 1 && dynamic_cast<GroupCoordination*>(group1) == nullptr)  ||
                 (vGroupMustBeInCoordinationShellOf[i].second == 2 && dynamic_cast<GroupCoordination*>(group2) == nullptr)  ||
                 (vGroupMustBeInCoordinationShellOf[i].second == 3 && dynamic_cast<GroupCoordination*>(group3) == nullptr) ) {
                throw string("GroupMustBeInCoordinationShellOf specified for gorup without coordination shell! I refuse to continue with this sick job.");
            }


        }
        pAngleRDF->setNegateGroupMustBeInCoordinationShellOf(negategroupmustbeincoordinationshellof);
        pAngleRDF->setAngleType(angletype);
        //if (group3dummyvector) {
            //pAngleRDF->setGroup3DummyVector(group3dummyx, group3dummyy, group3dummyz);
        //}
        addObject(vpObjects, pAngleRDF);
        pActionAngleRDF = new ActionAngleRDF;
        pActionAngleRDF->addObject(pAngleRDF);
        pActionAngleRDF->setEvery(every);
        pActionAngleRDF->setPrintEvery(printevery);
        for (int i=1; i<=ACTIONANGLERDF_NUMOUTPUTFILES; i++) {
            string out1=o.outputfileprefix+ardffilename+"_"+std::to_string(static_cast<long long>(i))+o.outputfilesuffix;
            if (ardffilename == "/dev/null") out1=ardffilename;
            if (!permission_to_write(out1, o.overwritefiles)) throw string("no permission to write to " + totalfilename);
            //vOutputfiles.push_back(new ofstream(out1));
            pActionAngleRDF->addOut(out1);
        }
        //pActionAngleRDF->setGnu(dognu, o.outputfilesuffix+ardffilename, o.outputfilesuffix);
        pActionAngleRDF->setDescription(origs);
        addAction(vpActions, pActionAngleRDF);
    } //end if w[0] == anglerdf                                       
    else if (w[0] == string("dihedralrdf")) {                         
        string ardffilename("");
        float mindist1, maxdist1, mindist3, maxdist3, mindist4, maxdist4;
        groupname1=w[1];
        from_string<float>(mindist1, w[2], &vNamedConsts);
        from_string<float>(maxdist1, w[3], &vNamedConsts);
        groupname2=w[4];
        groupname3=w[5];
        from_string<float>(mindist3, w[6], &vNamedConsts);
        from_string<float>(maxdist3, w[7], &vNamedConsts);
        groupname4=w[8];
        from_string<float>(mindist4, w[9], &vNamedConsts);
        from_string<float>(maxdist4, w[10], &vNamedConsts);
        group1=FindGroupWithId(vpGroups, groupname1);
        group2=FindGroupWithId(vpGroups, groupname2);
        group3=FindGroupWithId(vpGroups, groupname3);
        group4=FindGroupWithId(vpGroups, groupname4);
        ardffilename=w[11];
        float minangle=0, maxangle=180, angleresolution=1.0, distanceresolution=0.01;
        every=1;
        for (int i=12; i<w.size(); i++) {
            if (w[i] == "minangle" && w.size()>i+1) { from_string<float>(minangle, w[i+1], &vNamedConsts); i++; }
            else if (w[i] == "maxangle" && w.size()>i+1) { from_string<float>(maxangle, w[i+1], &vNamedConsts); i++; }
            else if (w[i] == "angleresolution" && w.size()>i+1) { from_string<float>(angleresolution, w[i+1], &vNamedConsts); i++; }
            else if (w[i] == "distanceresolution" && w.size()>i+1) { from_string<float>(distanceresolution, w[i+1], &vNamedConsts); i++; }
            else if (w[i] == "every" && w.size()>i+1) { from_string<int>(every, w[i+1]); i++; }
            else if (w[i] == "filename" && w.size()>i+1) { ardffilename=origw[i+1]; i++; }
            else { throw string("Something is wrong with the DihedralRDF command..."); }
        }
        if (ardffilename == "") throw string("Must give filename for DihedralRDF command!");
        string out1(o.outputfileprefix+ardffilename+o.outputfilesuffix);
        if (!permission_to_write(out1, o.overwritefiles)) throw string("unpermitted ifle action!");
        //vOutputfiles.push_back(nullptr);  
        //vOutputfiles[vOutputfiles.size()-1] = new ofstream(out1.c_str());
        pDihedralRDF = new DihedralRDF(group1, mindist1, maxdist1, group2, group3, mindist3, maxdist3, group4, mindist4, maxdist4, angleresolution, minangle, maxangle);
        addObject(vpObjects, pDihedralRDF);
        pActionDihedralRDF = new ActionDihedralRDF;
        pActionDihedralRDF->addObject(pDihedralRDF);
        pActionDihedralRDF->setEvery(every);
        pActionDihedralRDF->addOut(out1);
        addAction(vpActions, pActionDihedralRDF);
        //vDihedralRDF.push_back(DihedralRDF(group1, mindist1, maxdist1, group2, group3, mindist3, maxdist3, group4, mindist4, maxdist4, angleresolution, minangle, maxangle));
        //vActions.push_back(Action(ACTION_TYPE_DIHEDRALRDF, every, &vDihedralRDF[vDihedralRDF.size()-1], vOutputfiles[vOutputfiles.size()-1]));
    } //end if w[0] == dihedralrdf
    //////////// ACTION TYPE FINALSHELL //////////////
    else if (w[0] == string("finalshell")) {
        string shellcommand("");
        for (int i=1; i<origw.size(); i++) {
            shellcommand=shellcommand+string(" ")+origw[i];
        }
        pActionFinalShell = new ActionFinalShell;
        pActionFinalShell->setCommand(shellcommand);
        pActionFinalShell->setDescription(origs);
        addAction(vpActions, pActionFinalShell);
    }
    else if (w[0] == "maxtotalsize" && w.size() == 3) {
        group1=FindGroupWithId(vpGroups, w[1]);
        int a;
        from_string<int>(a, w[2], &vNamedConsts);
        group1->setMaxTotalSize(a);
    }
    else if (w[0] == "sharedligands") {
        group1=FindGroupWithId(vpGroups, w[1]); //O_Na
        pActionSharedLigands = new ActionSharedLigands;
        totalfilename=o.outputfileprefix+origw[2]+o.outputfilesuffix;
        if (!permission_to_write(totalfilename, o.overwritefiles)) throw string("unpermitted file action");
        pActionSharedLigands->addOut(totalfilename);
        totalfilename=o.outputfileprefix+origw[3]+o.outputfilesuffix;
        if (!permission_to_write(totalfilename, o.overwritefiles)) throw string("unpermitted file action");
        group2=FindGroupWithId(vpGroups, "all");
        group3=FindGroupWithId(vpGroups, "all");
        for (int i=4; i<w.size(); i++) {
            if (w[i] == "ligandparentgroup") { group2=FindGroupWithId(vpGroups, w[i+1]); i++; }
            else if (w[i] == "centralparentgroup") { group3=FindGroupWithId(vpGroups, w[i+1]); i++; }
            else if (w[i] == "ligandgroups") {
                int j;
                for (j=i+1; j<w.size(); j++) {
                    if (w[j] == "centralgroups") { j=i-1; break; }
                    pActionSharedLigands->addLigandGroup(FindGroupWithId(vpGroups, w[j]));
                }
                break;
            }
            else if (w[i] == "centralgroups") {
                int j;
                for (j=i+1; j<w.size(); j++) {
                    if (w[j] == "ligandgroups") { j=i-1; break; }
                    pActionSharedLigands->addCentralGroup(FindGroupWithId(vpGroups, w[j]));
                }
            }
        }
        pActionSharedLigands->addGroup(group1);
        pActionSharedLigands->setLigandParentGroup(group2);
        pActionSharedLigands->setCentralParentGroup(group3);
        pActionSharedLigands->addOut(totalfilename);
        //cout << "ADDING ACTION  " << endl;
        addAction(vpActions, pActionSharedLigands);
    }
    else if (w[0] == "sphericalharmonics") {
        group1=FindGroupWithId(vpGroups, w[1]);
        totalfilename=o.outputfileprefix+origw[2]+o.outputfilesuffix;
        if (!permission_to_write(totalfilename, o.overwritefiles)) throw string("unpermitted file action");
        GroupExternal *externalgroup=nullptr;
        vector<string> rangegroupnames;
        pActionSphericalHarmonics = new ActionSphericalHarmonics;
        double maxlibraryerror=-1;
        int librarymin=0, librarymax=100;
        string diffcommand="";
        for (int i=3; i<w.size(); i++) {
            if (w[i] == "libraryrange") {
                from_string<int>(librarymin, w[i+1], &vNamedConsts);
                from_string<int>(librarymax, w[i+2], &vNamedConsts);
                i+=2;
            }
            else if (w[i] == "group") {
                externalgroup=static_cast<GroupExternal*>(addGroup(vpGroups, new GroupExternal(w[i+1])));
                rangegroupnames.push_back(origw[i+1]);
                GroupWithVectorOfSphericalHarmonicsRange gwvoshr;
                gwvoshr.group = externalgroup;
                for (int j=i+2; j<w.size(); j+=3) {
                    if (w[j] == "group" || w[j] == "diffgroup") {
                        i=j-1;
                        break;
                    }
                    int op; double minv, maxv;
                    from_string<int>(op, w[j], &vNamedConsts);
                    from_string<double>(minv, w[j+1], &vNamedConsts);
                    from_string<double>(maxv, w[j+2], &vNamedConsts);
                    gwvoshr.range.push_back(SphericalHarmonicsRange(op, minv, maxv));
                }
                pActionSphericalHarmonics->addRange(gwvoshr);
            }
            else if (w[i] == "diffgroup") {
                diffcommand = "DefineGroup " + origw[i+1] + " DIFF " + origw[1];
                for (int j=0; j<rangegroupnames.size(); j++) {
                    diffcommand +=  " " + rangegroupnames[j];
                }
                i++;
            }
            else if (w[i] == "maxlibraryerror" && w.size()>i+1) {
                from_string<double>(maxlibraryerror, w[i+1]);
                pActionSphericalHarmonics->setMaxLibraryError(maxlibraryerror);
                i++;
            }
            else if (w[i] == "library") {
                group2 = FindGroupWithIdNoThrow(vpGroups, w[i+1]);
                if (group2 == nullptr) {
                    externalgroup=static_cast<GroupExternal*>(addGroup(vpGroups, new GroupExternal(w[i+1])));
                    rangegroupnames.push_back(origw[i+1]);
                }
                else {
                    externalgroup = dynamic_cast<GroupExternal*>(group2);
                    if (externalgroup == nullptr) throw string("Group " + origw[i+1] + " is not of type external!");
                }
                GroupWithVectorOfSphericalHarmonicsRange gwvoshr;
                gwvoshr.group = externalgroup;
                int j=0;
                for (j=i+2; j<w.size(); j+=2) {
                    //cout << i << " " << j << " " << w[i] << " " << w[j] << " " << w[i+1] << endl;
                    if (w[j] == "group" || w[j] == "library" || w[j] == "diffgroup") {
                        i=j-1;
                        //cout << "I have had enough! i = " << i << endl;
                        break;
                    }
                    int op; double minv;
                    from_string<int>(op, w[j], &vNamedConsts);
                    if (op < librarymin || op > librarymax) continue;
                    from_string<double>(minv, w[j+1], &vNamedConsts);
                    gwvoshr.range.push_back(SphericalHarmonicsRange(op, minv, 0));
                }
                pActionSphericalHarmonics->addRange(gwvoshr);
                pActionSphericalHarmonics->setCompareToLibrary(true);
                i=j-1;
            }
            else {
                throw string(Formatter() << "something is wrong with sphericalharmoncis!" << w[i] << " " << i);
            }
        }
        pActionSphericalHarmonics->addGroup(group1);
        pActionSphericalHarmonics->addOut(totalfilename);
        addAction(vpActions, pActionSphericalHarmonics);
        if (diffcommand!="") ReadConfigFile::ProcessSingleLine(diffcommand, configfilename, o, vpActions, vpGroups, vpObjects, vNamedConsts, vInputfiles, keepreading);
    }
    else if (w[0] == string("shell")) {
        string shellcommand("");
        for (int i=1; i<origw.size(); i++) {
            shellcommand=shellcommand+string(" ")+origw[i];
        }
        const int systemresult = system(shellcommand.c_str());
    }
    else if (w[0] == string("timedensity")) {
        pActionTimeDensity = new ActionTimeDensity;
        group1=FindGroupWithId(vpGroups, "all"); group2=nullptr; group3=nullptr;
        every=1; bool explicitly_set_timeunit=false;
        for (int i=1; i<w.size(); i++) {
            if (w[i] == "group" && w.size()>i+1) {
                group1=FindGroupWithId(vpGroups, w[i+1]);
                i++;
            }
            else if (w[i] == "region" && w.size()>i+1) {
                group2=FindGroupWithId(vpGroups, w[i+1]);
                group3=dynamic_cast<GroupRegion*>(group2);
                if (group3 == nullptr) {
                    throw string("the Group for command TimeDensity must be of type REGION");
                }
                i++;
            }
            else if (w[i] == "filename" && w.size()>i+1) {
                totalfilename=o.outputfileprefix+origw[i+1]+o.outputfilesuffix;
                if (!permission_to_write(totalfilename, o.overwritefiles)) throw string("unpermitted file action");
                pActionTimeDensity->addOut(totalfilename);
                i++;
            }
            else if (w[i] == "timeunit" && w.size()>i+1) {
                float x;
                from_string<float>(x, w[i+1]);
                pActionTimeDensity->setTimeUnit(x);
                explicitly_set_timeunit=true;
                i++;
            }
            else if (w[i] == "every" && w.size()>i+1) {
                from_string<int>(every, w[i+1]);
                pActionTimeDensity->setEvery(every);
                i++;
            }
            else {
                throw string("Error in TimeDensity command");
            }
        }
        if (group1==nullptr) { throw string("TimeDensity REQUIRES a Group!"); } //actually defaults to "all" so this never happens
        if (group2==nullptr) { throw string("TimeDensity REQUIRES a region!"); }
        if (!explicitly_set_timeunit) {
            pActionTimeDensity->setTimeUnit(o.basictimeunit, o.superevery, every);
        }
        pActionTimeDensity->addGroup(group1); //the group
        pActionTimeDensity->addGroup(group2); //the region group
        pActionTimeDensity->setDescription(origs);
        addAction(vpActions, pActionTimeDensity);
    }
    ////////// ACTION TYPE DENSITY ////////////////
    else if (w[0] == string("density")) {
        pActionDensity = new ActionDensity;
        for (int i=1; i<w.size(); i++) {
            if (w[i] == "axis" && w.size()>i+1 )  { 
                pActionDensity->setDirection(stringToCoordinateAxis(w[i+1])); 
                i++; 
            }
            else if (w[i] == "group" && w.size()>i+1) {
                pActionDensity->addGroup(FindGroupWithId(vpGroups, w[i+1]));
                i++;
            }
            else if (w[i] == "filename" && w.size()>i+1) {
                totalfilename=o.outputfileprefix+origw[i+1]+o.outputfilesuffix;
                if (!permission_to_write(totalfilename, o.overwritefiles)) throw string("unpermitted file action");
                //vOutputfiles.push_back(new ofstream(totalfilename));
                //pActionDensity->addOut(vOutputfiles.back());
                pActionDensity->addOut(totalfilename);
                i++;
            }
            else if (w[i] == "minvalue" && w.size()>i+1) {
                pActionDensity->setMinValue(from_string<float>(w[i+1], &vNamedConsts));
                i++;
            }
            else if (w[i] == "maxvalue" && w.size()>i+1) {
                pActionDensity->setMaxValue(from_string<float>(w[i+1], &vNamedConsts));
                i++;
            }
            else if (w[i] == "resolution" && w.size()>i+1) {
                pActionDensity->setResolution(from_string<float>(w[i+1], &vNamedConsts));
                i++;
            }
            else if (w[i] == "printevery" && w.size()>i+1) {
                pActionDensity->setPrintEvery(from_string<int>(w[i+1], &vNamedConsts));
                i++;
            }
            else if (w[i] == "noperiodic") {
                pActionDensity->setPeriodic(false);
            }
            else throw string("Something is wrong with the Density command: " + w[i]);
        }
        if (pActionDensity->getNumGroups() == 0) pActionDensity->addGroup(FindGroupWithId(vpGroups, "all"));
        pActionDensity->setDescription(origs);
        addAction(vpActions, pActionDensity);
    }
    else if (w[0] == "doubleshortdelta") {
        if (w.size() < 7) throw string ("Not enough arguments to DoubleShortDelta");
        group1=FindGroupWithId(vpGroups, w[1]);
        group2=FindGroupWithId(vpGroups, w[2]);
        group3=FindGroupWithId(vpGroups, w[3]);
        group4=FindGroupWithId(vpGroups, w[4]);
        group5=FindGroupWithId(vpGroups, w[5]);
        totalfilename=o.outputfileprefix+origw[6]+o.outputfilesuffix;
        if (!permission_to_write(totalfilename, o.overwritefiles)) throw string("unpermitted file action");
        double maxvalue=2; double resolution=0.1; double minvalue=0;
        every=1;
        for (int i=7; i<w.size(); i++) {
            if (w[i] == "resolution") {
                resolution=from_string<double>(w[i+1], &vNamedConsts);
                i++;
            }
            else if (w[i] == "every") {
                every=from_string<int>(w[i+1], &vNamedConsts);
                i++;
            }
            else if (w[i] == "printevery") {
                printevery=from_string<int>(w[i+1], &vNamedConsts);
                i++;
            }
            else if (w[i] == "maxvalue") {
                maxvalue=from_string<double>(w[i+1], &vNamedConsts);
                i++;
            }
            else if (w[i] == "minvalue") {
                minvalue=from_string<double>(w[i+1], &vNamedConsts);
                i++;
            }
            else throw string ("unknown argument to ShortDelta: " + w[i]);
        }
        pActionDoubleShortDelta = new ActionDoubleShortDelta;
        pActionDoubleShortDelta->setEvery(every);
        pActionDoubleShortDelta->setPrintEvery(printevery);
        pActionDoubleShortDelta->setResolution(resolution);
        pActionDoubleShortDelta->setMaxDelta(maxvalue);
        //pActionDoubleShortDelta->setMinDelta(minvalue);
        pActionDoubleShortDelta->addOut(totalfilename);
        pActionDoubleShortDelta->setStart(group1);
        pActionDoubleShortDelta->setMiddle1(group2);
        pActionDoubleShortDelta->setEnd1(group3);
        pActionDoubleShortDelta->setMiddle2(group4);
        pActionDoubleShortDelta->setEnd2(group5);
        addAction(vpActions, pActionDoubleShortDelta);
    }
    else if (w[0] == "doublecoordinationshortdelta") {
        double maxvalue=2, minvalue=0;
        double max13=3.5, min13=2, resolution13=0.05;
        double resolution=0.01;
        Group *lgroup2mustbe=nullptr, *lgroup3mustbe=nullptr;
        Group *rgroup2mustbe=nullptr, *rgroup3mustbe=nullptr;
        GroupExternal *newgroupl1=nullptr, *newgroupl2=nullptr, *newgroupl3=nullptr;
        GroupExternal *newgroupr1=nullptr, *newgroupr2=nullptr, *newgroupr3=nullptr;
        int coordinationnewgroup1, coordinationnewgroup2, coordinationnewgroup3;
        int rcoordinationnewgroup1, rcoordinationnewgroup2, rcoordinationnewgroup3;
        coordinationnewgroup1=coordinationnewgroup2=coordinationnewgroup3=rcoordinationnewgroup1=rcoordinationnewgroup2=rcoordinationnewgroup3=ANGLERDF_NEWGROUPCOORDINATION_BOTH;
        every=1;
        int printevery=0;
        group1=group2=group3=group4=nullptr;
        totalfilename="/dev/null";
        string printalldeltasfilename="/dev/null";
        bool printalldeltas=false;
        bool wellatzero=false;
        for (int i=1; i<w.size(); i++) {
            if (w[i] == "filename") {
                totalfilename=o.outputfileprefix+origw[i+1]+o.outputfilesuffix;
                if (!permission_to_write(totalfilename, o.overwritefiles)) throw string("unpermitted file action");
                i++;
            }
            else if (w[i] == "printalldeltas") {
                printalldeltas=true;
                printalldeltasfilename=o.outputfileprefix+origw[i+1]+o.outputfilesuffix;
                if (!permission_to_write(printalldeltasfilename, o.overwritefiles)) throw string("unpermitted file action");
                i++;
            }
            else if (w[i] == "lhs") {
                group1=FindGroupWithId(vpGroups, w[i+1]);
                group2=FindGroupWithId(vpGroups, w[i+2]);
                i+=2;
            }
            else if (w[i] == "rhs") {
                group3=FindGroupWithId(vpGroups, w[i+1]);
                group4=FindGroupWithId(vpGroups, w[i+2]);
                i+=2;
            }
            else if (w[i] == "resolution") { resolution=from_string<double>(w[i+1], &vNamedConsts); i++; }
            else if (w[i] == "resolution13") { resolution13=from_string<double>(w[i+1], &vNamedConsts); i++; }
            else if (w[i] == "every") { every=from_string<int>(w[i+1], &vNamedConsts); i++; }
            else if (w[i] == "printevery") { printevery=from_string<int>(w[i+1], &vNamedConsts); i++; }
            else if (w[i] == "maxvalue") { maxvalue=from_string<double>(w[i+1], &vNamedConsts); i++; }
            else if (w[i] == "minvalue") { minvalue=from_string<double>(w[i+1], &vNamedConsts); i++; }
            else if (w[i] == "max13") { max13=from_string<double>(w[i+1], &vNamedConsts); i++; }
            else if (w[i] == "min13") { min13=from_string<double>(w[i+1], &vNamedConsts); i++; }
            else if (w[i] == "wellatzero") {wellatzero=true;}
            else if (w[i] == "newgroupl1" && w.size()>i+1) { newgroupl1 = static_cast<GroupExternal*>(addGroup(vpGroups, new GroupExternal(w[i+1]))); ++i; }
            else if (w[i] == "newgroupl2" && w.size()>i+1) { newgroupl2 = static_cast<GroupExternal*>(addGroup(vpGroups, new GroupExternal(w[i+1]))); ++i; }
            else if (w[i] == "newgroupl3" && w.size()>i+1) { newgroupl3 = static_cast<GroupExternal*>(addGroup(vpGroups, new GroupExternal(w[i+1]))); ++i; }
            else if (w[i] == "newgroupl1coordinationgroup" && w.size()>i+1) { from_string<int>(coordinationnewgroup1, w[i+1]); ++i; }
            else if (w[i] == "newgroupl2coordinationgroup" && w.size()>i+1) { from_string<int>(coordinationnewgroup2, w[i+1]); ++i; }
            else if (w[i] == "newgroupl3coordinationgroup" && w.size()>i+1) { from_string<int>(coordinationnewgroup3, w[i+1]); ++i; }
            else if (w[i] == "lgroup3mustbe") { lgroup3mustbe=FindGroupWithId(vpGroups, w[i+1]); ++i; }
            else if (w[i] == "lgroup2mustbe") { lgroup2mustbe=FindGroupWithId(vpGroups, w[i+1]); ++i; }
            else if (w[i] == "newgroupr1" && w.size()>i+1) { newgroupr1 = static_cast<GroupExternal*>(addGroup(vpGroups, new GroupExternal(w[i+1]))); ++i; }
            else if (w[i] == "newgroupr2" && w.size()>i+1) { newgroupr2 = static_cast<GroupExternal*>(addGroup(vpGroups, new GroupExternal(w[i+1]))); ++i; }
            else if (w[i] == "newgroupr3" && w.size()>i+1) { newgroupr3 = static_cast<GroupExternal*>(addGroup(vpGroups, new GroupExternal(w[i+1]))); ++i; }
            else if (w[i] == "newgroupr1coordinationgroup" && w.size()>i+1) { from_string<int>(rcoordinationnewgroup1, w[i+1]); ++i; }
            else if (w[i] == "newgroupr2coordinationgroup" && w.size()>i+1) { from_string<int>(rcoordinationnewgroup2, w[i+1]); ++i; }
            else if (w[i] == "newgroupr3coordinationgroup" && w.size()>i+1) { from_string<int>(rcoordinationnewgroup3, w[i+1]); ++i; }
            else if (w[i] == "rgroup3mustbe") { rgroup3mustbe=FindGroupWithId(vpGroups, w[i+1]); ++i; }
            else if (w[i] == "rgroup2mustbe") { rgroup2mustbe=FindGroupWithId(vpGroups, w[i+1]); ++i; }
            else throw string ("something is wrong with ShortDelta command: " + w[i]);
        }
        pActionDoubleCoordinationShortDelta = new ActionDoubleCoordinationShortDelta;
        pActionDoubleCoordinationShortDelta->setEvery(every);
        pActionDoubleCoordinationShortDelta->setPrintEvery(printevery);
        if (group1 == nullptr || group2 == nullptr) throw string("ERROR! You should specify LHS in DoubleCoordinationShortDelta!");
        pActionDoubleCoordinationShortDelta->addGroup(group1);
        pActionDoubleCoordinationShortDelta->addGroup(group2);
        pActionDoubleCoordinationShortDelta->addGroup(group3);
        pActionDoubleCoordinationShortDelta->addGroup(group4); //parent group for 1 and 3
        pActionDoubleCoordinationShortDelta->addOut(totalfilename);
        pActionDoubleCoordinationShortDelta->addOut(totalfilename+"2d");
        if (printalldeltas) {
            pActionDoubleCoordinationShortDelta->setPrintAllDeltas(printalldeltasfilename);
        }
        pActionDoubleCoordinationShortDelta->setMaxDelta(maxvalue);
        pActionDoubleCoordinationShortDelta->setMinDelta(minvalue);
        pActionDoubleCoordinationShortDelta->setResolution(resolution);
        pActionDoubleCoordinationShortDelta->setMax13(max13);
        pActionDoubleCoordinationShortDelta->setMin13(min13);
        pActionDoubleCoordinationShortDelta->setResolution13(resolution13);
        pActionDoubleCoordinationShortDelta->setWellAtZero(wellatzero);
        pActionDoubleCoordinationShortDelta->setNewGroupL1(newgroupl1);
        pActionDoubleCoordinationShortDelta->setNewGroupL2(newgroupl2);
        pActionDoubleCoordinationShortDelta->setNewGroupL3(newgroupl3);
        pActionDoubleCoordinationShortDelta->setNewGroupL1CoordinationGroup(coordinationnewgroup1);
        pActionDoubleCoordinationShortDelta->setNewGroupL2CoordinationGroup(coordinationnewgroup2);
        pActionDoubleCoordinationShortDelta->setNewGroupL3CoordinationGroup(coordinationnewgroup3);
        pActionDoubleCoordinationShortDelta->setLGroup3MustBe(lgroup3mustbe);
        pActionDoubleCoordinationShortDelta->setLGroup2MustBe(lgroup2mustbe);
        pActionDoubleCoordinationShortDelta->setNewGroupR1(newgroupr1);
        pActionDoubleCoordinationShortDelta->setNewGroupR2(newgroupr2);
        pActionDoubleCoordinationShortDelta->setNewGroupR3(newgroupr3);
        pActionDoubleCoordinationShortDelta->setNewGroupR1CoordinationGroup(rcoordinationnewgroup1);
        pActionDoubleCoordinationShortDelta->setNewGroupR2CoordinationGroup(rcoordinationnewgroup2);
        pActionDoubleCoordinationShortDelta->setNewGroupR3CoordinationGroup(rcoordinationnewgroup3);
        pActionDoubleCoordinationShortDelta->setRGroup3MustBe(rgroup3mustbe);
        pActionDoubleCoordinationShortDelta->setRGroup2MustBe(rgroup2mustbe);
        addAction(vpActions, pActionDoubleCoordinationShortDelta);
    }
    else if (w[0] == "shortdelta") {
        group1=FindGroupWithId(vpGroups, w[1]);
        group2=FindGroupWithId(vpGroups, w[2]);
        group3=FindGroupWithId(vpGroups, w[3]);
        group4=FindGroupWithId(vpGroups, "all");
        Group *historygroup=nullptr;
        Group *group3mustbe=nullptr;
        totalfilename=o.outputfileprefix+origw[4]+o.outputfilesuffix;
        if (!permission_to_write(totalfilename, o.overwritefiles)) throw string("unpermitted file action");
        double maxvalue=2, minvalue=0;
        double resolution=0.01;
        double scaleby=1.0;
        GroupExternal *newgroup1=nullptr, *newgroup2=nullptr, *newgroup3=nullptr;
        int setdelta=0;
        int coordinationnewgroup1, coordinationnewgroup2, coordinationnewgroup3;
        coordinationnewgroup1=coordinationnewgroup2=coordinationnewgroup3=ANGLERDF_NEWGROUPCOORDINATION_BOTH;
        every=1;
        int historywinner=0, desiredwinner=0, margin=0, printevery=0;
        bool saveexactdelta=false, excludegroup1coordinationtable=false;;
        for (int i=5; i<w.size(); i++) {
            if (w[i] == "resolution") { resolution=from_string<double>(w[i+1], &vNamedConsts); i++; }
            else if (w[i] == "every") { every=from_string<int>(w[i+1], &vNamedConsts); i++; }
            else if (w[i] == "printevery") { printevery=from_string<int>(w[i+1], &vNamedConsts); i++; }
            else if (w[i] == "maxvalue") { maxvalue=from_string<double>(w[i+1], &vNamedConsts); i++; }
            else if (w[i] == "minvalue") { minvalue=from_string<double>(w[i+1], &vNamedConsts); i++; }
            else if (w[i] == "scaleby") { scaleby=from_string<double>(w[i+1], &vNamedConsts); i++; }
            else if (w[i] == "setdelta") { setdelta=from_string<int>(w[i+1], &vNamedConsts); i++; }
            else if (w[i] == "newgroup1" && w.size()>i+1) { newgroup1 = static_cast<GroupExternal*>(addGroup(vpGroups, new GroupExternal(w[i+1]))); ++i; }
            else if (w[i] == "newgroup2" && w.size()>i+1) { newgroup2 = static_cast<GroupExternal*>(addGroup(vpGroups, new GroupExternal(w[i+1]))); ++i; }
            else if (w[i] == "newgroup3" && w.size()>i+1) { newgroup3 = static_cast<GroupExternal*>(addGroup(vpGroups, new GroupExternal(w[i+1]))); ++i; }
            else if (w[i] == "newgroup1coordinationgroup" && w.size()>i+1) { from_string<int>(coordinationnewgroup1, w[i+1]); ++i; }
            else if (w[i] == "newgroup2coordinationgroup" && w.size()>i+1) { from_string<int>(coordinationnewgroup2, w[i+1]); ++i; }
            else if (w[i] == "newgroup3coordinationgroup" && w.size()>i+1) { from_string<int>(coordinationnewgroup3, w[i+1]); ++i; }
            else if (w[i] == "parentgroup" && w.size()>i+1) { group4=FindGroupWithId(vpGroups, w[i+1]); ++i; }
            else if (w[i] == "desiredwinner") { from_string<int>(desiredwinner, w[i+1]); ++i; }
            else if (w[i] == "margin") { from_string<int>(margin, w[i+1]); ++i; }
            else if (w[i] == "historywinner") { from_string<int>(historywinner, w[i+1]); ++i; }
            else if (w[i] == "historygroup") { historygroup=FindGroupWithId(vpGroups, w[i+1]); ++i; }
            else if (w[i] == "saveexactdelta") { saveexactdelta=true; }
            else if (w[i] == "group3mustbe") { group3mustbe=FindGroupWithId(vpGroups, w[i+1]); ++i; }
            else if (w[i] == "excludegroup1coordinationtable") { excludegroup1coordinationtable=true; }
            else throw string ("something is wrong with ShortDelta command: " + w[i]);
        }
        pActionShortDelta = new ActionShortDelta;
        pActionShortDelta->setEvery(every);
        pActionShortDelta->setPrintEvery(printevery);
        pActionShortDelta->addGroup(group1);
        pActionShortDelta->addGroup(group2);
        pActionShortDelta->addGroup(group3);
        pActionShortDelta->addGroup(group4); //parent group for 1 and 3
        pActionShortDelta->setHistoryGroup(historygroup);
        pActionShortDelta->addOut(totalfilename);
        pActionShortDelta->setMaxDelta(maxvalue);
        pActionShortDelta->setMinDelta(minvalue);
        pActionShortDelta->setResolution(resolution);
        pActionShortDelta->setScaleBy(scaleby);
        pActionShortDelta->setNewGroup1(newgroup1);
        pActionShortDelta->setNewGroup2(newgroup2);
        pActionShortDelta->setNewGroup3(newgroup3);
        pActionShortDelta->setNewGroup1CoordinationGroup(coordinationnewgroup1);
        pActionShortDelta->setNewGroup2CoordinationGroup(coordinationnewgroup2);
        pActionShortDelta->setNewGroup3CoordinationGroup(coordinationnewgroup3);
        pActionShortDelta->setSetDelta(setdelta);
        pActionShortDelta->setHistoryWinner(historywinner);
        pActionShortDelta->setDesiredWinner(desiredwinner);
        pActionShortDelta->setMargin(margin);
        pActionShortDelta->setSaveExactDelta(saveexactdelta);
        pActionShortDelta->setGroup3MustBe(group3mustbe);
        if (excludegroup1coordinationtable && dynamic_cast<GroupCoordination*>(group1) == nullptr) throw string("ERROR! ExcludeGroup1CoordinationTable ONLY if group1 has a coordination table, silly!");
        pActionShortDelta->setExcludeGroup1CoordinationTable(excludegroup1coordinationtable);
        addAction(vpActions, pActionShortDelta);
    }
    else if (w[0] == "sumvelocities") {
        if (w.size() == 3) {
            group1=FindGroupWithId(vpGroups, w[1]);
            string msdfilename=origw[2];
            pActionSumVelocities = new ActionSumVelocities;
            pActionSumVelocities->addGroup(group1);
            pActionSumVelocities->addOut(msdfilename);
            addAction(vpActions, pActionSumVelocities);
        }
    }
    else if (w[0] == string("mintimestep")) {
        if (w.size()==1) { throw string("Argument required to MinTimestep"); }
        from_string<int>(o.mintimestep, w[1]);
    }
    else if (w[0] == string("maxtimestep")) {
        if (w.size()==1) { throw string("Argument required to MaxTimestep"); }
        if (o.maxtimestep_specified_on_commandline) {
            cerr << "WARNING: I will use the MaxTimestep specifed on the command line by the -M flag: " << o.maxtimestep << endl;
        }
        else {
            from_string<int>(o.maxtimestep, w[1]);
        }
    }
    else if (w[0] == string("superevery")) {
        if (w.size()==1) { throw string("Argument required to SuperEvery"); }
        from_string<int>(o.superevery, w[1]);
    }
    else if (w[0] == string("maxnumatoms")) {
        if (w.size()==1) { throw string("Argument required to MaxNumAtoms"); }
        from_string<int>(o.maxnumatoms, w[1]);
    }
    else if (w[0] == "timesteps") {
        int tempvalue=0;
        for (int j=1; j<w.size(); j++) {
            from_string<int>(tempvalue, w[j], &vNamedConsts);
            o.vTimesteps.push_back(tempvalue);
        }
    }
    else if (w[0] == string("dumpfile")) {
        if (w.size()==1) { throw string("Argument required to DumpFile"); }
        if (o.dumpfile_specified_on_commandline) {
            cerr << "WARNING: DumpFile " << origw[1] << " specified in config file." << endl;
            cerr << "WARNING: I will NOT use this file but instead the one specified on command line: " << o.dumpfilename << endl; //the contents of dumpfilename have been SAVED since the parsin of the command line
        }
        else {
            for (int i=1; i<origw.size(); i++) {
                o.vDumpfilenames.push_back(origw[i]);
            }
            //dumpfilename=origw[1];
        }
        //if (o.outputfileprefix=="") o.outputfileprefix=dumpfilename+string("_");
        //i never use prefixing anyway so no reason to have a crazy default value
    }
    else if (w[0] == string("dumpfileformat")) {
        if (w.size()==1) { throw string("Argument required to DumpFileFormat"); }
        if (o.dumpfileformat_specified_on_commandline && o.dumpfileformat!=w[1]) {
            cerr << "WARNING: DumpFileFormat " << origw[1] << " specified in config file." << endl;
            cerr << "WARNING: I will NOT use this format but instead the one specified on command line: " << o.dumpfileformat << endl;
        }
        else {
            o.dumpfileformat=w[1];
        }
    }
    else if (w[0] == string("const")) {
        if (w.size() != 3) { throw string("Const requires two arguments (the name and the value)"); }
        float tempvalue=0;
        from_string<float>(tempvalue, w[2], &vNamedConsts);
        for (int j=0; j<vNamedConsts.size(); j++) {
            if (vNamedConsts[j].first == w[1]) {
                cerr << "WARNING: Already found constant with name " << w[1] << " with old value " << vNamedConsts[j].second << " ; I will NOT change the value of this constant!" <<endl;
            }
        }
        vNamedConsts.push_back(NamedConst(w[1], tempvalue));
    }
    else if (w[0] == string("prefix")) {
        if (w.size()==1) { o.outputfileprefix=""; }
        else { o.outputfileprefix=origw[1]; }
    }
    else if (w[0] == string("suffix")) {
        if (o.suffix_specified_on_commandline) {
            cerr << "WARNING: suffix command specified in config file but I will use what was specified on command line: " << o.outputfilesuffix << endl;
        }
        else {
            if (w.size()==1) { o.outputfilesuffix=""; }
            else { o.outputfilesuffix=origw[1]; }
        }
    }
    else if (w[0] == string("coutfrequency") || w[0] == "cf") {
        if (w.size()==1) { throw string("Argument required to CoutFrequency"); }
        from_string<int>(o.coutfrequency, w[1]);
    }
    else if (w[0] == string("overwrite")) {
        o.overwritefiles=Overwrite::YesAll;
        //o.overwritefiles=true;
    }
    else if (w[0] == string("devnulliffileexists")) {
        if (o.overwrite_specified_on_commandline) {
            cerr << "IGNORING DevNullIfFileExists since you specified that you want to overwrite files in the command-line arguments" << endl;
        }
        else {
            o.overwritefiles=Overwrite::DevNull;
        }
    }
    else if (w[0] == string("nodistances")) {
        o.calculatedistances=false;
    }
    else if (w[0] == string("calculatevelocities")) {
        o.doCalculateVelocities=true;
    }
    else if (w[0] == string("calculatemomentum")) {
        o.doCalculateMomentum=true;
    }
    else if (w[0] == string("intelligentunwrap")) {
        o.doIntelligentUnwrap=true;
    }
    else if (w[0] == string("startbyte")) {
        if (w.size() == 1) throw string("StartByte requires argument!");
        from_string<long long int>(o.startbyte, w[1], &vNamedConsts);
    }
    else if (w[0] == "basictimeunit") {
        if (w.size() == 1) throw string("BasicTimeUnit requires argument!");
        from_string<float>(o.basictimeunit, w[1], &vNamedConsts);
    }
    else if (w[0] == "readfilebasedir") {
        if (w.size() != 2) throw string("ReadFileBaseDir requires a single argument");
        o.readfilebasedir=origw[1];
    }
    else if (w[0] == "threads") {
        if (w.size() != 2) throw string("Threads requries a single argument");
        int x=1;
        from_string<int>(x, w[1], &vNamedConsts);
        omp_set_num_threads(x);
    }
    else if (w[0] == "averagesize") {
        stringstream myss;
        myss << "FinalPrintProperties averagesize.dat MultipleLines PrintEvery 100";
        for (int i=1; i<origw.size(); i++) {
            myss << " " << origw[i];
        }
        myss << " ALLGROUPS PROPERTIES groupname averagesize"; 
        ReadConfigFile::ProcessSingleLine(myss.str(), configfilename, o, vpActions, vpGroups, vpObjects, vNamedConsts, vInputfiles, keepreading);
    }
    else if (w[0] == "hydrogenbond") {
        if (w.size() < 4) throw string("ERROR in HydrogenBond alias");
        stringstream myss;
        myss << "AngleRDF Group1 " << origw[1] << " Group2 " << origw[2] << " Group3 " << origw[3] << " GroupMustBeInCoordinationShellOf 3 2 MaxDist12 3.5 MaxDist23 1.8 MaxDist13Frac12 1 MaxAngle 30";
        for (int i=4; i<origw.size(); i++) {
            myss << " " << origw[i];
        }
        ReadConfigFile::ProcessSingleLine(myss.str(), configfilename, o, vpActions, vpGroups, vpObjects, vNamedConsts, vInputfiles, keepreading);
    }
    else if (w[0] == "singleenvironment") {
        if (w.size() < 4) throw string("ERROR!");
        group1=FindGroupWithId(vpGroups, w[1]);
        float maxdist=5;
        from_string<float>(maxdist, w[2], &vNamedConsts);
        string filename=w[3];
        string options("");
        for (int i=4; i<origw.size(); i++) {
            options+= " " + origw[i];
        }
        // nowadays I think one would only need one extra group that is defined with DefineGroup BOND fromgroup All IncludeToGroup
        string singlegroupname = w[1] + "XXXXX" + to_string(static_cast<long long int>(vpGroups.size()));
        string envgroupname = w[1] + "XXXXY" + to_string(static_cast<long long int>(vpGroups.size()));
        string sumgroupname = w[1] + "XXXXZ" + to_string(static_cast<long long int>(vpGroups.size()));
        ReadConfigFile::ProcessSingleLine("DefineGroup " + singlegroupname + " SUBGROUPRANDOM " + origw[1], configfilename, o, vpActions, vpGroups, vpObjects, vNamedConsts, vInputfiles, keepreading);
        ReadConfigFile::ProcessSingleLine(string(Formatter() << "DefineGroup " << envgroupname << " BOND All " << singlegroupname << " MaxDist " << maxdist), configfilename, o, vpActions, vpGroups, vpObjects, vNamedConsts, vInputfiles, keepreading);
        ReadConfigFile::ProcessSingleLine(string(Formatter() << "DefineGroup " << sumgroupname << " SUM " << singlegroupname << " " << envgroupname), configfilename, o, vpActions, vpGroups, vpObjects, vNamedConsts, vInputfiles, keepreading);
        ReadConfigFile::ProcessSingleLine(string(Formatter() << "PrintGroup " << sumgroupname << " XYZ " << filename << " NoEmptyFrames CenterOn 0 " << options), configfilename, o, vpActions, vpGroups, vpObjects, vNamedConsts, vInputfiles, keepreading);
        
        
    }
    else if (w[0] == "readfile" || w[0] == "rf") {
        if (w.size() != 2) throw string("ReadFile requires a single argument");
        string fullpath;
        if (origw[1][0] == '/') fullpath=origw[1];
        else fullpath=o.readfilebasedir + "/" + origw[1];
        ReadConfigFile::ReadConfigFile(fullpath, o, vpActions, vpGroups, vpObjects, vNamedConsts, vInputfiles);
        cout << "    ### End reading " << fullpath << endl;
        //ifstream rfin(o.readfilebasedir + "/" + origw[1]);
        //if (rfin.fail()) {
        //    throw string("ERROR! Couldn't read file " + o.readfilebasedir + "/" + origw[1]);
        //}
        ////cout << "#### begin file " << origw[1] << " #### "
        //string xxstring;
        //while (getline(rfin, xxstring)) {
        //    ss << xxstring << endl;
        //}
        //rfin.close();
    }
    else if (w[0] == "loop") { //Loop [VarIndex X] VARIABLE a1 a2 a3 VARIABLE b1 b2 b3 COMMAND PrintGroup @1@2 XYZ @1@2.xyz
        //cout << "loopiloopi" << endl;
        vector<vector<string>> vVar;
        int varindex=1;
        //const string& ssstr=ss.str();
        //ss.seekp(0);
        for (int i=1; i<w.size(); i++) {
            if (w[i] == "varindex") {
                from_string<int>(varindex, w[i+1]);
                i++;
            }
            if (w[i] == "variable") {
                vVar.push_back(vector<string>());
                for (int j=i+1; j<w.size(); j++) {
                    if (w[j] == "variable" || w[j] == "command") {
                        if (j == i+1) throw string("illegal loop command");
                        i=j-1;
                        break;
                    }
                    vVar.back().push_back(origw[j]);
                }
            }
            else if (w[i] == "command") {
                string command("");
                for (int j=i+1; j<w.size(); j++) {
                    if (w[j] == "command") {
                        if (j==i+1) throw string("illegal loop command");
                        i=j-1;
                        break;
                    }
                    command=command + " " + origw[j];
                    //cout << "origw[j] = " << origw[j] << "  !!! command is now: " << command << endl;
                }
                //cout << " var index = " << varindex << " vVar.size() = " << vVar.size() << endl;
                stringstream varidss;
                if (vVar.size() > 0) {
                    varidss.str("");
                    varidss.clear();
                    varidss << "@" << varindex;
                    for (int k=0; k<vVar[0].size(); k++) { //loops through the vlaues of e.g. variable 1
                        stringstream ss;
                        ss.str(""); ss.clear();
                        //cout << "REPLACING the string '" << command << "' from '" << varidss.str() << "' to '" << vVar[0][k] << "'" << endl;
                        string newcommand = ReplaceString(command, varidss.str(), vVar[0][k]);
                        if (vVar.size() == 1) {
                            ss << newcommand; //DO NOT APPEND NEWLINE, ProcessSingleLine() is picky
                        }
                        else {
                            ss << "Loop VarIndex " << varindex+1;
                            for (int j=1; j<vVar.size(); j++) {
                                //ss << "VarIndex " << varindex+1 << " VARIABLE ";
                                ss << " VARIABLE ";
                                for (int l=0; l<vVar[j].size(); l++) {
                                    ss << vVar[j][l] << " ";
                                }
                            }
                            ss << "COMMAND " << newcommand; //DO NOT APPEND NEWLINE
                        }
                        //cout << " Ia m about to process " << ss.str() << endl;
                        ReadConfigFile::ProcessSingleLine(ss.str(), configfilename, o, vpActions, vpGroups, vpObjects, vNamedConsts, vInputfiles, keepreading);
                    }
                }
            }
        }
        //ss << ssstr;
        //cout << "ss is now " << ss.str() << endl;
        }
        else if (w[0] == string("globaldistancegroup")) {
          o.globaldistancegroup=FindGroupWithId(vpGroups, w[1]);
          o.vGlobalDistanceGroups.push_back(std::make_pair(o.globaldistancegroup, o.globaldistancegroup));
        }
        else if (w[0] == string("globaldistancegroups")) {
            for (int i=1; i<w.size(); i+=2) {
                if ( ! (i+1 < w.size() )) throw string("Error! I tried to read after the end of line, probably expected another group to be specified for GlobalDistanceGroups");
                group1=FindGroupWithId(vpGroups, w[i]);
                group2=FindGroupWithId(vpGroups, w[i+1]);
                o.vGlobalDistanceGroups.push_back(std::make_pair(group1, group2));
            }
        }
        else if (w[0] == string("framenumbersmustincrease")) {
            o.framenumbersmustincrease=true;
        }
        else if (w[0] == "break") {
            keepreading=false;
            cout << "I will stop reading the configuration file here" << endl;
        }
        else {
          throw string("Unknown command: ")+origw[0];
        }
}
