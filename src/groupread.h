#ifndef GROUPREAD_H
#define GROUPREAD_H

#include "group.h"

class GroupRead : public Group {
public:
    GroupRead(string s);
    void setReadStream(ifstream *a) {mReadStream=a;}
protected:
    void internalUpdate(const vector<Atom> *, const Timestep *) override;
    ifstream *mReadStream;
};



#endif
