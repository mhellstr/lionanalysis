#include "atom.h"

Atom::Atom() 
:id(0),type(string("0")),x(0),y(0),z(0),q(0),sumbo(0),nlp(0),printed(false),mass(1), delta(0), fx(0), fy(0), fz(0)
{
  //bondedatoms.clear(); bondedbos.clear();
  //bondedatoms.reserve(10); bondedbos.reserve(10);
  bonds.clear();
}

Atom::~Atom()
{
}

Atom::Atom(int iid, string t, float xx, float yy, float zz, float qq, float ssumbo, float nnlp)
:id(iid), type(t), x(xx), y(yy), z(zz), q(qq), sumbo(ssumbo), nlp(nnlp), printed(false),mass(1)
{
  //bondedatoms.clear();
  //bondedbos.clear();
  //bondedatoms.reserve(10); bondedbos.reserve(10);
  bonds.clear();
}

