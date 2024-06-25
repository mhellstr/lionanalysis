.. _about:

=================================
About *lionanalysis*
=================================

:Author: Matti Hellström

This is a nice tool that performs statistical analyses of sets of frames
with atomic coordinates. It is particularly good at extracting atoms
with some special environment. When the frames are trajectories from
molecular dynamics simulations, various time correlation functions can
be calculated.

Quickstart
---------------

See the :ref:`Examples`.

Features
----------

Extract atoms with given atomic environments, and do the below analysis *only* for the atoms that fulfill your criteria:

* Visualize interesting atomic environments
* Radial distribution functions [NaOH-Structure]_
* Angular distribution functions [NaOH-Structure]_
* Number density along axis [ZnO-Structure]_
* Histograms of coordination numbers [NaOH-Structure]_
* Bond life time (hydrogen bonds, metal-ligand bonds, ...) [NaOH-Exchange]_
* Species life time (hydroxide ion lifetime, ...)
   * Stable states picture [NaOH-NQE]_
   * Intermittent time correlation function
   * Continuous time correlation function
* Proton transfer free energy landscape [NaOH-PT]_ [ZnO-PT]_ [ZnO-PT2]_
* Mean squared displacement, diffusion coefficient [NaOH-NQE]_ [ZnO-PT2]_
   * Follow multiple Grotthuss-diffusing hydroxide/hydronium ions!
* Velocity autocorrelation function, power spectrum [NaOH-NQE]_ [ZnO-Vib]_
* Determine coordination polyhedra [NaOH-Structure]_



Who should use lionanalysis?
-----------------------------

If 

* you run simulations that are more "atomistic" than "molecular", and
* the system size is a few thousand atoms or so,

then lionanalysis is for you!

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


Compilation
----------------------

This should be as easy as::

    cd src && make

See the Makefile for details.

Running the program
----------------------

.. code-block::

    lionanalysis.exe name-of-input-file

There are various flags, run ``lionanalysis.exe --help`` to see them. Options specified on the
command line will typically override any options given in the input
file. The default name of the input file is "ionanalysis.config". See
also the standalone olblock.sh utility.


References
---------------

.. [NaOH-PT] Hellstrom, Behler. `Concentration-Dependent Proton Transfer Mechanisms in Aqueous NaOH Solutions: From Acceptor-Driven to Donor-Driven and Back. <https://doi.org/10.1021/acs.jpclett.6b01448>`__
.. [ZnO-PT] Quaranta, Hellstrom, Behler. `Proton-Transfer Mechanisms at the Water–ZnO Interface: The Role of Presolvation <https://doi.org/10.1021/acs.jpclett.7b00358>`__
.. [NaOH-Exchange] Hellstrom, Behler. `Proton-Transfer-Driven Water Exchange Mechanism in the Na+ Solvation Shell <https://doi.org/10.1021/acs.jpcb.7b01490>`__
.. [NaOH-Structure] Hellstrom, Behler. `Structure of aqueous NaOH solutions: insights from neural-network-based molecular dynamics simulations <https://doi.org/10.1039/c6cp06547c>`__
.. [ZnO-Vib] Quaranta, Hellstrom, Behler, Kullgren, Mitev, Hermansson. `Maximally resolved anharmonic OH vibrational spectrum of the water/ZnO(101¯0) interface from a high-dimensional neural network potential <https://doi.org/10.1063/1.5012980>`__
.. [NaOH-NQE] Hellstrom, Ceriotti, Behler. `Nuclear Quantum Effects in Sodium Hydroxide Solutions from Neural Network Molecular Dynamics Simulations <https://doi.org/10.1021/acs.jpcc.8b10781>`__
.. [ZnO-Structure] Quaranta, Behler, Hellstrom. `Structure and Dynamics of the Liquid-Water/Zinc-Oxide Interface from Machine Learning Potential Simulations <https://doi.org/10.1021/acs.jpcc.8b10781>`__
.. [ZnO-PT2] Hellstrom, Quaranta, Behler. `One-dimensional vs. two-dimensional proton transport processes at solid–liquid zinc-oxide–water interfaces <https://doi.org/10.1039/c8sc03033b>`__
