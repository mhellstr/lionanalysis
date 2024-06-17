#ifndef COBJECT_H
#define COBJECT_H
#include <vector>
#include "atom.h"
#include "timestep.h"


class CObject {
public:
    CObject() { };
    virtual ~CObject() {};
    virtual void update(const std::vector<Atom> *, const Timestep *) = 0;
};

#endif
