#include "utility.h"
#include "action.h"
#include "anglerdf.h"
#include <cmath>
#include <sstream>
#include <sys/time.h>
#include <sys/stat.h>
#include <algorithm>
#include "actionttcf.h"
#include "actionwhengroupchanged.h"
#include "actiontresidencetimesspcoordinationshell.h"
#include "actiontmsdfollow.h"
#define BOHRTOANGSTROM 0.5291772109

/**** GENERAL HELP FUNCTIONS *****/

bool permission_to_write(string &totalfilename, Overwrite &overwritefiles) {
  if (FileExists(totalfilename) && overwritefiles == Overwrite::Ask) {
    cout << "WARNING: File " << totalfilename << " (and possibly others) will be overwritten. Continue? ";
    char ch;
    cin >> ch;
    if (ch == 'y' || ch == 'Y') { overwritefiles = Overwrite::YesAll; return true; }
    else return false;
  }
  else if (FileExists(totalfilename) && overwritefiles == Overwrite::DevNull && totalfilename != "/dev/null") {
    cout << "WARNING: File " << totalfilename << " exists; I will NOT overwrite this file but write to /dev/null instead." << endl;
    totalfilename="/dev/null";
    return true;
  }
  return true;
}

//From http://www.techbytes.ca/techbyte103.html
bool FileExists(string strFilename) {
  struct stat stFileInfo;
  bool blnReturn;
  int intStat;

  // Attempt to get the file attributes
  intStat = stat(strFilename.c_str(),&stFileInfo);
  if(intStat == 0) {
    // We were able to get the file attributes
    // so the file obviously exists.
    blnReturn = true;
  } else {
    // We were not able to get the file attributes.
    // This may mean that we don't have permission to
    // access the folder which contains this file. If you
    // need to do that level of checking, lookup the
    // return values of stat which will give you
    // more details on why stat failed.
    blnReturn = false;
  }
  
  return(blnReturn);
}

double gettime(void)
{
    struct timeval tv;
    gettimeofday(&tv,nullptr);
    return tv.tv_sec + 1e-6*tv.tv_usec;
}

//Group *FindGroupWithId(vector<Group> &groups, const string &groupname) {
//  for (int i=0; i<groups.size(); i++) {
//    if (groups[i].getId() == groupname) return &groups[i];
//  }
//  stringstream ss;
//  ss << "Error! Couldn't find group with name " << groupname;
//  throw ss.str();
//}

Group *FindGroupWithIdNoThrow(vector<Group*> &groups, const string &groupname) {
  for (int i=0; i<groups.size(); i++) {
    if (groups[i]->getId() == groupname) return groups[i];
  }
  return nullptr;
}

Action *FindActionWithId(vector<Action*> &actions, const string &actionname) {
    for (int i=0; i<actions.size(); i++) {
        if (actions[i]->getId() == actionname) return actions[i];
    }
    throw string("Error! Couldn't find action with name " + actionname);
}


Group *FindGroupWithId(vector<Group*> &groups, const string &groupname) {
  for (int i=0; i<groups.size(); i++) {
    if (groups[i]->getId() == groupname) return groups[i];
  }
  stringstream ss;
  ss << "Error! Couldn't find group with name " << groupname;
  throw ss.str();
}

int FindGroupIndexWithId(vector<Group> &groups, string groupname) {
  for (int i=0; i<groups.size(); i++) {
    if (groups[i].getId() == groupname) return i;
  }
  stringstream ss;
  ss << "Error! Couldn't find group with name " << groupname;
  throw ss.str();
}

void stow(const string &s, vector<string> &v)
{ //string to words. word vector v should be empty when passed to this function
  stringstream ss(s);
  string w;
  while (ss.good()) {
    ss >> w;
    v.push_back(w);
  }
}

void trim_trailing(string& str)
{
  string::size_type pos = str.find_last_not_of(' ');
  if(pos != string::npos) {
    str.erase(pos + 1);
  }
}

//From http://www.codeproject.com/KB/stl/stdstringtrim.aspx
void trim(string& str)
{
  string::size_type pos = str.find_last_not_of(' ');
  if(pos != string::npos) {
    str.erase(pos + 1);
    pos = str.find_first_not_of(' ');
    if(pos != string::npos) str.erase(0, pos);
  }
  else str.erase(str.begin(), str.end());
}

/**** ROUTINES FOR READING INPUT FILES ******/

