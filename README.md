# lionanalysis
Reactive molecular dynamics trajectory analysis.

*lionanalysis* is short for *LAMMPS ion analysis*, but it can also be used to analyze trajectories from other programs and be applied to a variety of structures (not just ions).

![logo](logo.jpg)

## Installation

```
cd src
# edit Makefile if necessary
make
```

This should create an executable called `lionanalysis.exe`. Put this executable somewhere on your PATH.

Check your installation by running
```
lionanalysis.exe --help
```

## Usage, examples, documentation

Check the examples/ folder for some self-contained pedagogical examples!

Documentation at [lionanalysis.readthedocs.io](https://lionanalysis.readthedocs.io)

## Papers using lionanalysis

- [Concentration-Dependent Proton Transfer Mechanisms in Aqueous NaOH Solutions: From Acceptor-Driven to Donor-Driven and Back](https://doi.org/10.1021/acs.jpclett.6b01448)
- [Proton-Transfer Mechanisms at the Water–ZnO Interface: The Role of Presolvation](https://doi.org/10.1021/acs.jpclett.7b00358)
- [Proton-Transfer-Driven Water Exchange Mechanism in the Na+ Solvation Shell](https://doi.org/10.1021/acs.jpcb.7b01490)
- [Structure of aqueous NaOH solutions: insights from neural-network-based molecular dynamics simulations](https://doi.org/10.1039/c6cp06547c)
- [Maximally resolved anharmonic OH vibrational spectrum of the water/ZnO(101¯0) interface from a high-dimensional neural network potential](https://doi.org/10.1063/1.5012980)
- [Nuclear Quantum Effects in Sodium Hydroxide Solutions from Neural Network Molecular Dynamics Simulations](https://doi.org/10.1021/acs.jpcc.8b10781)
- [Structure and Dynamics of the Liquid-Water/Zinc-Oxide Interface from Machine Learning Potential Simulations](https://doi.org/10.1021/acs.jpcc.8b10781)
- [One-dimensional vs. two-dimensional proton transport processes at solid–liquid zinc-oxide–water interfaces](https://doi.org/10.1039/c8sc03033b)
- [Temperature effects on the ionic conductivity in concentrated alkaline electrolyte solutions](https://doi.org/10.1039/c9cp06479f)
- [Importance of Nuclear Quantum Effects on Aqueous Electrolyte Transport under Confinement in Ti3C2 MXenes](https://doi.org/10.1021/acs.jctc.2c00771)


## External packages, dependencies

The packages 

- [TCLAP](https://tclap.sourceforge.net) (MIT License), and
- [dlib](http://dlib.net) (Boost License) 

are included with `lionanalysis` in the `src/include` directory.


