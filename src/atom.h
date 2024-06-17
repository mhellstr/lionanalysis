#ifndef ATOM_H
#define ATOM_H

#include <vector>
#include <string>
using namespace std;


class Atom {
public:
  Atom(int,string,float,float,float,float,float,float);
  ~Atom();
  Atom();
  int id;
  string type;
  float x,y,z,q,sumbo,nlp,xu,yu,zu,vx,vy,vz,vtot; //xu = unwrapped x coordinate
  float fx,fy,fz;
  float mass;
  bool printed;
  vector<float> bonds;
  float delta;
};

typedef vector<Atom*> PAtomVector;

#endif