bool read_frame(istream &xmolout, vector<Atom> &atoms, Timestep &curtimestep, string format) {
  string dummy,s;
  int na=0,tsnumber;
  string type;
  float cellx,celly,cellz,ax,ay,az,x1,x2,y1,y2,z1,z2,dummyfloat;
  bool periodic_calculation=true;
  vector<string> w;

  if (format=="lammpsrdx" || format=="lammpsrdxnoq" || format=="lammpsrdxunwrapped" || format=="lammpsrdxpeter" || format=="lammpsrdxvelnoq") {
    xmolout >> dummy >> dummy; //ITEM: TIMESTEP
    if (xmolout.eof()) return false;
    if (dummy != "TIMESTEP") {
        throw string("This does not appear to be a file of format " + format + " - Exiting.");
        return false;
    }
    xmolout >> curtimestep.number;
    //cout << "tsnumber " << tsnumber << endl;
    //if (tsnumber != curtimestep.number) return false;
    xmolout >> dummy >> dummy >> dummy >> dummy; //ITEM: NUMBER OF ATOMS
    xmolout >> na;
    //cout << "na " << na << endl;
    if (na != atoms.size()-1) { atoms.resize(na+1); }
    curtimestep.numatoms=na;
    //cout << "passed the test" << endl;
    //xmolout >> dummy >> dummy >> dummy >> dummy >> dummy >>dummy; //ITEM: BOX BOUNDS pp pp pp
    getline(xmolout,dummy); //empty
    getline(xmolout,dummy); //ITEM: BOX BOUNDS pp pp pp, or ITEM: BOX BOUNDS (depends on settings in lammps input file)
    xmolout >> x1 >> x2 >> y1 >> y2 >> z1 >> z2;
    curtimestep.cellx = x2-x1; curtimestep.celly = y2-y1; curtimestep.cellz = z2-z1;
    getline(xmolout,dummy); //empty (just reads the end of line)
    getline(xmolout,dummy); //ITEM: ATOMS id type x y z q
    trim_trailing(dummy); //remove trailing space, since it messes with the stow() function
    w.clear();
    stow(dummy, w);
    //cout << dummy << endl;
    //cout << endl;
    int numproperties=w.size()-2;
    if (format=="lammpsrdx") {
      for (int i=1; i<=na; i++) {
        xmolout >> atoms[i].id >> atoms[i].type >> atoms[i].x >> atoms[i].y >> atoms[i].z >> atoms[i].q;
        atoms[i].xu=atoms[i].x; atoms[i].yu=atoms[i].y; atoms[i].zu=atoms[i].z; //set unwrapped coordinates same as wrapped ones, since the unwrapped weren't given
        for (int j=0; j<numproperties-6; j++) xmolout >> dummy;
      }
    }
    else if (format=="lammpsrdxnoq") {
      for (int i=1; i<=na; i++) {
        xmolout >> atoms[i].id >> atoms[i].type >> atoms[i].x >> atoms[i].y >> atoms[i].z;
        atoms[i].xu=atoms[i].x; atoms[i].yu=atoms[i].y; atoms[i].zu=atoms[i].z; //set unwrapped coordinates same as wrapped ones, since the unwrapped weren't given
        for (int j=0; j<numproperties-5; j++) xmolout >> dummy;
      }
    }
    else if (format=="lammpsrdxunwrapped") {
      for (int i=1; i<=na; i++) {
        xmolout >> atoms[i].id >> atoms[i].type >> atoms[i].x >> atoms[i].y >> atoms[i].z >> atoms[i].q >> atoms[i].xu >> atoms[i].yu >> atoms[i].zu;
        for (int j=0; j<numproperties-9; j++) xmolout >> dummy;
      }
    }
    else if (format=="lammpsrdxpeter") {
      for (int i=1; i<=na; i++) {
        xmolout >> atoms[i].id >> atoms[i].x >> atoms[i].y >> atoms[i].z >> atoms[i].q; atoms[i].xu=atoms[i].x; atoms[i].yu=atoms[i].y; atoms[i].zu=atoms[i].z;
        for (int j=0; j<numproperties-5; j++) xmolout >> dummy;
      }
    }
    else if (format=="lammpsrdxvelnoq") {
        for (int i=1; i<=na; i++) {
            xmolout >> atoms[i].id >> atoms[i].type >> atoms[i].x >> atoms[i].y >> atoms[i].z >> atoms[i].vx >> atoms[i].vy >> atoms[i].vz;
            atoms[i].xu=atoms[i].x; atoms[i].yu=atoms[i].y; atoms[i].zu=atoms[i].z;
            for (int j=0; j<numproperties-8; j++) xmolout >> dummy;
        }
    }
    return true;
  }
  else if (format=="mattixyz" || format=="xmolout" || format=="xyz" || format=="asexyz" || format=="ceriottixyz" || format=="ceriotticellbohrxyz" || format=="ceriottivelocitycellbohrxyz" || format=="ceriottivelocityxyz") {
    xmolout >> na;
    if (xmolout.eof()) return false;
    curtimestep.numatoms=na;
    if (na != atoms.size()-1) { atoms.resize(na+1); }
    if (format=="mattixyz") {
      xmolout >> dummy >> curtimestep.cellx >> curtimestep.celly >> curtimestep.cellz;
      if (dummy != "XYZ") {
        throw string("Error! This does not seem to be a file of format " + format + " - Exiting");
      }
      curtimestep.number++;
    }
    else if (format=="xmolout") {
      //XYZ                                          5   -34159.07  16.67  18.49  14.45  90.00  90.00  90.00
      xmolout >> dummy >> curtimestep.number >> curtimestep.energy >> curtimestep.cellx >> curtimestep.celly >> curtimestep.cellz >> dummy >> dummy >> dummy;
    }
    else if (format=="asexyz") {
        //Lattice="5.0 0.0 0.0 0.0 5.0 0.0 0.0 0.0 5.0" Properties=species:S:1:pos:R:3:forces:R:3 nAtoms=2.0 energy_weight=1.0 forces_weights=1.0 energy=-31.21919841039272 pbc="T T T"
        xmolout >> dummy;
        if (dummy.find("Lattice=\"") != 0) {
            cout << na << "\n";
            throw string("Error! This does not seem to be a file of format " + format + " - requires comment line to start with Lattice=  - Exiting; " + dummy);
        }
        dummy = dummy.substr(9);
        curtimestep.cellx = std::stof(dummy); // after Lattice="
        xmolout >> dummyfloat; if (abs(dummyfloat) > 1e-3) throw string("Error! Non-orthorhombic lattice.");
        xmolout >> dummyfloat; if (abs(dummyfloat) > 1e-3) throw string("Error! Non-orthorhombic lattice.");
        xmolout >> dummyfloat; if (abs(dummyfloat) > 1e-3) throw string("Error! Non-orthorhombic lattice.");
        xmolout >> curtimestep.celly;
        xmolout >> dummyfloat; if (abs(dummyfloat) > 1e-3) throw string("Error! Non-orthorhombic lattice.");
        xmolout >> dummyfloat; if (abs(dummyfloat) > 1e-3) throw string("Error! Non-orthorhombic lattice.");
        xmolout >> dummyfloat; if (abs(dummyfloat) > 1e-3) throw string("Error! Non-orthorhombic lattice.");
        xmolout >> dummy;
        dummy = dummy.substr(0, dummy.find("\""));
        curtimestep.cellz = std::stof(dummy);
        curtimestep.number++;
        getline(xmolout, dummy);
    }
    else if (format=="ceriottixyz" || format=="ceriottivelocityxyz") {
    //# CELL(abcABC):   47.91212    47.91212    47.91212    90.00000    90.00000    90.00000  cell{}  Traj: x_centroid{angstrom} Step:           0  Bead:       0 
        xmolout >> dummy >> dummy >> curtimestep.cellx >> curtimestep.celly >> curtimestep.cellz >> dummy >> dummy >> dummy >>     dummy >> dummy >> dummy >> dummy     >> curtimestep.number;
        getline(xmolout, dummy);
    }
    else if (format=="ceriotticellbohrxyz" || format=="ceriottivelocitycellbohrxyz") {
        xmolout >> dummy >> dummy >> curtimestep.cellx >> curtimestep.celly >> curtimestep.cellz >> dummy >> dummy >> dummy >>     dummy >> dummy >> dummy >> dummy     >> curtimestep.number;
        curtimestep.cellx*=0.5291772109;
        curtimestep.celly*=0.5291772109;
        curtimestep.cellz*=0.5291772109;
        getline(xmolout, dummy);
    }
    else if (format=="xyz") {
      getline(xmolout, dummy); // end of line
      getline(xmolout, dummy);
    }
    for (int i=1; i<=na; i++) {
      atoms[i].id = i;
      xmolout >> type >> atoms[i].x >> atoms[i].y >> atoms[i].z;
      atoms[i].xu=atoms[i].x; atoms[i].yu=atoms[i].y; atoms[i].zu=atoms[i].z; //set unwrapped coordinates same as wrapped ones, since the unwrapped ones weren't given
      if (format=="asexyz") {
        getline(xmolout, dummy);
      }
      if (format=="ceriottivelocitycellbohrxyz" || format=="ceriottivelocityxyz") {
        atoms[i].vx = atoms[i].x; atoms[i].vy = atoms[i].y; atoms[i].vz = atoms[i].z; // in these file formats there are only velocities and no atomic positions. so the .x .y .z are meaningless
      }
      atoms[i].type = type; //for (int j=0; j<type.length(); j++) { atoms[i].type[j] = tolower(type[j]); }
      // WHY did I decide to convert atomic types to lowercase? There is probably a good reason but I do not know what. Maybe the "ATOMICNUMBER" definition? But that should work also case sensitive.
    }
    return true;
  }
  else if (format=="runnerdata") {
    int i=0;
    if (curtimestep.number==-1000) na=0;
    //cout << curtimestep.number << endl;
    while (true) {
        xmolout >> dummy;
        //cout << "dummy is " << dummy << endl;
        if (xmolout.eof()) return false;
        if (dummy.c_str()[0] == 'b') { //begin
            curtimestep.number++;
            continue; //begin
        }
        else if (dummy.c_str()[0] == 'c') { getline(xmolout,dummy); continue; } //comment or charge
        //else if (dummy.c_str()[0] == 'e' && dummy.c_str()[2]=='e' ) { getline(xmolout, dummy); continue; } //energy
        else if (dummy == "energy") {
        //else if (dummy.c_str()[0] == 'e' && dummy.c_str()[2]=='e' ) {
            xmolout >> curtimestep.energy;
        } //energy
        else if (dummy == "energy_weight") {
            xmolout >> curtimestep.energy_weight;
        }
        else if (dummy.c_str()[0] == 'a') { //atom
            na++; //if (curtimestep.number==-999) na++;
            curtimestep.numatoms=na;
            i++;
            atoms.resize(na+1); //if (curtimestep.number==-999) atoms.resize(na+1);
            atoms[i].id = i;
            //xmolout >> atoms[i].x >> atoms[i].y >> atoms[i].z >> atoms[i].type >> atoms[i].q >> dummy >> dummy >> dummy >> dummy;
            xmolout >> atoms[i].x >> atoms[i].y >> atoms[i].z >> atoms[i].type >> atoms[i].q >> dummy >> atoms[i].fx >> atoms[i].fy >> atoms[i].fz;
            atoms[i].x *= BOHRTOANGSTROM; atoms[i].y *= BOHRTOANGSTROM; atoms[i].z *= BOHRTOANGSTROM;
            atoms[i].fx *= BOHRTOANGSTROM; atoms[i].fy *= BOHRTOANGSTROM; atoms[i].fz *= BOHRTOANGSTROM;
            atoms[i].xu=atoms[i].x; atoms[i].yu=atoms[i].y; atoms[i].zu=atoms[i].z; //set unwrapped coordinates same as wrapped ones, since the unwrapped ones weren't given
            //for (int j=0; j<atoms[i].type.length(); j++) { atoms[i].type[j] = tolower(atoms[i].type[j]); }
        }
        else if (dummy.c_str()[0] == 'l') { //lattice
            xmolout >> curtimestep.cellx >> dummy >> dummy;
            xmolout >> dummy; //next "lattice"
            xmolout >> dummy >> curtimestep.celly >> dummy;
            xmolout >> dummy; //next "lattice"
            xmolout >> dummy >> dummy >> curtimestep.cellz;
            curtimestep.cellx *= BOHRTOANGSTROM; curtimestep.celly *= BOHRTOANGSTROM; curtimestep.cellz *= BOHRTOANGSTROM;
        }
        else if (dummy.c_str()[0]=='e' && dummy.c_str()[2]=='d') { //end
            break;
        }
        else {
            cout << "I dont know what Im doing: dummy = " << dummy;
            break;
        }
    }
    return true;
  }
  else if (format=="dftb" || format=="gen") {
    xmolout >> na >> dummy;
    cout << na << endl;
    if (xmolout.eof()) return false;
    curtimestep.numatoms=na;
    if (na != atoms.size()-1) { atoms.resize(na+1); }
    curtimestep.number++;
    if (dummy == "C") {
      periodic_calculation=false;
      curtimestep.cellx=10000; curtimestep.celly=curtimestep.cellx; curtimestep.cellz=curtimestep.cellx;
    }
    getline(xmolout, dummy); //end of line
    getline(xmolout, dummy); //atom type definitions, not used here, instead i just use the numbers (perhaps would be a good idea to implement a change here)
    stow(dummy, w); //w now contains the atomic kinds
    for (int i=0; i<w.size(); i++) {
      for (int j=0; j<w[i].length(); j++) {
        w[i][j] = tolower(w[i][j]);
      }
    }
    int inttype=0;
    for (int i=1; i<=na; i++) {
      xmolout >> atoms[i].id >> inttype >> atoms[i].x >> atoms[i].y >> atoms[i].z;
      atoms[i].type = w[inttype-1];
      atoms[i].xu=atoms[i].x; atoms[i].yu=atoms[i].y; atoms[i].zu=atoms[i].z; //set unwrapped coordinates same as wrapped ones, since the unwrapped ones weren't given
    }
    getline(xmolout, dummy); //end of line
    if (periodic_calculation == true) {
      getline(xmolout, dummy); //full of zeroes
      xmolout >> curtimestep.cellx >> dummy >> dummy >>    dummy >> curtimestep.celly >> dummy       >> dummy >> dummy >> curtimestep.cellz;
      getline(xmolout, dummy); //end of line
    }
    return true;
  }

  else {
    throw string("Unknown dump file format!")+format;
  }
}

