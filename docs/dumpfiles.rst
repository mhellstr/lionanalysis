.. _DumpFileFormats:

Trajectory and structure file formats
=======================================

Frames can have different number of atoms and lattice parameters – no
problem.

.. important::

    **lionanalysis only supports orthorhombic cells**. Some file formats
    require you to specify the full lattice. Off-diagonal elements are either
    ignored or nonzero values will give an error.

    *lionanalysis* always assumes periodic boundary conditions. For non-periodic
    systems, specify large lattice vectors.

.. contents::

XYZ files
---------

mattixyz
~~~~~~~~

The comment line should read: "XYZ cellx celly cellz", where it is
assumed that the cell is orthorhombic with lattice parameters cellx
celly and cellz (for the periodic boundary conditions). For nonperiodic
systems, set these numbers to large values.

::

   3
   XYZ 10 10 10
   O 0 0 0
   H 1 1 1
   H -1 1 1
   3
   XYZ 12 13 14
   O 0.1 0.1 0.1
   H 1 -1 1
   H -1 1 1

Read this format with *lionanalysis*:

::

    DumpFileFormat mattixyz

Write this format with *lionanalysis*:

::

    PrintGroup All XYZ my_xyz_file.xyz

asexyz
~~~~~~~~~~~~

.. important::

    All components of the lattice vectors are specified, but all off-diagonal components must be 0! *lionanalysis* does not support non-orthorhobic cells.

::

    3
    Lattice="10.0 0.0 0.0 0.0 10.0 0.0 0.0 0.0 10.0"
    O 0 0 0
    H 1 1 1
    H -1 1 1

Read this format with *lionanalysis*:

::

    DumpFileFormat asexyz

Write this format with *lionanalysis*:

::

    PrintGroup All ASEXYZ my_xyz_file.xyz

ceriottixyz
~~~~~~~~~~~

As output from i-PI, with a lattice line in angstrom, atomic coordinates in angstroms 

::

    7
    # CELL(abcABC):   13.38377    13.38377    13.38377    90.00000    90.00000    90.00000  Step:           4  Bead:       1 positions{angstrom}  cell{angstrom}
           O  5.40520e-01 -9.74800e-01 -2.16618e-01
           O  2.92479e+00 -8.39306e-01  1.52342e-01
           H  1.85094e-01 -1.25718e+00 -1.07674e+00
           H  1.74325e+00 -9.09271e-01 -1.00174e-01
           H  3.29578e+00 -1.59156e+00  6.45803e-01
           H  3.52853e+00 -6.08880e-01 -5.74492e-01
           H  4.39795e-02 -1.96614e-01  9.14013e-02

Read this format with *lionanalysis*:

::

    DumpFileFormat ceriottixyz


ceriotticellbohrxyz
~~~~~~~~~~~~~~~~~~~

As output from i-PI, with a lattice line in bohr (atomic units), atomic coordinates in angstrom

LAMMPS "custom" output
----------------------

In the LAMMPS input file, make SURE you have set "dump_modify sort id",
so that the atoms are printed in ascending order with respect to their
"id".


lammpsrdx
~~~~~~~~~

LAMMPS input:

::

   dump 1 all custom 200 dump.cfg id type x y z q
   dump_modify 1 sort id

Trajectory file:

::

   ITEM: TIMESTEP
   1000
   ITEM: NUMBER OF ATOMS
   2
   ITEM: BOX BOUNDS pp pp pp
   0.0 11.0
   0.0 12.0
   0.0 13.0
   ITEM: ATOMS id type x y z q
   1 1 0 0 0 -1.0
   2 2 1 1 1 1.0


Read this format with *lionanalysis*:

::

    DumpFileFormat lammpsrdx

Write this format with *lionanalysis*:

::

    PrintGroup All LAMMPSRDX output.cfg




lammpsrdxnoq
~~~~~~~~~~~~

LAMMPS input:

::

   dump 1 all custom 200 dump.cfg id type x y z
   dump_modify 1 sort id

Trajectory file looks like this:

::

   ITEM: TIMESTEP
   1000
   ITEM: NUMBER OF ATOMS
   2
   ITEM: BOX BOUNDS pp pp pp
   0.0 11.0
   0.0 12.0
   0.0 13.0
   ITEM: ATOMS id type x y z 
   1 1 0 0 0
   2 2 1 1 1

Read this format with *lionanalysis*:

::

    DumpFileFormat lammpsrdxnoq


lammpsrdxvelnoq
~~~~~~~~~~~~~~~

id type x y z vx vy vz


RuNNer files
------------

runnerdata
~~~~~~~~~~

The program can read the RuNNer input.data format. Only orthogonal
lattices can be read correctly. 

Example:

::

    begin
    lattice 20.787 0.0 0.0
    lattice 0.0 22.6767 0.0
    lattice 0.0 0.0 24.5664
    atom 0 0 0 H 0 0 0 0 0
    atom 1.88973 1.88973 1.88973 He 0 0 0 0 0
    energy 0.000000
    end


Read this format with *lionanalysis*:

::

    DumpFileFormat runnerdata

Write this format with *lionanalysis*:

::

    PrintGroup All RUNNERDATA runner_input.data

