#include "histogram.h"
#include <vector>
#include <numeric>
#include <string>
#include <iostream>
#include <utility>

Histogram::Histogram()
: mMinVal(0), mMaxVal(1), mResolution(1), mDynamicRange(false), mSaveRawData(false)
{

}

Histogram::~Histogram()
{
}

void Histogram::resizeDataVector(double newminval, double newmaxval, double oldminval, double oldmaxval)
{
    std::vector<std::pair<double, long long int>> mData2(getNumElements(newminval, newmaxval),std::pair<double, long long int>(0, 0));
    //std::cout << "mData2.size() = " << mData2.size() << std::endl;
    //std::cout << "mData.size() " << mData.size() << std::endl;
    for (int i=0; i<mData.size(); i++) {
        double olddata=indexToReal(i);
        int newindex=realToIndex(olddata, newminval, newmaxval);
        //std::cout << i << " " << olddata << " " << newindex << std::endl;
        mData2[newindex] = mData[i];
    }
    //std::cout << "before assignment" << std::endl;
    mData.resize(mData2.size(), std::pair<long long int, long long int>(0, 0));
    for (int i=0; i<mData.size(); i++) {
        mData[i]=mData2[i];
    }
    //mData = mData2;
    //std::cout << "after assignment" << std::endl;
    mMinVal = newminval;
    mMaxVal = newmaxval;
}

int Histogram::getNumElements() const {
    return getNumElements(mMinVal, mMaxVal);
}
int Histogram::getNumElements(double minval, double maxval) const {
    return static_cast<int>(1 + (maxval-minval)/mResolution);
}

int Histogram::realToIndex(double x) const {
    return realToIndex(x, mMinVal, mMaxVal);
}
int Histogram::realToIndex(double x, double minval, double maxval) const {
    //if (x < minval) throw std::string("Error! realToIndex but for vlaue smaller than minval");
    //if (x > maxval) throw std::string ("ErRROR ! data ToIndex but for value greater than maxval");
    return static_cast<int>( (x-minval)/mResolution );
}


double Histogram::indexToReal(int a) const {
    //if (a < 0 || a >= mData.size()) throw std::string("ERROR! indexToDAta but idnex out of bouinds");
    return mMinVal + a*mResolution;
}

void Histogram::initialize() {
    if (mMaxVal < mMinVal) mMaxVal = mMinVal;
    //std::cout << &mData << std::endl;
    //std::cout << mMinVal << " " << mMaxVal << " " << getNumElements() << std::endl;
    mData.resize(getNumElements(),std::pair<double, long long int>(0,0));
    //std::cout << &mData << std::endl;
}

void Histogram::addToInternalIndex(int index, double v, int weight) {
    if (mData.empty()) initialize();
    if (index >=0 && index < mData.size()) {
        mData[index].first+=v;
        mData[index].second+=weight;
    }
}

void Histogram::add(double x, double v, int weight)
{
    if (mData.empty()) initialize();
    int index=realToIndex(x);
    if (mDynamicRange && index < 0) {
        resizeDataVector(x, mMaxVal, mMinVal, mMaxVal);
        index=realToIndex(x);
    }
    if (mDynamicRange && index >= mData.size()) {
        resizeDataVector(mMinVal, x, mMinVal, mMaxVal);
        index=realToIndex(x);
    }
    if (index >=0 && index < mData.size()) {
        mData[index].first+=v;
        mData[index].second+=weight;
        if (mSaveRawData) {
            mRawData.push_back(x);
        }
    }
}

double Histogram::sum() const {
    //return std::accumulate(mData.begin(), mData.end(), 0);
    double s=0;
    for (int i=0; i<mData.size(); i++) {
        s+=mData[i].first;        
    }
    return s;
}
long long int Histogram::sumCount() const {
    //return std::accumulate(mData.begin(), mData.end(), 0);
    long long int s=0;
    for (int i=0; i<mData.size(); i++) {
        s+=mData[i].second;        
    }
    return s;
}

double Histogram::weightedAverage() const {
    double a=0, s=0;
    for (int i=0; i<mData.size(); i++) {
        s+=mData[i].first;
        a+=indexToReal(i)*mData[i].first;
    }
    if (s==0) return 0;
    else return a/s;
}