float distance(float x1, float y1, float z1, float x2, float y2, float z2, const Timestep *curtimestep) {
  float xdiff=x2-x1; float ydiff=y2-y1; float zdiff=z2-z1;
  float cellx=curtimestep->cellx, celly=curtimestep->celly, cellz=curtimestep->cellz;
  while (xdiff < -cellx/2.) xdiff+=cellx;
  while (xdiff >  cellx/2.) xdiff-=cellx;
  while (ydiff < -celly/2.) ydiff+=celly;
  while (ydiff >  celly/2.) ydiff-=celly;
  while (zdiff < -cellz/2.) zdiff+=cellz;
  while (zdiff >  cellz/2.) zdiff-=cellz;
  return sqrt(xdiff*xdiff+ydiff*ydiff+zdiff*zdiff);
}

void periodicdistances(float x1, float y1, float z1, float x2, float y2, float z2, float *arr, const Timestep *curtimestep) {
    // arr should have size 27 to accommodate all distances
    float xdiff=x2-x1; float ydiff=y2-y1; float zdiff=z2-z1;
    float cellx=curtimestep->cellx, celly=curtimestep->celly, cellz=curtimestep->cellz;
    for (int i=-1; i<=1; i++) {
        for (int j=-1; j<=1; j++) {
            for (int k=-1; k<=1; k++) {
                int l=(k+1)+(j+1)*3+(i+1)*9;
                float xx=xdiff+k*cellx;
                float yy=ydiff+j*celly;
                float zz=zdiff+i*cellz;
                arr[l]=sqrt(xx*xx+yy*yy+zz*zz);
            }
        }
    }
}

void TranslateAtomCoordinatesToVicinity(float &px, float &py, float &pz, float cx, float cy, float cz, const Timestep *t)
{
  while (cx - px < -t->cellx/2.0) px-=t->cellx;
  while (cy - py < -t->celly/2.0) py-=t->celly;
  while (cz - pz < -t->cellz/2.0) pz-=t->cellz;
  while (cx - px >  t->cellx/2.0) px+=t->cellx;
  while (cy - py >  t->celly/2.0) py+=t->celly;
  while (cz - pz >  t->cellz/2.0) pz+=t->cellz;
}

