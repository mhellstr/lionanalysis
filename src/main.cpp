/**********************************
Copyright Matti Hellstrom
***********************************/
#define VERBOSITY 0
#ifndef SVN_REV
#define SVN_REV "unknown version"
#endif
//#define DEBUG
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
#include "atom.h"
#include "timestep.h"
#include "utility.h"
#include <tclap/CmdLine.h>
#include "formatter.h"
#include "actionincludes.h"
#include "groupincludes.h"
#include <signal.h>
#include <unistd.h>
#include "options.h"
#include "readconfigfile.h"


#define MAXGROUPS 10000
#define MAXACTIONS 10000
static volatile bool GLOBAL_keepRunning = true;
void intHandler(int) {
    if (GLOBAL_keepRunning == true) {
        cout << "Ctrl-C caught... exiting at next timestep" << endl;
        GLOBAL_keepRunning = false; //this will end the main loop prematurely, and move on to freeing up memory and writing output
    }
}

using namespace std;

void show_help();

int main(int argc, char **argv) {
    //main() does:
    //1. process command line
    //2. read configuration file
    //3. loop trajectory file
    //4. close files, free memory, exit
    struct sigaction act;
    act.sa_handler = intHandler;
    sigaction(SIGINT, &act, nullptr);

    srand(time(NULL));

    CMainOptions o;

    vector<string> vCommandsFromCommandLine;


    //bool o.overwritefiles=false;
    try {
        TCLAP::CmdLine cmd("lionanalysis version " +string(SVN_REV), ' ', SVN_REV);

        //dump-file
        TCLAP::ValueArg<std::string> dumpFileArg("d","dumpfile","Name of dumpfile",false,"dump.cfg","string");
        cmd.add( dumpFileArg );

        //format
        vector<string> vAllowedFormats;
        vAllowedFormats.push_back("lammpsrdx");
        vAllowedFormats.push_back("lammpsrdxnoq");
        vAllowedFormats.push_back("lammpsrdxpeter");
        vAllowedFormats.push_back("lammpsrdxunwrapped");
        vAllowedFormats.push_back("lammpsrdxvelnoq");
        vAllowedFormats.push_back("mattixyz");
        vAllowedFormats.push_back("xmolout");
        vAllowedFormats.push_back("ceriottixyz");
        vAllowedFormats.push_back("asexyz");
        TCLAP::ValuesConstraint<std::string> allowedFormatsConstraint(vAllowedFormats);
        TCLAP::ValueArg<std::string> dumpFormatArg("f","format","Format",false,o.dumpfileformat,&allowedFormatsConstraint);
        cmd.add( dumpFormatArg );

        TCLAP::ValueArg<std::string> suffixArg("s","suffix","Output files suffix",false,o.outputfilesuffix,"string");
        cmd.add( suffixArg );

        //config file name (last argument)
        TCLAP::UnlabeledValueArg<std::string> configFileArg("config-file", "Name of configuration file", false, "ionanalysis.config", "string");
        cmd.add( configFileArg );

        //detailed usage (format of config file)
        //TCLAP::SwitchArg usageSwitch("u","usage","Show detailed usage (format of config file)", false);  cmd.add( usageSwitch );
        TCLAP::SwitchArg overwriteSwitch("o","overwrite","Overwrite output files without asking", false); cmd.add(overwriteSwitch);

        TCLAP::ValueArg<int> maxTimestepArg("M","maxtimestep","Maximum timestep iteration",0,o.maxtimestep,"int"); cmd.add(maxTimestepArg);

        TCLAP::MultiArg<string> prependCommandsArg("c","command","Commands to prepend to input file (use multiple calls to add multiple commands). NOTE: use '@' instead of whitespace for strings containing spaces!",false,"string"); cmd.add(prependCommandsArg);
        cmd.parse( argc, argv );

        o.dumpfilename = dumpFileArg.getValue();
        o.configfilename = configFileArg.getValue();
        o.dumpfileformat = dumpFormatArg.getValue();
        o.outputfilesuffix = suffixArg.getValue();
        o.maxtimestep = maxTimestepArg.getValue();
        vCommandsFromCommandLine = prependCommandsArg.getValue();
        //if (usageSwitch.getValue()) {
        //    show_help();
        //    return 0;
        //}
        if (dumpFileArg.isSet()) {
            o.dumpfile_specified_on_commandline=true;
            stow(o.dumpfilename, o.vDumpfilenames);
        }
        if (dumpFormatArg.isSet()) {
            o.dumpfileformat_specified_on_commandline=true;
        }
        if (overwriteSwitch.getValue()) {
            o.overwritefiles=Overwrite::YesAll;
        }
        if (maxTimestepArg.isSet()) {
            o.maxtimestep_specified_on_commandline=true;
        }
        o.overwrite_specified_on_commandline=overwriteSwitch.isSet();
        o.suffix_specified_on_commandline=suffixArg.isSet();
    }
    catch (TCLAP::ArgException &e)  {
         std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; 
         return 1;
    }

    //INPUT FILES
    ifstream dumpfile,
             config(o.configfilename.c_str()) //contains settings, see above
             ;
    
    //READ CONFIG FILE
    //Set default values
    float minrdfdist, maxrdfdist, rdfresolution;

    vector<Group*> vpGroups;
    vector<Action*> vpActions;
    vector<ifstream*> vInputfiles; // all input files except dumpfile and config file
    vector<CObject*> vpObjects;
    vector<Action*> vpSerialActions; //these action must be performed serially, DefineGroup...
    vector<Action*> vpNoDistancesNeededActions; //actions that are performed BEFORE the calculation of distances. For example definegroup atomicnumber - because you have to define the group that is the globaldistancegroup before you can calculate distances within that group... 
    vector<Action*> vpParallelActions; //can be performed in parallel, RDF and PrintGroup...
    vector<NamedConst> vNamedConsts;

    vpGroups.reserve(MAXGROUPS);
    vpActions.reserve(MAXACTIONS); //this is necessary since there will be a lot of pointers, e.g. to Groups
    vInputfiles.reserve(MAXACTIONS);
    vpObjects.reserve(MAXACTIONS);
    vNamedConsts.reserve(MAXACTIONS);

    char hostname[128];
    gethostname(hostname, 128);

    //DEFAULT GROUP All
    GroupAtomicNumber *pGroupAtomicNumber = new GroupAtomicNumber("all");
    pGroupAtomicNumber->setTargetAtomType(GROUP_CONSTANT_ALLATOMS);
    addGroup(vpGroups, pGroupAtomicNumber);
    Action *pAction = new ActionDefineGroup;
    pAction->addObject(pGroupAtomicNumber);
    pAction->setDescription("hardcoded definition of GroupAtomicNumber all");
    addAction(vpActions, pAction);


    try {
        if (!vCommandsFromCommandLine.empty()) {
            cout << "   ### ARGUMENTS FROM COMMAND LINE" << endl;
            for (auto &s : vCommandsFromCommandLine) {
                s = ReplaceString(s, "@", " "); //TCLAP doesn't work for arguments containing strings, so use the @ sign on the command line and here replace it with whitespace before passing it to ProcessSingleLine
                bool x; //need to define this because it's passed by reference to ProcessSingleLine....  However its value doesn't matter for processing the additional commands from the command line
                ReadConfigFile::ProcessSingleLine(s, "GIVEN-ON-COMMANDLINE", o, vpActions, vpGroups, vpObjects, vNamedConsts, vInputfiles, x);
            }
            cout << "   ### END READING ARGUMENTS FROM COMMAND LINE" << endl;
        }
        ReadConfigFile::ReadConfigFile(o.configfilename, o, vpActions, vpGroups, vpObjects, vNamedConsts, vInputfiles);
    }
    catch (string errorstring) {
        //cout << "EXCEPTION thrown: " << errorstring << " (at line " << numconfiglines << " in config file " << o.configfilename << ")" << endl;
        cout << errorstring << endl;
        cout << "Exiting." << endl;
        //CloseAllFiles<ofstream*>(vOutputfiles);
        CloseAllFiles(vInputfiles);
        for (auto & x : vpActions) delete x;
        for (auto & x : vpGroups) delete x;
        for (auto & x : vpObjects) delete x;
        return 1;
    }

    config.close();

    for (auto a : vpActions) {
        if (a->requiresDistances() == false) {
            vpNoDistancesNeededActions.push_back(a); //serial, performed BEFORE distances are calculated (but after the atomic coordinates have been read in)
        }
        else {
            const ActionType actiontype=a->getType();
            //cout << "Action " << a->getDescription() << " is of type: " << actiontype << endl;
            // I used to do:
            //     if (dynamic_cast<ActionTTCFParent*>(a) == nullptr && dynamic_cast<ActionChangeGroupTime*>(a) == nullptr) {
            //     but I dont wan't ActionChangeGroupTime to be parallel, because then it will auto-clear any groups within WHenGroupChangedDefineGroup that it calls, which is no bene
            //     and the Manipulation in ActionTTCF (ActionTVACF) doesn't work if there are several that run in parallel when the thread changes
            if (true) {
                vpSerialActions.push_back(a);
            }
            else {
                vpParallelActions.push_back(a);
            }
            //switch (actiontype) {
            //    //case ActionType::DefineGroupAtomicNumber:
            //        //vpNoDistancesNeededActions.push_back(a);
            //        //break;
            //    case ActionType::DefineGroup: 
            //    case ActionType::MSD:
            //    case ActionType::FindVacuum:
            //    //case ActionType::PrintProperties:
            //    case ActionType::AngleRDF: //includes some group definitions so must be serial
            //    case ActionType::FinalShell:
            //    case ActionType::ResidenceTimeCoordinationShell:
            //    case ActionType::ResidenceTime:
            //    case ActionType::ResidenceTimeSSP:
            //    case ActionType::ResidenceTimeSSPCoordinationShell:
            //    case ActionType::ResidenceTimeContinuous:
            //    case ActionType::ResidenceTimeSwitchBond:
            //    case ActionType::VACF:
            //    //case ActionType::RDF:
            //        //cout << "adding action " << a->getDescription() << " to serial" << endl;
            //        vpSerialActions.push_back(a);
            //        break;
            //    default:
            //        //cout << "adding action " << a->getDescription() << " to parallel" << endl;
            //        vpParallelActions.push_back(a);
            //        break;
            //}
        }
    }
        

    cout << "###########################################################" << endl;
    #pragma omp parallel sections
    {
        #pragma omp section
        cout << "Running on " << omp_get_num_threads() << " threads on host ." << hostname << endl;
    }
    cout << "There are " << vpGroups.size() << " groups and " << vpActions.size() << " actions." << endl;
    cout << "I am lionanalysis version " << SVN_REV << endl;
     
    //OPEN DUMP FILE
    //stow(dumpfilename, vDumpfilenames);
    int numdumpfiles=o.vDumpfilenames.size();
    if (numdumpfiles==0) {
      cout << "ERROR! NO DUMP FILES!!" << endl;
      return 1;
    }


    //VARIABLE DEFINITIONS
    vector<Atom> atoms; atoms.reserve(o.maxnumatoms); atoms.push_back(Atom(0,"0",0,0,0,0,0,0)); //dummy atom in place so that atom id = index in vector
    vector<Atom> oldatoms; oldatoms.reserve(o.maxnumatoms);
    Timestep curtimestep;
    string dummy;
    int na=0,
        type,
        loopcount=0
        ;
    float sumcellx=0, sumcelly=0, sumcellz=0; 

    curtimestep.number = -1000;

    double timetaken=gettime();

    int dumpfileindex=0;
    long long int firsttimestep=0;
    long long int lasttimestep=0;
    long long int previoustimestepnumber=0;
    std::streampos dumpfilesize=0;

    try {
        cout << "Opening dumpfile " << o.vDumpfilenames[dumpfileindex] << " at byte " << o.startbyte << flush;
        dumpfilesize=getFileSize(o.vDumpfilenames[dumpfileindex]);
        dumpfile.open(o.vDumpfilenames[dumpfileindex].c_str());
        dumpfile.seekg(o.startbyte, ios::beg);
        cout << " : " << dumpfile.tellg() << endl;
        if (dumpfile.fail()) {
            throw string("ERROR! Unable to open dumpfile " + o.vDumpfilenames[dumpfileindex]);
        }
        //MAIN LOOP
        while (true && GLOBAL_keepRunning==true) {
            loopcount++;
            //cout << loopcount << " " << atoms.size() << " SIZE " << oldatoms.size() << endl;
            oldatoms = atoms;
            //cout << "before clear " << endl;
            atoms.clear(); atoms.push_back(Atom(0,"0",0,0,0,0,0,0));
            //cout << "after clear " << endl;
            if (o.maxtimestep != 0 && loopcount > o.maxtimestep) break;
  
            previoustimestepnumber = curtimestep.number;
            if (!read_frame(dumpfile, atoms, curtimestep, o.dumpfileformat)) {
                if (dumpfileindex < o.vDumpfilenames.size()-1) {
                    cout << "Closing dumpfile " << o.vDumpfilenames[dumpfileindex] << " at byte " << dumpfile.tellg() << endl;
                    dumpfile.close();
                    dumpfileindex++;
                    //cout << "Opening dump file " << vDumpfilenames[dumpfileindex] << endl;
                    cout << "Opening dumpfile " << o.vDumpfilenames[dumpfileindex] << " at byte " << o.startbyte << endl;
                    dumpfilesize=getFileSize(o.vDumpfilenames[dumpfileindex]);
                    dumpfile.open(o.vDumpfilenames[dumpfileindex].c_str());
                    dumpfile.seekg(o.startbyte, ios::beg);
                    if (dumpfile.fail()) {
                        cout << "ERROR! Unable to open dumpfile " << o.vDumpfilenames[dumpfileindex] << endl;
                        //CloseAllFiles(vOutputfiles);
                        CloseAllFiles(vInputfiles);
                        return 1;
                    }
                    loopcount--;
                    atoms = oldatoms; //this is needed since atoms was cleared above! without this line e.g. CalculateVelocities will crash when there are multiple dump files, since it requires that the number of atoms remain constant.
                    continue;
                }
                else {
                    break;
                }
            }
            if (loopcount == 1) firsttimestep=curtimestep.number;
            lasttimestep=curtimestep.number;
  
            curtimestep.iteration = loopcount;
            //cout << curtimestep.cellx << " " << curtimestep.celly << " " << curtimestep.cellz << endl;

            if (o.framenumbersmustincrease == true && curtimestep.number <= previoustimestepnumber) {
                curtimestep.number = previoustimestepnumber;
                loopcount--;
                //cout << "skipping frame because FrameNumbersMustIncrease" << endl;
                continue;
            }
  
            if (o.doIntelligentUnwrap && loopcount >=2) intelligentUnwrap(oldatoms, atoms, &curtimestep); //sets atom.xu, atom.yu, atom.zu
            if (o.doCalculateVelocities && loopcount >= 2) calculateVelocities(oldatoms, atoms);
            
  
            int maxatomid=0;
            for (int i=0; i<atoms.size(); i++) {
                if (atoms[i].id > maxatomid) maxatomid = atoms[i].id;
            }
            for (int i=0; i<atoms.size(); i++) {
                //atoms[i].bonds.resize(atoms.size()+1, 0);
                atoms[i].bonds.resize(maxatomid+1, 0);
            }
  
            if (loopcount < o.mintimestep || (loopcount-1) % o.superevery != 0) continue;
            if (!o.vTimesteps.empty()) {
                bool success=false;
                for (int i=0; i<o.vTimesteps.size(); i++) {
                    if (o.vTimesteps[i] == loopcount) {
                        success=true;
                        break;
                    }
                }
                if (!success) continue;
            }
  
            //cout << "timestep= " << curtimestep.number <<  " loopcount= " << loopcount << " ; " << flush; 
            if (loopcount == 1 || loopcount % o.coutfrequency == 0) {
                cout << setw(10) << curtimestep.number << setw(8) << " (" << loopcount << ") (" << setw(8) << (loopcount-1)*o.basictimeunit << " ps) (" << setw(8) << gettime()-timetaken << " s) (" << setw(8) << setprecision(3) << (dumpfile.tellg()*100.0/dumpfilesize) << " %)" << setprecision(8) << endl;
            }
  
            for (int i=0; i<vpNoDistancesNeededActions.size(); i++) {
                //cout << "NO DISTANCE NEEDED: " << vpNoDistancesNeededActions[i]->getDescription() << endl;
                vpNoDistancesNeededActions[i]->action(&atoms, &curtimestep);
            }
  
            ///////////////////////////////////
            ////////////// COMPUTE ALL DISTANCES
            ///////////////////////////////////
  
            float xd,yd,zd;
            float dist;
            //cout << " I AM ITERATION " << curtimestep.iteration << " " << loopcount << endl;
            if (o.calculatedistances == true) {
                if (o.vGlobalDistanceGroups.empty()) {
                    #pragma omp parallel for schedule(dynamic) private(dist)
                    for (int i=1; i<atoms.size(); i++) {
                        for (int j=i+1; j<atoms.size(); j++) {
                            dist=distance(atoms[i].x,atoms[i].y,atoms[i].z,atoms[j].x,atoms[j].y,atoms[j].z, &curtimestep);
                            atoms[i].bonds[atoms[j].id] = dist;
                            atoms[j].bonds[atoms[i].id] = dist;
                        }
                    }
                }
                else {
                    for (int iii=0; iii<o.vGlobalDistanceGroups.size(); iii++) {
                        const Group *fromgroup = o.vGlobalDistanceGroups[iii].first;
                        const Group *togroup = o.vGlobalDistanceGroups[iii].second;
                        #pragma omp parallel for schedule(static) private(dist)
                        for (int ii=0; ii<fromgroup->size(); ii++) {
                            int i=fromgroup->atom(ii)->id;
                            int startjj=0;
                            if (fromgroup == togroup) {
                                startjj=ii+1;
                            }
                            for (int jj=startjj; jj<togroup->size(); jj++) {
                                int j=togroup->atom(jj)->id;
                                dist=distance(atoms[i].x,atoms[i].y,atoms[i].z,atoms[j].x,atoms[j].y,atoms[j].z, &curtimestep);
                                    //cout << "DISTLOOP GLBG" << ii << " " << i << " " << atoms[i].type << " " << jj << " " << j << " " << atoms[j].type << dist << endl;
                                    
                                atoms[i].bonds[j] = dist;
                                atoms[j].bonds[i] = dist;
                            }
                        }
                    }
                } //globaldistancegroups.empty()
            } //if calculatedistances
  
  
            /////////////////////
            //PERFORM ACTIONS
            ////////////////////
            //the groups must be defined first, cannot do that in parallel without some heavy trickery
            for (int i=0; i<vpSerialActions.size(); i++) {
                vpSerialActions[i]->action(&atoms, &curtimestep);
            }
            //the rest of the actions can be done independently (printgroup and rdf and anglerdf)
            #pragma omp parallel for schedule(dynamic,1) 
            for (int i=0; i<vpParallelActions.size(); i++) {
                  if (VERBOSITY >=2) cout << "I am thread " << omp_get_thread_num() << " and will perform action: " << vpParallelActions[i]->getDescription() << endl;
                  //cout << "DISTANCES NEEDED PARALLEL : " << vpParallelActions[i]->getDescription() << endl;
                  vpParallelActions[i]->action(&atoms, &curtimestep);
            }
  
            if (o.doCalculateMomentum) { //this needs to come after group definitions since the masses are set in DefineGroup
                calculateMomentum(atoms, &curtimestep); //calculates overall momentum, modifies curtimestep
                cout << " Momentum: " << curtimestep.momx << " " << curtimestep.momy << " " << curtimestep.momz << endl;
            }
  
            curtimestep.totaltimesteps++;
  
            curtimestep.sumcellx+=curtimestep.cellx;
            curtimestep.sumcelly+=curtimestep.celly;
            curtimestep.sumcellz+=curtimestep.cellz;

            //check the total sizes of groups
            for (int i=0; i<vpGroups.size(); i++) {
                if (vpGroups[i]->getMaxTotalSize() >= 0 && vpGroups[i]->getTotalSize() > vpGroups[i]->getMaxTotalSize()) {
                    cout << "At timestepnumber " << curtimestep.number << " timestepiteration " << curtimestep.iteration << " : group " << vpGroups[i]->getId() << " total size " << vpGroups[i]->getTotalSize() << " bigger than what you allowed: " << vpGroups[i]->getMaxTotalSize() << endl;
                    GLOBAL_keepRunning=false;
                }
            }
  
        } //FINISHED READING DUMP FILE
  
        cout << "Closing dumpfile " << o.vDumpfilenames[dumpfileindex] << " at byte " << dumpfile.tellg() << endl;
        cout << "Finished reading dump file... " << curtimestep.totaltimesteps << " frames... Writing output...." << endl;
        //Perform final action after reading entire dump file
        //#pragma omp parallel for schedule(static,1)
        for (int i=0; i<vpActions.size(); i++) {
            //cout << "final action for action " << vpActions[i]->getDescription() << endl;
            if (VERBOSITY >=2) cout << "I am thread " << omp_get_thread_num() << " and will perform action: " << vpActions[i]->getDescription() << endl;
            vpActions[i]->finalAction(&atoms, &curtimestep);
            //cout << " finished!" << endl;
        }
    }
    catch (string errorstring) {
        cout << "Exception during main loop! timestep = "<< curtimestep.number << " loopcount= " << loopcount << " " << errorstring << endl;
    }
    //CloseAllFiles(vOutputfiles);
    for (auto &x : vpActions) delete x;
    for (auto &x : vpGroups) delete x;
    for (auto &x : vpObjects) delete x;
    dumpfile.close();

    cout << "FIRST TIMESTEP: " << firsttimestep << endl;
    cout << "LAST TIMESTEP: " << lasttimestep << endl;
    timetaken=gettime()-timetaken;
    cout << "Total time: " << timetaken << " s; " << timetaken/(loopcount-1-o.mintimestep) << " s/timestep" << endl;
    cout << "Goodbye!" << endl;
    return 0;
}


void show_help() {
  cout << "lionanalysis version " << SVN_REV << endl;
  const int systemresult=system("cat /home/mhellstr/src/lionanalysis/lionanalysis/branches/oo-1/README");

}
