#ifndef ATOMDURATIONDELAY_H
#define ATOMDURATIONDELAY_H

class AtomDurationDelay {
public:
    AtomDurationDelay() :atom(nullptr), duration(1), delay(0) {};
    AtomDurationDelay(const Atom* a, int du, int de) :atom(a), duration(du), delay(de) {};
    const Atom* atom;
    int duration;
    int delay;
};


#endif
