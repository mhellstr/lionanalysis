#include <cmath>

#ifndef TIMESTEP_H
#define TIMESTEP_H
class Timestep {
public:
  Timestep();
  float momMagnitude() { return std::sqrt(momx*momx+momy*momy+momz*momz); }
  int number;
  float cellx, celly, cellz, sumcellx, sumcelly, sumcellz, energy, momx, momy, momz;
  int totaltimesteps;
  int iteration;
  int numatoms;
  float energy_weight;
};
#endif

