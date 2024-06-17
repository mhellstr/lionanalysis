#ifndef HISTOGRAM_H
#define HISTOGRAM_H
#include <vector>

class Histogram {
public:
    Histogram();
    virtual ~Histogram();
    void addToInternalIndex(int index, double v=1, int weight=1);
    void add(double x, double v=1, int weight=1);
    void setResolution(double x) {mResolution=x;}
    double sum() const;
    long long int sumCount() const;
    int getNumElements() const;
    int getNumElements(double, double) const;
    int realToIndex(double) const;
    int realToIndex(double, double, double) const;
    double indexToReal(int) const;
    double getElement(int a) const { return mData[a].first; }
    long long int getCount(int a) const { return mData[a].second; }
    int size() const { return mData.size(); }
    void setMinVal(double x) { mMinVal = x; }
    void setMaxVal(double x) { mMaxVal = x; }
    void setDynamicRange(bool x) { mDynamicRange = x; }
    void setSaveRawData(bool x) { mSaveRawData = x; }
    double getMinVal() const { return mMinVal; }
    double getMaxVal() const { return mMaxVal; }
    double getResolution() const { return mResolution; }
    double weightedAverage() const;
    void initialize(); //called in add(), in case of ris kof empty ehistogram should be called from outside
protected:
    double mMinVal;
    double mMaxVal;
    double mResolution;
    std::vector<std::pair<double, long long int>> mData;
    void resizeDataVector(double, double, double, double);
    bool mDynamicRange;
    std::vector<double> mRawData;
    bool mSaveRawData;
};

#endif