float angle(const Atom* a, const Atom* b, const Atom* c, const Timestep *curtimestep) {
//angle abc, with b as the middle atom
  //cout << "in angle " << endl;
  float cx=curtimestep->cellx, cy=curtimestep->celly, cz=curtimestep->cellz;
  //cout << "celx " << curtimestep->cellx << endl;
  float nbx=b->x, nby=b->y, nbz=b->z, ncx=c->x, ncy=c->y, ncz=c->z;
  if (nbx - a->x > cx/2.0) nbx-=cx;
  if (nbx - a->x < -cx/2.0) nbx+=cx;
  if (nby - a->y > cy/2.0) nby-=cy;
  if (nby - a->y < -cy/2.0) nby+=cy;
  if (nbz - a->z > cz/2.0) nbz-=cz;
  if (nbz - a->z < -cz/2.0) nbz+=cz;
  if (ncx - a->x > cx/2.0) ncx-=cx;
  if (ncx - a->x < -cx/2.0) ncx+=cx;
  if (ncy - a->y > cy/2.0) ncy-=cy;
  if (ncy - a->y < -cy/2.0) ncy+=cy;
  if (ncz - a->z > cz/2.0) ncz-=cz;
  if (ncz - a->z < -cz/2.0) ncz+=cz;

  float pab=distance(a->x,a->y,a->z,nbx,nby,nbz,curtimestep);
  float pac=distance(a->x,a->y,a->z,ncx,ncy,ncz,curtimestep);
  float pbc=distance(nbx,nby,nbz,ncx,ncy,ncz,curtimestep);

  float ang=acos((pab*pab+pbc*pbc-pac*pac)/(2*pab*pbc));

  return ang*180/3.14159;
}
float angle(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, const Timestep *curtimestep) {
//angle abc, with b as the middle atom
  //cout << "in angle " << endl;
  float cx=curtimestep->cellx, cy=curtimestep->celly, cz=curtimestep->cellz;
  //cout << "celx " << curtimestep->cellx << endl;
  //float nbx=x2, nby=y2, nbz=z2, ncx=c->x, ncy=c->y, ncz=c->z;
  if (x2 - x1 > cx/2.0) x2-=cx;
  if (x2 - x1 < -cx/2.0) x2+=cx;
  if (y2 - y1 > cy/2.0) y2-=cy;
  if (y2 - y1 < -cy/2.0) y2+=cy;
  if (z2 - z1 > cz/2.0) z2-=cz;
  if (z2 - z1 < -cz/2.0) z2+=cz;
  if (x3 - x1 > cx/2.0) x3-=cx;
  if (x3 - x1 < -cx/2.0) x3+=cx;
  if (y3 - y1 > cy/2.0) y3-=cy;
  if (y3 - y1 < -cy/2.0) y3+=cy;
  if (z3 - z1 > cz/2.0) z3-=cz;
  if (z3 - z1 < -cz/2.0) z3+=cz;

  float pab=distance(x1,y1,z1,x2,y2,z2,curtimestep);
  float pac=distance(x1,y1,z1,x3,y3,z3,curtimestep);
  float pbc=distance(x2,y2,z2,x3,y3,z3,curtimestep);

  float ang=acos((pab*pab+pbc*pbc-pac*pac)/(2*pab*pbc));

  return ang*180/3.14159;
}

float signedangle(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, const AngleType angletype, const Timestep *t) {
   float nx1=x1,ny1=y1,nz1=z1,nx3=x3,ny3=y3,nz3=z3;
   float dummy=0;
   float v1x=0;
   float v1y=0;
   float v3x=0;
   float v3y=0;
   TranslateAtomCoordinatesToVicinity(nx1, ny1, nz1, x2, y2, z2, t);
   TranslateAtomCoordinatesToVicinity(nx3, ny3, nz3, x2, y2, z2, t);
   switch (angletype) {
        case AngleType::XY:
           v1x=nx1-x2;
           v1y=ny1-y2;
           v3x=nx3-x2;
           v3y=ny3-y2;
           break;
        case AngleType::YZ:
           v1x=ny1-y2;
           v1y=nz1-z2;
           v3x=ny3-y2;
           v3y=nz3-z2;
           break;
        case AngleType::ZX:
           v1x=nz1-z2;
           v1y=nx1-x2;
           v3x=nz3-z2;
           v3y=nx3-x2;
           break;
        case AngleType::XZ:
           v1x=nx1-x2;
           v1y=nz1-z2;
           v3x=nx3-x2;
           v3y=nz3-z2;
           break;
   }
   //TranslateAtomCoordinatesToVicinity(nx1, ny1, dummy, x2, y2, dummy, t);
   //TranslateAtomCoordinatesToVicinity(nx3, ny3, dummy, x2, y2, dummy, t);
   //float v1x=nx1-x2;
   //float v1y=ny1-y2;
   //float v3x=nx3-x2;
   //float v3y=ny3-y2;
   float ang=atan2(v3y,v3x) - atan2(v1y,v1x);
   ang*=180/3.14159;
   if (ang >=180) ang=-(360-ang);
   if (ang <-180) ang= 360+ang;
   return ang;
}

float dihedral(const Atom *a, const Atom *b, const Atom *c, const Atom *d, const Timestep *t)
{
  float ax=a->x,ay=a->y,az=a->z,bx=b->x,by=b->y,bz=b->z,cx=c->x,cy=c->y,cz=c->z,dx=d->x,dy=d->y,dz=d->z;
  TranslateAtomCoordinatesToVicinity(ax,ay,az,bx,by,bz,t);
  TranslateAtomCoordinatesToVicinity(cx,cy,cz,bx,by,bz,t);
  TranslateAtomCoordinatesToVicinity(dx,dy,dz,bx,by,bz,t);
  float r1,r2,r3,s1,s2,s3;
  //cross_prod(bx-ax, by-ay, bz-az, cx-ax, cy-ay, cz-az, r1, r2, r3);
  //cross_prod(cx-bx, cy-by, cz-bz, dx-bx, dy-by, dz-bz, s1, s2, s3);
  cross_prod(bx-ax, by-ay, bz-az, cx-bx, cy-by, cz-bz, r1, r2, r3);
  cross_prod(cx-bx, cy-by, cz-bz, dx-cx, dy-cy, dz-cz, s1, s2, s3);
  float numerator=(r1*s1+r2*s2+r3*s3); 
  float denominator=sqrt(r1*r1+r2*r2+r3*r3) * sqrt(s1*s1+s2*s2+s3*s3); 
  //cout << " r&s " << r1 << " " << r2 << " " << r3 << " " << s1 << " " << s2 << " " << s3 << " " << numerator << " " << denominator << " " << (180.0/3.14159)*(numerator/denominator) << endl;
  return (180.0/M_PI)*acos((numerator/denominator));
}

void cross_prod(float a1, float a2, float a3, float b1, float b2, float b3, float &r1, float &r2, float &r3) {
  r1=a2*b3-a3*b2;
  r2=a3*b1-a1*b3;
  r3=a1*b2-a2*b1;
}




/*
void CloseAllFiles(vector<ofstream*> & v) {
  for (auto x : v) {
    x->close();
    delete x;
  }
  //for (int i=0; i<v.size(); i++) {
  //  v[i]->close();
  //  delete v[i];
  //}
}


void CloseAllFiles(vector<ifstream*> & v) {
  for (int i=0; i<v.size(); i++) {
    v[i]->close();
    delete v[i];
  }
}
*/

/*
void addGroup(vector<Group> &v, const Group & g) {
    //playing around with lambda expressions
    if (std::find_if(v.begin(), v.end(), 
                    [&g] (const Group &x) -> bool { return x.getId() == g.getId(); }
                    ) != v.end()) {
            throw string("ERROR! Group with name " + g.getId() + " already exists.");
            return;
    }
    else {
        v.push_back(g);
    }
}
*/

Group *addGroup(vector<Group*> &v, Group *g) {
    //playing around with lambda expressions
    if (std::find_if(v.begin(), v.end(), 
                    [&g] (Group *x) -> bool { return x->getId() == g->getId(); }
                    ) != v.end()) {
            throw string("ERROR! Group with name " + g->getId() + " already exists.");
            return nullptr;
    }
    else {
        v.push_back(g);
        return g;
    }
}

Action *addAction(vector<Action*> &v, Action *a) {
    v.push_back(a);
    return a;
}

CObject *addObject(vector<CObject*> &v, CObject *a) {
    v.push_back(a);
    return a;
}



