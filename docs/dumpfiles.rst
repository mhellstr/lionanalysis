.. _DumpFileFormats:

Dump file formats
=================

Frames can have different number of atoms and lattice parameters – no
problem.

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

ceriottixyz
~~~~~~~~~~~

As output from i-PI, with a lattice line in angstrom, atomic coordinates in angstroms 

ceriotticellbohrxyz
~~~~~~~~~~~~~~~~~~~

As output from i-PI, with a lattice line in bohr, atomic coordinates in angstrom

LAMMPS "custom" output
----------------------

In the LAMMPS input file, make SURE you have set "dump_modify sort id",
so that the atoms are printed in ascending order with respect to their
"id".

::

   dump 1 all custom 200 dump.cfg id type x y z
   dump_modify 1 sort id

lammpsrdx
~~~~~~~~~

id type x y z q

lammpsrdxnoq
~~~~~~~~~~~~

id type x y z

lammpsrdxvelnoq
~~~~~~~~~~~~~~~

id type x y z vx vy vz


RuNNer files
------------

runnerdata
~~~~~~~~~~

The program can read the RuNNer input.data format. Only orthogonal
lattices can be read correctly. Specify DumpFileFormat runnerdata

