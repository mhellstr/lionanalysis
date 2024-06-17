#ifndef ACTIONMSDFOLLOW_H
#define ACTIONMSDFOLLOW_H


#include "actionmsd.h"

class ActionMSDFollow : public ActionMSD {
public:
    ActionMSDFollow();
protected:
    void push_back_in_data(const Timestep *) override;
    vector<float> mOldPositions;
    float getShortDistance(int atomindex, const Timestep *t, int& switchindex, const vector<int> &vIgnore);
};


#endif
