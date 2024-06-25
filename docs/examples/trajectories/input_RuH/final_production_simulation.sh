#!/bin/sh
# requires AMS2024.102 or greater
$AMSBIN/ams <<EOF
MolecularDynamics
  addmolecules
    deviationangle 0.0
    fractionalcoordsbox 0.0 1.0 0.0 1.0 0.7977159756511886 0.8077159756511886
    frequency 1000
    startstep 100
    system projectile
    velocity 0.03
    velocitydirection 0.0 0.0 -1.0
  End
  initialvelocities
    temperature 100.0
  End
  nsteps 100000
  removemolecules
    formula *
    frequency 1
    sinkbox
      fractionalcoordsbox 0.0 1.0 0.0 1.0 0.9 0.99
    End
  End
  thermostat
    region thermostatted
    tau 100.0
    temperature 300.0
    type NHC
  End
  thermostat
    region very_cold
    tau 10.0
    temperature 2.0
    type NHC
  End
  trajectory
    samplingfreq 100
  End
End

Task MolecularDynamics

system
  Atoms
             Ru       1.3524198212       2.1367593838       7.4657396348 region=thermostatted
             Ru       2.7048396424       0.0000000000       6.6849196870 region=thermostatted
             Ru       0.0000000000       2.1367593838       5.1232797913 region=thermostatted
             Ru       1.3524198212       0.0000000000       4.3424598435 region=thermostatted
             Ru       1.3524198212       2.1367593838       2.7808199478 region=thermostatted
             Ru       2.7048396424       0.0000000000       2.0000000000 region=very_cold
             Ru       1.3524198212       6.4102781513       7.4657396348 region=thermostatted
             Ru       2.7048396424       4.2735187675       6.6849196870 region=thermostatted
             Ru       0.0000000000       6.4102781513       5.1232797913 region=thermostatted
             Ru       1.3524198212       4.2735187675       4.3424598435 region=thermostatted
             Ru       1.3524198212       6.4102781513       2.7808199478 region=thermostatted
             Ru       2.7048396424       4.2735187675       2.0000000000 region=very_cold
             Ru       4.0572594636       2.1367593838       7.4657396348 region=thermostatted
             Ru       5.4096792848       0.0000000000       6.6849196870 region=thermostatted
             Ru       2.7048396424       2.1367593838       5.1232797913 region=thermostatted
             Ru       4.0572594636       0.0000000000       4.3424598435 region=thermostatted
             Ru       4.0572594636       2.1367593838       2.7808199478 region=thermostatted
             Ru       5.4096792848       0.0000000000       2.0000000000 region=very_cold
             Ru       4.0572594636       6.4102781513       7.4657396348 region=thermostatted
             Ru       5.4096792848       4.2735187675       6.6849196870 region=thermostatted
             Ru       2.7048396424       6.4102781513       5.1232797913 region=thermostatted
             Ru       4.0572594636       4.2735187675       4.3424598435 region=thermostatted
             Ru       4.0572594636       6.4102781513       2.7808199478 region=thermostatted
             Ru       5.4096792848       4.2735187675       2.0000000000 region=very_cold
             Ru       6.7620991060       2.1367593838       7.4657396348 region=thermostatted
             Ru       8.1145189272       0.0000000000       6.6849196870 region=thermostatted
             Ru       5.4096792848       2.1367593838       5.1232797913 region=thermostatted
             Ru       6.7620991060       0.0000000000       4.3424598435 region=thermostatted
             Ru       6.7620991061       2.1367593838       2.7808199478 region=thermostatted
             Ru       8.1145189272       0.0000000000       2.0000000000 region=very_cold
             Ru       6.7620991060       6.4102781513       7.4657396348 region=thermostatted
             Ru       8.1145189272       4.2735187675       6.6849196870 region=thermostatted
             Ru       5.4096792848       6.4102781513       5.1232797913 region=thermostatted
             Ru       6.7620991060       4.2735187675       4.3424598435 region=thermostatted
             Ru       6.7620991061       6.4102781513       2.7808199478 region=thermostatted
             Ru       8.1145189272       4.2735187675       2.0000000000 region=very_cold
  End
  Lattice
         8.1145189272     0.0000000000     0.0000000000
         0.0000000000     8.5470375350     0.0000000000
         0.0000000000     0.0000000000    15.0000000000
  End
End
system projectile
  Atoms
              H       0.0000000000       0.0000000000       0.0000000000 region=hydrogen
  End
End

Engine MLPotential
  Backend M3GNet
  MLDistanceUnit angstrom
  MLEnergyUnit eV
  Model Custom
  ParameterDir $AMSHOME/atomicdata/MLPotentials/M3GNet/RuH 
EndEngine

EOF
