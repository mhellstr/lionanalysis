#ifndef READCONFIGFILE_H
#define READCONFIGFILE_H
#include <string>
#include <vector>
#include "actionincludes.h"
#include "groupincludes.h"
#include "options.h"
using namespace std;

namespace ReadConfigFile {
void ReadConfigFile(string, CMainOptions&, vector<Action*>&, vector<Group*>&, vector<CObject*>&, vector<NamedConst>&, vector<ifstream*>&);
void ProcessSingleLine(const string&, string, CMainOptions&, vector<Action*>&, vector<Group*>&, vector<CObject*>&, vector<NamedConst>&, vector<ifstream*>&, bool&);
}

#endif
