#ifndef OPTIONS_H
#define OPTIONS_H
class CMainOptions {
public: 
    CMainOptions() :mintimestep(0), maxtimestep(0), basictimeunit(1), superevery(1), maxnumatoms(8000), outputfileprefix(""), outputfilesuffix(""), readfilebasedir("."), doCalculateMomentum(false), doCalculateVelocities(false), doIntelligentUnwrap(false), calculatedistances(true), startbyte(0), coutfrequency(1), globaldistancegroup(nullptr), overwritefiles(Overwrite::Ask), dumpfileformat("mattixyz"), configfilename("ionanalysis.config"), dumpfilename("dump.cfg"),
    dumpfile_specified_on_commandline(false),
    dumpfileformat_specified_on_commandline(false),
    overwrite_specified_on_commandline(false),
    suffix_specified_on_commandline(false),
    maxtimestep_specified_on_commandline(false),
    framenumbersmustincrease(false)
    {}
    int mintimestep, maxtimestep; //a value of 0 means no limit
    float basictimeunit;
    int superevery;
    int maxnumatoms;
    string outputfileprefix;
    string outputfilesuffix;
    string readfilebasedir;
    bool doCalculateVelocities;
    bool doIntelligentUnwrap;
    bool calculatedistances;
    bool doCalculateMomentum;
    long long int startbyte;
    int coutfrequency;
    Group *globaldistancegroup;
    Overwrite overwritefiles;
    string dumpfileformat;
    string configfilename;
    string dumpfilename;
    bool dumpfile_specified_on_commandline;
    bool dumpfileformat_specified_on_commandline;
    bool overwrite_specified_on_commandline;
    bool suffix_specified_on_commandline;
    bool maxtimestep_specified_on_commandline;
    vector<std::pair<Group*,Group*>> vGlobalDistanceGroups;
    vector<string> vDumpfilenames;
    vector<int> vTimesteps;
    bool framenumbersmustincrease;
};

#endif