GROUP_PROPERTY StringToProperty(const std::string &propertyname) {
    if (propertyname == "x") { return GROUP_PROPERTY::X; }
    else if (propertyname == "y") { return GROUP_PROPERTY::Y; }
    else if (propertyname == "z") { return GROUP_PROPERTY::Z; }
    else if (propertyname == "xu") { return GROUP_PROPERTY::XU; }
    else if (propertyname == "yu") { return GROUP_PROPERTY::YU; }
    else if (propertyname == "zu") { return GROUP_PROPERTY::ZU; }
    else if (propertyname == "vx") { return GROUP_PROPERTY::VX; }
    else if (propertyname == "vy") { return GROUP_PROPERTY::VY; }
    else if (propertyname == "vz") { return GROUP_PROPERTY::VZ; }
    else if (propertyname == "q") { return GROUP_PROPERTY::Q; }
    else if (propertyname == "qaverage") { return GROUP_PROPERTY::QAVERAGE; }
    else if (propertyname == "coordinationnumber") { return GROUP_PROPERTY::COORDINATIONNUMBER; }
    else if (propertyname == "id") { return GROUP_PROPERTY::ID; }
    else if (propertyname == "type") { return GROUP_PROPERTY::TYPE; }
    else if (propertyname == "distances") { return GROUP_PROPERTY::DISTANCES; }
    else if (propertyname == "numatoms") { return GROUP_PROPERTY::NUMATOMS; }
    else if (propertyname == "timestepnumber") { return GROUP_PROPERTY::TIMESTEPNUMBER; }
    else if (propertyname == "timestepiteration") { return GROUP_PROPERTY::TIMESTEPITERATION; }
    else if (propertyname == "totalsize") { return GROUP_PROPERTY::TOTALSIZE; }
    else if (propertyname == "averagesize") { return GROUP_PROPERTY::AVERAGESIZE; }
    else if (propertyname == "name" || propertyname=="groupname") { return GROUP_PROPERTY::NAME; }
    else if (propertyname == "coordinationtable") { return GROUP_PROPERTY::COORDINATIONTABLE; }
    else if (propertyname == "coordinationtablex") { return GROUP_PROPERTY::COORDINATIONTABLEX; }
    else if (propertyname == "coordinationtabley") { return GROUP_PROPERTY::COORDINATIONTABLEY; }
    else if (propertyname == "coordinationtablez") { return GROUP_PROPERTY::COORDINATIONTABLEZ; }
    else if (propertyname == "coordinationtypes") { return GROUP_PROPERTY::COORDINATIONTYPES; }
    else if (propertyname == "atomtypes") { return GROUP_PROPERTY::ATOMTYPES; }
    else if (propertyname == "bonds") { return GROUP_PROPERTY::BONDS; }
    else if (propertyname == "bond0") { return GROUP_PROPERTY::BOND0; }
    else if (propertyname == "bond1") { return GROUP_PROPERTY::BOND1; }
    else if (propertyname == "bond2") { return GROUP_PROPERTY::BOND2; }
    else if (propertyname == "bond3") { return GROUP_PROPERTY::BOND3; }
    else if (propertyname == "delta") { return GROUP_PROPERTY::DELTA; }
    else if (propertyname == "cell") { return GROUP_PROPERTY::CELL; }
    else { 
        throw string("Error!! Unknown property: ")+propertyname; 
    }
}

string PropertyToString(const GROUP_PROPERTY a) {
    switch (a) {
        case GROUP_PROPERTY::X : return "x"; 
        case GROUP_PROPERTY::Y : return "y";
        case GROUP_PROPERTY::Z : return "z";
        case GROUP_PROPERTY::Q : return "q";
        case GROUP_PROPERTY::XU : return "xu"; 
        case GROUP_PROPERTY::YU : return "yu";
        case GROUP_PROPERTY::ZU : return "zu";
        case GROUP_PROPERTY::VX : return "vx"; 
        case GROUP_PROPERTY::VY : return "vy";
        case GROUP_PROPERTY::VZ : return "vz";
        case GROUP_PROPERTY::QAVERAGE : return "qaverage";
        case GROUP_PROPERTY::COORDINATIONNUMBER : return "coordinationnumber";
        case GROUP_PROPERTY::ID : return "id";
        case GROUP_PROPERTY::TYPE : return "type"; 
        case GROUP_PROPERTY::DISTANCES : return "distances"; 
        case GROUP_PROPERTY::NUMATOMS : return "numatoms"; 
        case GROUP_PROPERTY::TIMESTEPNUMBER : return "timestepnumber"; 
        case GROUP_PROPERTY::TIMESTEPITERATION : return "timestepiteration"; 
        case GROUP_PROPERTY::TOTALSIZE: return "totalsize";
        case GROUP_PROPERTY::AVERAGESIZE: return "averagesize";
        case GROUP_PROPERTY::NAME : return "name"; 
        case GROUP_PROPERTY::COORDINATIONTABLE : return "coordinationtable"; 
        case GROUP_PROPERTY::DELTA : return "delta"; 
    }
    return "unknown";
}


void calculateVelocities(const vector<Atom>& oldatoms, vector<Atom>& newatoms) {
    //both oldatoms and newatoms contain x, y, and z members that have been initialized
    if (oldatoms.size() != newatoms.size()) {
        throw string("ERROR! in calculateVelocities, oldatoms.size()!=newatoms.size()");
    }
    for (int i=0; i<newatoms.size(); i++) {
        newatoms[i].vx = newatoms[i].xu-oldatoms[i].xu;
        newatoms[i].vy = newatoms[i].yu-oldatoms[i].yu;
        newatoms[i].vz = newatoms[i].zu-oldatoms[i].zu;
    }
}
void calculateMomentum(const vector<Atom>& atoms, Timestep *timestep) {
    float momx=0, momy=0, momz=0;
    for (int i=0; i<atoms.size(); i++) {
        cout << "momx += " << atoms[i].vx << " * " << atoms[i].mass << " " << atoms[i].id << " " << atoms[i].type << endl;
        momx+=atoms[i].vx*atoms[i].mass;
        momy+=atoms[i].vy*atoms[i].mass;
        momz+=atoms[i].vz*atoms[i].mass;
    }
    timestep->momx=momx;
    timestep->momy=momy;
    timestep->momz=momz;
}


void intelligentUnwrap(const vector<Atom>& oldatoms, vector<Atom>& newatoms, const Timestep *timestep) {
    //both oldatoms and newatoms contain x, y, and z members that have been initialized
    if (oldatoms.size() != newatoms.size()) {
        throw string("ERROR! in intelligentUnwrap, oldatoms.size()!=newatoms.size()");
    }
    for (int i=0; i<newatoms.size(); i++) {
        TranslateAtomCoordinatesToVicinity(newatoms[i].xu, newatoms[i].yu, newatoms[i].zu, oldatoms[i].xu, oldatoms[i].yu, oldatoms[i].zu, timestep);
    }
}


void deleteObjects(vector<CObject*> &v) {
    for (auto &x : v) {
        delete x;
    }
}


int stringAtomToNumAtom(const string &s) {
    if (s == "H" || s == "h") return 1;
    if (s == "C" || s == "c") return 6;
    if (s == "O" || s == "o") return 8;
    if (s == "Na" || s == "na") return 11;
    if (s == "Cl" || s == "cl") return 17;
    if (s == "Cu" || s == "cu") return 29;
    if (s == "Zn" || s == "zn") return 30;
    if (s == "Ce" || s == "ce") return 58;
    throw string("Unknown atomic species: " + s);
}

float getAtomMass(const int atomicnumber) {
    switch (atomicnumber) {
        case 1: return 1.008;
        case 6: return 12.00;
        case 8: return 16.00;
        case 11: return 22.99;
        case 17: return 35.45;
        case 29: return 63.55;
        case 30: return 65.39;
        case 58: return 140.1;
        default: throw string("Unknown atomic number for mass calculation: " + atomicnumber);
    }
}


Consequence stringToConsequence(const string& s) {
    if (s == "ur" || s == "uncareremaining") return Consequence::UncareRemaining;
    else if (s == "ut" || s == "uncarethis") return Consequence::UncareThis;
    else if (s == "ua" || s == "uncareall") return Consequence::UncareAll;
    else if (s == "vr" || s == "valueremaining") return Consequence::ValueRemaining;
    else if (s == "vt" || s == "valuethis") return Consequence::ValueThis;
    else if (s == "no" || s == "noconsequence") return Consequence::NoConsequence;
    else throw string ("Unknown consequence: " + s);
}

