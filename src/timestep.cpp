#include "timestep.h"

Timestep::Timestep() {
  number=0; cellx=0; celly=0; cellz=0; totaltimesteps=0; iteration=0;
  energy=0;
  sumcellx=0; sumcelly=0; sumcellz=0;
  numatoms=0;
  energy_weight=0;
}

