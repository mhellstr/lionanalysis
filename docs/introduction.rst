.. _about:

=================================
About *lionanalysis*
=================================

:Author: Matti Hellström

*lionanalysis* is a command-line program that analyses frames with atomic
coordinates. It is particularly good at extracting atoms with some special
environment. When the frames are trajectories from molecular dynamics
simulations, various time correlation functions can be calculated.

It reads a text input file and produces text output files.

It is possible to read/plot the results files in gnuplot or using Python with
numpy and pandas.  See the :ref:`Examples`.


Quickstart
---------------

* `Download the source code <https://github.com/mhellstr/lionanalysis>`__ from GitHub
* :ref:`Compile the source code <Compilation>`
* See the :ref:`Examples`. The original Jupyter notebooks (and required ``common.py`` file) can be found in ``docs/examples`` in the source tree.

Features
----------

Extract atoms with given atomic environments, and do the below analysis *only* for the atoms that fulfill your criteria:

* Visualize interesting atomic environments
* Radial distribution functions 
* Angular distribution functions
* Number density along axis
* Histograms of coordination numbers
* Bond life time (hydrogen bonds, metal-ligand bonds, ...)
* Species life time (hydroxide ion lifetime, ...)
   * Stable states picture
   * Intermittent time correlation function
   * Continuous time correlation function
* Proton transfer free energy landscape
* Mean squared displacement, diffusion coefficient
   * Follow multiple Grotthuss-diffusing hydroxide/hydronium ions!
* Velocity autocorrelation function, power spectrum
* Determine coordination polyhedra 



Who should use lionanalysis?
-----------------------------

If you run simulations that are more "atomistic" than "molecular", and the
system size is no more than a few thousand atoms or so, then lionanalysis is
for you!

Lionanalysis is designed around atomic environments. It is **easy** to detect that
a C atom is bonded to 1 H and 2 C, and that those carbons are themselves
also bonded to 1 H and 2 C, but it's **very difficult** to set up lionanalysis to
determine if a C atom is part of a benzene molecule.

If you already know which atoms are part of benzene molecules, you can set up
lionanalysis to do analysis on those. But they will be treated as atoms, not as
molecules. For example, when calculating mean squared displacement, you do so
on the atomic positions and not on the center of mass of each molecule.


Current limitations
----------------------

* Only orthorhombic simulation boxes are supported
* Mean squared displacement of atoms, not center-of-mass-of-molecule


.. _Compilation:

Compilation
----------------------

You need a C++11 compiler. Compilation should be as easy as::

    cd src && make

See the Makefile for details.

*lionanalysis* has only been tested on Linux, but should in principle also work on other platforms.


Running the program
----------------------

Copy the ``lionanalysis.exe`` executable created by the compilation to somewhere on your PATH.

Check your installation:

.. code-block::

    lionanlaysis.exe --help

To run an input file:

.. code-block::

    lionanalysis.exe name-of-input-file

There are various flags, run ``lionanalysis.exe --help`` to see them. Options
specified on the command line will typically override any options given in the
input file. The default name of the input file is "ionanalysis.config". See
also the standalone olblock.sh utility.


References
---------------

* Hellstrom, Behler. `Concentration-Dependent Proton Transfer Mechanisms in Aqueous NaOH Solutions: From Acceptor-Driven to Donor-Driven and Back. <https://doi.org/10.1021/acs.jpclett.6b01448>`__
* Quaranta, Hellstrom, Behler. `Proton-Transfer Mechanisms at the Water–ZnO Interface: The Role of Presolvation <https://doi.org/10.1021/acs.jpclett.7b00358>`__
* Hellstrom, Behler. `Proton-Transfer-Driven Water Exchange Mechanism in the Na+ Solvation Shell <https://doi.org/10.1021/acs.jpcb.7b01490>`__
* Hellstrom, Behler. `Structure of aqueous NaOH solutions: insights from neural-network-based molecular dynamics simulations <https://doi.org/10.1039/c6cp06547c>`__
* Quaranta, Hellstrom, Behler, Kullgren, Mitev, Hermansson. `Maximally resolved anharmonic OH vibrational spectrum of the water/ZnO(101¯0) interface from a high-dimensional neural network potential <https://doi.org/10.1063/1.5012980>`__
* Hellstrom, Ceriotti, Behler. `Nuclear Quantum Effects in Sodium Hydroxide Solutions from Neural Network Molecular Dynamics Simulations <https://doi.org/10.1021/acs.jpcc.8b10781>`__
* Quaranta, Behler, Hellstrom. `Structure and Dynamics of the Liquid-Water/Zinc-Oxide Interface from Machine Learning Potential Simulations <https://doi.org/10.1021/acs.jpcc.8b10781>`__
* Hellstrom, Quaranta, Behler. `One-dimensional vs. two-dimensional proton transport processes at solid–liquid zinc-oxide–water interfaces <https://doi.org/10.1039/c8sc03033b>`__