void parseTCFOptions(vector<string> &w, vector<string> &origw, ActionTTCFParent* a, vector<Action*> &vpActions, vector<Group*> &vpGroups, vector<NamedConst> &vNamedConsts, float basictimeunit, int superevery, string outputfileprefix, string outputfilesuffix, Overwrite& overwritefiles) {
    bool explicitly_set_timeunit=false;
    bool realtime=false;
    int every=1, printevery=0;
    float timeunit=1;
    Group *group1=nullptr;
    Group *parentgroup=FindGroupWithId(vpGroups, "all");
    Group *group2=nullptr;
    Group *shellgroup=nullptr;
    Group *shellgroup1=nullptr;
    Group *shellgroup2=nullptr;
    string msdfilename(""), manipulateoutfilename(""), vectoroutfilename(""), printidsfilename(""), printpositionsfilename(""); float msdmaxtime=0;
    double manipulatemin=3200, manipulatemax=3800;
    int manipulatetype=0;
    string totalfilename;
    float correlationfrequency=1;
    string xyzstring="xyz";
    vector<TCFCondition> vTCFConditions;
    bool explicitly_set_correlation_frequency=false;
    bool printallmembers=false;
    string id="";
    bool usealltimeorigins=true;
    bool uncareallwhenuncareremaining=false;
    bool printrawnumbers=true;
    bool discretizecontinuous=false, discretizediscrete=false;
    for (int i=1; i<w.size(); i++) {
        if (w[i] == "every" && w.size()>i+1) { 
            from_string<int>(every, w[i+1]); 
            a->setEvery(every);
            i++; 
        }
        else if (w[i] == "printevery" && w.size()>i+1) {
            from_string<int>(printevery, w[i+1], &vNamedConsts);
            a->setPrintEvery(printevery);
            i++;
        }
        else if (w[i] == "filename") {
            msdfilename=origw[i+1];
            i++;
        }
        else if (w[i] == "group" || w[i] == "group1" || w[i] == "reactants") {
            group1=FindGroupWithId(vpGroups, w[i+1]);
            i++;
        }
        else if ( (w[0]=="tresidencetimessp" || w[0]=="tresidencetimesspcoordinationshell") && (w[i] == "group2" || w[i] == "products") && w.size()>i+1) {
            group2=FindGroupWithId(vpGroups, w[i+1]);
            i++;
        }
        //else if (dynamic_cast<ActionTResidenceTimeSSPCoordinationShell*>(a) != nullptr && w[i] == "wgaction") {
        //    ActionWhenGroupChangedParent *mypAWGC=dynamic_cast<ActionWhenGroupChangedParent*>(FindActionWithId(vpActions, w[i+1]));
        //    if (mypAWGC == nullptr) { throw string("Need ActionWhenGroupChanged kind of action for TResidenceTimeSSPCoordinationShell"); }
        //    static_cast<ActionTResidenceTimeSSPCoordinationShell*>(a)->addActionWhenGroupChanged(mypAWGC);
        //    i++;
        //}
        else if (w[i] == "maxhistory") {
            from_string<float>(msdmaxtime, w[i+1], &vNamedConsts);
            i++;
        }
        else if (w[i] == "timeunit" && w.size()>i+1) { 
            from_string<float>(timeunit, w[i+1], &vNamedConsts); 
            a->setTimeUnit(timeunit);
            explicitly_set_timeunit=true;
            i++; 
        }
        else if (w[i] == "readrestart" && w.size()>i+1) {
            a->setReadRestart(origw[i+1]);
            i++;
        }
        else if (w[i] == "writerestart" && w.size()>i+1) {
            a->setWriteRestart(origw[i+1]);
            i++;
        }
        else if (w[i] == "dontprintrawnumbers") {
            printrawnumbers=false;
            a->setDoPrintRawNumbers(false);
        }
        else if (w[i] == "dontusealltimeorigins") {
            usealltimeorigins=false;
            a->setUseAllTimeOrigins(false);
        }
        else if (w[i] == "parentgroup") { 
            parentgroup=FindGroupWithId(vpGroups, w[i+1]);  //added to action below
            i++; 
        }
        else if (w[i] == "uncareallwhenuncareremaining") { a->setUncareAllWhenUncareRemaining(true); }
        else if (w[i] == "valueprecedenceoveruncare") { a->setValuePrecedenceOverUncare(true); }
        else if (w[i] == "printallmembers") { a->setDoPrintAllMembers(true); }
        else if (w[i] == "noheader") {a->setDoPrintHeader(false); }
        else if (w[i] == "printheader") {a->setDoPrintHeader(true); }
        else if (w[i] == "average") { a->setAverage(true); }
        else if (w[i] == "noaverage") { a->setAverage(false); }
        //else if ( (w[i] == "oldmustbemember1"  || w[i] == "newmustbecontinuousmember1" || w[i] == "newmustnotbemember1" || w[i] == "newmustnotbemember2") && w.size() > i+1) {
        //    TCFCondition x(stringToTCFConditionType(w[i]));
        //    //x.g1=FindGroupWithId(vpGroups, w[i+1]); //this group is NOT used at the moment
        //    x.consequence=stringToConsequence(w[i+1]);
        //    vTCFConditions.push_back(x);
        //    //a->addCondition(x);
        //    i+=1;
        //}
        else if (w[i] == "id" && w.size()>i+1) {
            a->setId(w[i+1]);
            i++;
        }
        //else if (w[i] == "tcffrom" && w.size()>i+1) {
        //    ActionTTCFParent *atcffrom=dynamic_cast<ActionTTCFParent*>(FindActionWithId(vpActions, w[i+1]));
        //    if (atcffrom == nullptr) throw string("TCFFrom must be of type ActionTTCFParent!");
        //    a->setTCFFrom(atcffrom);
        //    i++;
        //}
        else if  (
            (   w[i] == "newmemberescape1" ||
                w[i] == "oldmemberescape1" ||
                w[i] == "newmemberescape2" ||
                w[i] == "oldmemberescape2" ||
                w[i] == "newvalueescape1" ||
                w[i] == "newvalueescape2" ||
                w[i] == "oldvalueescape1" ||
                w[i] == "oldvalueescape2" ||
                w[i] == "newmemberescape3" ||
                w[i] == "oldmemberescape3" ||
                w[i] == "newmemberescape4" ||
                w[i] == "oldmemberescape4" ||
                w[i] == "oldcoordinationescape1" ||
                w[i] == "newcoordinationescape1" ||
                w[i] == "newjumpescape1"  ||
                w[i] == "newmemberescape1!" ||
                w[i] == "oldmemberescape1!" ||
                w[i] == "newmemberescape2!" ||
                w[i] == "oldmemberescape2!" ||
                w[i] == "newvalueescape1!" ||
                w[i] == "newvalueescape2!" ||
                w[i] == "oldvalueescape1!" ||
                w[i] == "oldvalueescape2!" ||
                w[i] == "newmemberscape3!" ||
                w[i] == "oldmemberescape3!" ||
                w[i] == "newmemberescape4!" ||
                w[i] == "oldmemberescape4!"  ||
                w[i] == "oldcoordinationescape1!" ||
                w[i] == "newcoordinationescape1!" ||
                w[i] == "newjumpescape1!"  ||
                false
            ) 
                        && w.size()>i+4) {
            TCFConditionType tcfct=stringToTCFConditionType(w[i]);
            TCFCondition x(tcfct);
            //cout << " bii " << x.bii << endl;
            x.bii = vTCFConditions.size();
            //cout << " bii2 " << x.bii << endl;
            from_string<double>(x.d1, w[i+1], &vNamedConsts); //history
            from_string<double>(x.d2, w[i+2], &vNamedConsts); //continuosu escape
            from_string<double>(x.d3, w[i+3], &vNamedConsts); //total escape
            x.consequence=stringToConsequence(w[i+4]);
            if (w[i][w[i].length()-1] == '!') x.negate=true;
            //the conversion to "timestep units" is done internally in a->addCondition()
            if (w[i][3] == 'c' && w[0] != "tresidencetimecoordinationshell" && w[0] != "tresidencetimesspcoordinationshell") {
                throw string("Error! " + origw[i] + " is only available for TResidenceTimeCoordinationShell and TResidenceTimeSSPCoordinationShell");
            }
            if (w[i][3] == 'm' || w[i][3] == 'c') { //member or coordination, not vlaue
                if ( w[i+5] == "@" && group1 != nullptr && (tcfct == TCFConditionType::OldMustBeMemberFor1 || tcfct == TCFConditionType::NewMustBeMemberFor1 || tcfct == TCFConditionType::OldCoordinationFor1)) {
                    x.g1 = FindGroupWithId(vpGroups, group1->getId());
                }
                else if ( w[i+5] == "@" && group2 != nullptr && (tcfct == TCFConditionType::OldMustBeMemberFor2 || tcfct == TCFConditionType::NewMustBeMemberFor2 )) {
                    x.g1 = FindGroupWithId(vpGroups, group2->getId());
                }
                else {
                    x.g1 = FindGroupWithId(vpGroups, w[i+5]);
                }
                if (w[i][3] == 'c') { //oldcoordinationescape, newcoordinationescape
                    if (dynamic_cast<GroupCoordination*>(x.g1) == nullptr) {
                        throw string("Error! Must provide group of type GroupCoordination to " + origw[i]);
                    }
                }
                i++;
            }
            vTCFConditions.push_back(x);
            i+=4;
        }
        else if (w[i] == "softstart") {
            for (int j=0; j<vTCFConditions.size(); j++) {
                vTCFConditions[j].softhistory = true;
            }
        }
        else if (w[i] == "tidyold" && w.size()>i+1) {
            int tidyold; from_string<int>(tidyold, w[++i]);
            a->setTidyOld(tidyold);
        }
        else if (w[i] == "realtime") {
            realtime=true;
        }
        else if (w[i] == "correlationfrequency") {
            from_string<float>(correlationfrequency, w[i+1], &vNamedConsts);
            explicitly_set_correlation_frequency=true;
            ++i;
        }
        else if ( (w[0] == "tmsd" || w[0] == "tvacf" || w[0] == "tmsdfollow") &&
            (w[i] == "x" || w[i] == "y" || w[i] == "z" || w[i] == "xy" || w[i] == "yz" || w[i] == "xz" || w[i] == "xyz") ) {
                xyzstring=w[i];

        }
        else if ( w[0] == "tmsdfollow" && w[i] == "discretizecontinuous") {
            a->setOption(ActionTTCFOptions::Discretize, true);
            a->setOption(ActionTTCFOptions::DiscretizeContinuous, true);
            a->setOption(ActionTTCFOptions::DiscretizeDiscrete, false);
        }
        else if ( w[0] == "tmsdfollow" && w[i] == "discretizediscrete") {
            a->setOption(ActionTTCFOptions::Discretize, true);
            a->setOption(ActionTTCFOptions::DiscretizeContinuous, false);
            a->setOption(ActionTTCFOptions::DiscretizeDiscrete, true);
        }
        else if ( w[0] == "tmsdfollow" && w[i] == "hungarian" ) {
            a->setOption(ActionTTCFOptions::Hungarian, true);
        }
        else if (w[0] == "tmsdfollow" && w[i] == "hungariansquaredpenalty") {
            a->setOption(ActionTTCFOptions::HungarianSquaredPenalty, true);
        }
        else if ( w[0] == "tmsdfollow" && w[i] == "ignoreconflicts" ) {
            a->setOption(ActionTTCFOptions::IgnoreConflicts, true);
        }
        else if ( w[0] == "tmsdfollow" && w[i] == "vectorout" && w.size()>i+1) {
            a->setOption(ActionTTCFOptions::DoPrintVectorOut, true);
            vectoroutfilename=origw[i+1];
            i++;
        }
        else if ( w[0] == "tmsdfollow" && w[i] == "printidsfilename" && w.size()>i+1) {
            printidsfilename=origw[i+1];
            i++;
        }
        else if ( w[0] == "tmsdfollow" && w[i] == "printpositionsfilename" && w.size()>i+1) {
            printpositionsfilename=origw[i+1];
            i++;
        }
        else if ( w[0] == "tmsdfollow" && w[i] == "external" && w.size()>i+1) {
            Group *externalgroup = FindGroupWithId(vpGroups, w[i+1]);
            if (dynamic_cast<GroupExternal*>(externalgroup) != nullptr) {
                static_cast<ActionTMSDFollow*>(a)->setExternalGroup(static_cast<GroupExternal*>(externalgroup));
            }
            else {
                throw string("Expected EXTERNAL group for TMSDFollow command external");
            }
            i++;
        }
        else if ( ( w[0] == "tresidencetimecoordinationshell" || w[0] == "tresidencetimesspcoordinationshell") && w[i] == "shellgroup") {
            shellgroup=FindGroupWithId(vpGroups, w[i+1]);
            i++;
        }
        else if ( ( w[0] == "tresidencetimecoordinationshell" || w[0] == "tresidencetimesspcoordinationshell") && (w[i] == "conditionshell3") ) {
            shellgroup1=FindGroupWithId(vpGroups, w[i+1]);
            i++;
        }
        else if (w[0] == "tvacf" && w[i]=="manipulateout" & w.size()>i+4) {
            manipulateoutfilename=origw[i+1];
            from_string<double>(manipulatemin, w[i+2], &vNamedConsts);
            from_string<double>(manipulatemax, w[i+3], &vNamedConsts);
            if (w[i+4] == "peak") manipulatetype=0;
            else if (w[i+4] == "wavg") manipulatetype=1;
            else throw string("unknown manipulation type");
            i+=4;
        }
        else throw string("Unknown flags for " + w[0] + " command: " + w[i]);
    }
    if (group1 == nullptr) throw string ("ERROR! You must specify Group! ");
    if (!explicitly_set_timeunit) {
        a->setTimeUnit(basictimeunit, superevery, every);
    }
    a->setMaxTime(msdmaxtime, realtime);
    if (explicitly_set_correlation_frequency) {
        //wait with setting until you know if you use realtime
        //only call the function of the correlation frequency was specified, otherwise you might set the correlation frequency to 1 ps (instead of 1 frame) as default, when realtime is set
        a->setCorrelationFrequency(correlationfrequency, realtime);
    }
    a->addGroup(parentgroup);
    a->addGroup(group1);
    if (group2 != nullptr) a->addGroup(group2);
    if ( (w[0] == "tresidencetimessp" || w[0] == "tresidencetimesspcoordinationshell") && group2 == nullptr) {
        throw string("ERROR! Must give Products for command " + w[0]);
    }
    if ( (w[0] == "tresidencetimecoordinationshell" || w[0] == "tresidencetimesspcoordinationshell") && shellgroup == nullptr) {
        throw string("ERROR! Must give ShellGroup for command " + w[0]);
    }
    if (shellgroup != nullptr) a->addGroup(shellgroup);
    if (shellgroup1 != nullptr) a->addGroup(shellgroup1);
    if (shellgroup2 != nullptr) a->addGroup(shellgroup2);
    a->setShellGroup(shellgroup);
    a->setShellGroup1(shellgroup1);
    a->setShellGroup2(shellgroup2);
    totalfilename=outputfileprefix+msdfilename+outputfilesuffix;
    if (!permission_to_write(totalfilename, overwritefiles)) throw string("no permission to write to " + totalfilename);
    a->addOut(totalfilename);
    if (manipulateoutfilename != "") {
        totalfilename=outputfileprefix+manipulateoutfilename+outputfilesuffix;
        if (!permission_to_write(totalfilename, overwritefiles)) throw string("no permission to write to " + totalfilename);
        a->addOut(manipulateoutfilename);
        a->setManipulateIndividualTCFs(true, manipulatemin, manipulatemax, manipulatetype);
    }
    if (vectoroutfilename != "") {
        totalfilename=outputfileprefix+vectoroutfilename+outputfilesuffix;
        if (!permission_to_write(totalfilename, overwritefiles)) throw string("no permission to write to " + totalfilename);
        a->addOut(vectoroutfilename);
        a->setOption(ActionTTCFOptions::DoPrintVectorOut, true);
    }
    if (printidsfilename != "") {
        totalfilename=outputfileprefix+printidsfilename+outputfilesuffix;
        if (!permission_to_write(totalfilename, overwritefiles)) throw string("no permission to write to " + totalfilename);
        dynamic_cast<ActionTMSDFollow*>(a)->addPrintIdsFilename(printidsfilename);
    }
    if (printpositionsfilename != "") {
        totalfilename=outputfileprefix+printpositionsfilename+outputfilesuffix;
        if (!permission_to_write(totalfilename, overwritefiles)) throw string("no permission to write to " + totalfilename);
        dynamic_cast<ActionTMSDFollow*>(a)->addPrintPositionsFilename(printpositionsfilename);
    }
    if (w[0] == "tmsd" || w[0] == "tvacf" || w[0] == "tmsdfollow") {
        if      (xyzstring == "xyz") { a->setNumItemsPerTimeStep(3); a->setDimX(true); a->setDimY(true); a->setDimZ(true); }
        else if (xyzstring == "xy")  { a->setNumItemsPerTimeStep(2); a->setDimX(true); a->setDimY(true); a->setDimZ(false); }
        else if (xyzstring == "yz")  { a->setNumItemsPerTimeStep(2); a->setDimX(false); a->setDimY(true); a->setDimZ(true); }
        else if (xyzstring == "xz")  { a->setNumItemsPerTimeStep(2); a->setDimX(true); a->setDimY(false); a->setDimZ(true); }
        else if (xyzstring == "x")   { a->setNumItemsPerTimeStep(1); a->setDimX(true); a->setDimY(false); a->setDimZ(false); }
        else if (xyzstring == "y")   { a->setNumItemsPerTimeStep(1); a->setDimX(false); a->setDimY(true); a->setDimZ(false); }
        else if (xyzstring == "z")   { a->setNumItemsPerTimeStep(1); a->setDimX(false); a->setDimY(false); a->setDimZ(true); }
    }

    for (int i=0; i<vTCFConditions.size(); i++) {
        a->addCondition(vTCFConditions[i], realtime); //this is done here so that the timeunit is properly set in the action
        // Action->addCondition then converts to to timestep units if realtime has been specified
    }
}

