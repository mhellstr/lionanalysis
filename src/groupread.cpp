#include "groupread.h"
#include <sstream>

GroupRead::GroupRead(string s) 
: Group(GROUP_TYPE::READ, s), mReadStream(nullptr)
{
}


void GroupRead::internalUpdate(const vector<Atom> *allatoms, const Timestep *t)
{
    clear();
    if ( mReadStream->eof() ) {
        throw(string("ERROR: In group ")+mId+string(" I am reading past the end of file... FIX IT!"));
    }
    string s;
    getline( (*mReadStream), s);
    stringstream ss(s);
    int i;
    while (ss >> i) {
        addAtom(&((*allatoms)[i]));
    }

}
