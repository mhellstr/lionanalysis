#ifndef ACTIONHISTOGRAM_H
#define ACTIONHISTOGRAM_H

#include "action.h"
#include "group.h"
#include "histogram.h"

enum class HistogramDataType { Count, CoordinationNumber };

class ActionHistogram : public Action {
public:
    ActionHistogram();
    virtual ~ActionHistogram();
    void addProperty(GROUP_PROPERTY x) { mvProperties.push_back(x); }
    void addValue(int weight=1);
    void setMinVal(double);
    void setMaxVal(double);
    void setResolution(double);
    void setDynamicRange(bool);
    void setDataType(HistogramDataType x) {mDataType = x; }
protected:
    virtual void internalAction(const vector<Atom> *, const Timestep *) override;
    virtual void internalFinalAction(const vector<Atom> *, const Timestep *) override;
    void internalPrint(const vector<Atom> *allatoms, const Timestep *timestep) override;
    double mResolution;
    double mMinVal;
    std::vector<GROUP_PROPERTY> mvProperties; 
    Histogram mHistogram;
    void add(double, int x=1);
    bool mPrintHeader;
    HistogramDataType mDataType;
private:
};


#endif