TCFConditionType stringToTCFConditionType(const string& s) {
         if (s == "oldmemberescape1" || s == "oldmemberescape1!") return TCFConditionType::OldMustBeMemberFor1;
    else if (s == "oldmemberescape2" || s == "oldmemberescape2!") return TCFConditionType::OldMustBeMemberFor2;
    else if (s == "oldmemberescape3" || s == "oldmemberescape3!") return TCFConditionType::OldMustBeMemberFor3;
    else if (s == "oldmemberescape4" || s == "oldmemberescape4!") return TCFConditionType::OldMustBeMemberFor4;
    else if (s == "newmemberescape1" || s == "newmemberescape1!") return TCFConditionType::NewMustBeMemberFor1;
    else if (s == "newmemberescape2" || s == "newmemberescape2!") return TCFConditionType::NewMustBeMemberFor2;
    else if (s == "newmemberescape3" || s == "newmemberescape3!") return TCFConditionType::NewMustBeMemberFor3;
    else if (s == "newmemberescape4" || s == "newmemberescape4!") return TCFConditionType::NewMustBeMemberFor4;
    else if (s == "oldvalueescape1"  || s == "oldvalueescape1!") return TCFConditionType::OldValueIsTrueFor1;
    else if (s == "oldvalueescape2"  || s == "oldvalueescape2!") return TCFConditionType::OldValueIsTrueFor2;
    else if (s == "newvalueescape1"  || s == "newvalueescape1!") return TCFConditionType::NewValueIsTrueFor1;
    else if (s == "newvalueescape2"  || s == "newvalueescape2!") return TCFConditionType::NewValueIsTrueFor2;
    else if (s == "oldcoordinationescape1" || s == "oldcoordinationescape1!") return TCFConditionType::OldCoordinationFor1;
    else if (s == "newcoordinationescape1" || s == "newcoordinationescape1!") return TCFConditionType::NewCoordinationFor1;
    else if (s == "newjumpescape1" || s == "newjumpescape1!") return TCFConditionType::NewJumpMemberFor1;
    else throw string ("unknown tcfcondition type");
    //else if (s == "newmustbecontinuousmember4") return TCFConditionType::NewMustBeContinuousMember4;
    //else if (s == "oldmustbemember1" || s == "oldismember1") return TCFConditionType::OldIsMember1;
    //else if (s == "newmustbemember1" || s == "newismember1") return TCFConditionType::NewIsMember1;
    //else if (s == "newmustbecontinuousmember1") return TCFConditionType::NewMustBeContinuousMember1;
    //else if (s == "oldmustbemember2" || s == "oldismember2") return TCFConditionType::OldIsMember2;
    //else if (s == "newmustbemember2" || s == "newismember2") return TCFConditionType::NewIsMember2;
    //else if (s == "newmustbecontinuousmember2") return TCFConditionType::NewMustBeContinuousMember2;
    //else if (s == "oldmustbemember3" || s == "oldismember3") return TCFConditionType::OldIsMember3;
    //else if (s == "newmustbemember3" || s == "newismember3") return TCFConditionType::NewIsMember3;
    //else if (s == "newmustbecontinuousmember3") return TCFConditionType::NewMustBeContinuousMember3;
    //else if (s == "oldmustbemember4" || s == "oldismember4") return TCFConditionType::OldIsMember4;
    //else if (s == "newmustbemember4" || s == "newismember4") return TCFConditionType::NewIsMember4;
}

//http://stackoverflow.com/questions/447206/c-isfloat-function
bool isFloat( string myString ) {
    std::istringstream iss(myString);
    float f;
    iss >> noskipws >> f; // noskipws considers leading whitespace invalid
    // Check the entire string was consumed and if either failbit or badbit is set
    return iss.eof() && !iss.fail(); 
}


//http://stackoverflow.com/questions/5840148/how-can-i-get-a-files-size-in-c
std::ifstream::pos_type getFileSize(const string& filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg(); 
}


//http://stackoverflow.com/questions/5878775/how-to-find-and-replace-string
//global replace
std::string ReplaceString(std::string subject, const std::string& search, const std::string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
    return subject;
}
