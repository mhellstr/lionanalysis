.. _Input:

=================================
Input
=================================

Commands are read and executed sequentially. Some commands are options
(e.g. MaxTimestep), others are two-pronged, and (might) execute
something every SuperEvery frames, and then (might) execute something
else at the end of the program. For example,

-  DefineGroup does something every SuperEvery steps, but nothing at the
   end.

-  FinalShell does nothing every SuperEvery steps, but something at the
   end.

-  RDF does something every SuperEvery steps (collects data), and
   something at the end (prints results).

All commands are case-insensitive. Lines beginning with a hash (#) are
treated as comment lines. Lines ending with a backslash (\\) are joined
together with the next line. Words are separated by one or more spaces
or tab characters. When filenames and the like are specified, they
cannot contain any whitespace.

Certain commands requires filenames, but then you can specify /dev/null
as a filename to not write any output.

It is sometimes useful to use multiple Overwrite and DevNullIfFileExists
statements, if you for example temporarily want to turn off the output
to some certain file.

**Conventions for this documentation**:
The commands and most available options are written in title case (e.g.
ModifyGroup, GroupMustBeInCoordinationShellOf). Some options are for
historical reasons written in all-caps (e.g. PROPERTIES). All-lowercase
is used for words that the user should explicitly replace. For example,
"Filename filename" means, type a literal "Filename" followed by the
actual filename that you want to specify. Optional options are given
within square brackets. Default values are given after "=". You should
thus never actually write "=" signs when giving values for options.

AngleRDF
~~~~~~~~

::

     AngleRDF
     {[Group1 groupname1] [Group1DummyVector dx dy dz]
     [Group1CoordinationShellCenterOfMass]}
     Group2 groupname2
     {[Group3 groupname3] [Group3DummyVector dx dy dz]
     [Group3CoordinationShellCenterOfMass]}
     [Filename prefix_filename=/dev/null] [AngleResolution X=1]
     [DistanceResolution X=0.1]
     [MinAngle X] [MaxAngle X] [MaxDist12 X] [MinDist12 X]
     [MaxDist23 X] [MinDist23 X] [MaxDist13 X] [MinDist13 X]
     [MaxDist13Frac12 X] [MinDist13Frac12 X]
     [MaxDist23Frac12 X] [MinDist23Frac12 X]
     [MaxDist13Frac23 X] [MinDist13Frac23 X]
     [AngleXY] [AngleXZ] [AngleYZ] [AngleZX]
     [SignedAngle]
     [NewGroup1 groupname] [NewGroup2 groupname] [NewGroup3 groupname]
     [CopyGroup1 groupname] [CopyGroup2 groupname] [CopyGroup3 groupname]
     [MinHits12 X]
     [GroupMustBeInCoordinationShellOf {1,2,3} {1,2,3}]
     [NewGroup1CoordinationGroup {2,3}] [NewGroup2CoordinationGroup {1,3}]
     [NewGroup3CoordinationGroup {1,2}]

The AngleRDF command is not self-explanatory. An angle is between three
atoms.

There are three groups: group1, group2, and group3. All angles from
atoms in group1, group2, and group3 that obey the distance criterions
will contribute to the RDF. group2 is the group of the center atom. The
distance criterions are there since one wants to measure angles between
atoms that are quite close. Since it doesn’t make much sense to measure
the angle between an oxygen atom coordinated toa zinc atom, the
coordinated zinc atom, and an oxygen coordinated to ANOTHER zinc atom
somewhere else in the system, the distance criterions are there. (the
groups can thus be group1=group3={oxygen atoms bound to zn}, group2={zn
atoms}, even if there are several zn atoms)

So the program loops through the atoms in group2, for each such atom it
also loops through the groups group1 and group3 and finds all triads
that are "together" according to the distance criterions. The angle is
calculated and added to the RDF.

the MinDist?? and MaxDist?? options are self-explanatory

the MinDist13Frac12 option gives the minimum distance between atoms in
groups 1 3 as a fraction of the de facto distance between atoms in
groups 1 and 2.

**Special group1 and group3:** You can choose to speciy either Group3 or
Group3DummyVector or Group3CoordinationShellCenterOfMass:

Group3DummyVector: group3 will contain dummy atoms that have positions
(group2atom+dx, group2atom+dy, group2atom+dz) for each atom in group2.
NOTE that the distance criterions still must be fulfilled!

Group3CoordinationShellCenterOfMass: group3 will contain dummy atoms
that have positions (average position of an atom in group2 together with
its coordination shell), for each atom in group2. NOTE that the distance
criterions still must be fulfilled!

Group1/Group1DummyVector/Group1CoordinationShellCenterOfMass are
equivalent.

**Special angle:** AngleXY projects coordinates onto xy plane (i.e. sets
atom z coordinates to 0) before calculating angle. But the distance
between the atoms in Group1 and Group2 is NOT projected.

SignedAngle only works in combiantion with Angle??, i.e. in two
dimensions; this returns an angle in the interval -180 to 180 degrees.

GroupMustBeInCoordinationShellOf X Y means that Group X must be in the
coordination shell of group Y. This is useful if you’ve made a lot of
work defining e.g. group 3 to be those atoms in the coordination shell
of atoms in group 2, since it allows you to be more careless with the
distance criteria.

**Create new groups:** NewGroup1 creates a new group with all the atoms
in group1 that were part of at least one triad fulfilling the distance
and angle criterions. If MinHits12 is specified, the atoms in group1 and
group2 must together be part of at least X triads before NewGroup1 is
updated. In the new group, coordination tables are saved. By default,
both the atoms in group2 and group3 that are coordinated will be added
to the coordination tables. If NewGroup1CoordinationGroup is set to 2,
only atoms in group2 are added to the coordination table.

CopyGroup1 will copy ALL members in groupname1 into the new group, but
contain the coordination numbers and coordination tables that result
from the angle and distance criterions. CopyGroup would be like
specifying "MinCoord 0" in a BOND group. Only ONE of NewGroup1 and
CopyGroup1 can be specified. (both can be omitted)

An alternative to NewGroup1 would be to specify CopyGroup1, and then do
DefineGroup newgroup SUBGROUPCN copygroup MinCoord 1 — this should copy
both the coordination number and the coordination table

| **OUTPUT:** Although only one filename is specified, the AngleRDF
  command actually gives EIGHT output files, named filename_1,
  filename_2, ... , filename_8. You do not only get the angular RDF, but
  as a bonus you get the angular RDF as a function of the bond lengths,
  and also the bond lengths as a function of each other.
| filename_1: bondlength(group1-group2), angle, weight
| filename_2: bondlength(group2-group3), angle, weight
| filename_3: bondlength(group1-group2), bondlength(group2-group3),
  weight
| filename_4: bondlength(group1-group2), weight (same as filename_1 but
  summed over all angles)
| filename_5: bondlength(group2-group3), weight (....... filename_2
  ..........................)
| filename_6: angle, weight (same as filename_1/2 but summed over all
  bond lengths)
| filename_7: bondlength(group1-group3), angle, weight
| filename_8: bondlength(group1-group3), weight (same as filename_7 but
  summed over all angles)

The file you most often want to look at is thus filename_6. There is no
normalization with respect to sin(theta) - if you want to do this you
need to do it manually.

The AngleRDF command is currently the only way to creat new groups that
have angle conditions.

AverageSize
~~~~~~~~~~~

::

    AverageSize [options]

Alias for FinalPrintProperties averagesize.dat MultipleLines PrintEvery
100 [options] ALLGROUPS PROPERTIES groupname averagesize

BasicTimeUnit
~~~~~~~~~~~~~

::

    BasicTimeUnit X=1

Give the time between consecutive frames in the dump file in
picoseconds. If this is specified, you can use RealTime options for some
other commands to let you specify time in picoseconds rather than number
of frames.

Break
~~~~~

::

    Break

Stop reading input file, start the analysis!

CalculateVelocities
~~~~~~~~~~~~~~~~~~~

.. note::

    It is much preferred to just read the velocities from the dump file, if possible.

::
    
    CalculateVelocities


Calculate atomic velocities from atomic positions in adjacent frames. Only
useful if the trajectory contains every time step of the MD simulation, or the
velocities will be extremely inaccurate.

Can be used to calculate velocity autocorrelation functions in case you forgot
to write out the velocities to the dump file.


ChangeGroupTime
~~~~~~~~~~~~~~~

.. warning::
    
    Expert input option.

::

    ChangeGroupTime Reactants X Products Y Filename X [ParentGroup X=All]
    [ShellGroup X=All] [CheckCoordinationTable] [Every X=1] [MinReactantTime
    X=1(timestepunits)] [MaxReactionTime X=100000000] [Action X] [Action Y]
    [Action ...]

Print information for when a member in the reactants (that has been a
member of reactants at least MinReactantTime timesteps) turns into the
product.

The output is: atom id, first entered reactants, last entered reactants,
last stayed as reactant, first entered product.

You can specify Actions (of type WhenGroupChanged or
WhenGroupChangedDefineGroup; specify the action id) that will be called
whenever a reactant becomes a product.

CheckCoordinationTable does what it says. If Reactants are Na-O (O in
CT), and Products are Na...O (O in CT), then you will trigger the
Actions when an O bound to a *specific* Na becomes unbound to the *same*
Na. The atom that gets fed into the Action is the atom in the CT, i.e.,
the O in this case. Example:

::

   DefineGroup Na_O BOND Na O MaxDist 3
   DefineGroup Na...O BOND Na O MinDist 4.5 MaxDist 100
   ChangeGroupTime Reactants Na_O Products Na...O CheckCoordinationTable 

CheckOverlap
~~~~~~~~~~~~

Const
~~~~~

::

    Const name value

Create a (floating-point) constant with a given value.


::

    Const C_MyMaxDistance 3.0
    DefineGroup A BOND B C MaxDist C_MyMaxDistance

CoutFrequency
~~~~~~~~~~~~~

::

    CoutFrequency [x=1]

How frequently to print timing information to the screen.

DefineGroup
~~~~~~~~~~~

A group called ``All`` is automatically defined and it contains all atoms.
Groups can also be created using AngleRDF (or DoubleCoordinationShortDelta).
Group members of Static groups are not updated when new frames are read (they
are defined *in the first frame* and then kept constant). Groups of type SUM,
DIFF, and INTERSECTION automatically become static if and only if all their
constituent groups are static.

See also: ModifyGroup.

DefineGroup ADDCOORDINATION
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

::

    DefineGroup groupname ADDCOORDINATION group1 group2

If an atom in the first group's coordination table is a member of the second
group, add the corresponding atom's coordination tbale in the SECOND group to
the coordination tbale of the ORIGINAL atom in the FIRST group.

``groupname`` thus contains the same atoms as ``group1``. Only the coordination
table is different.

DefineGroup ATOMICNUMBER
~~~~~~~~~~~~~~~~~~~~~~~~

::

    DefineGroup groupname ATOMICNUMBER atomicnumber [Static]

Corresponds to the first column in XYZ files. It’s called "ATOMICNUMBER"
but it’s really a string.

DefineGroup BOND
~~~~~~~~~~~~~~~~

::

    DefineGroup groupname BOND fromgroup togroup 
    [Corresponding correspondinggroup] [IncludeToGroup] 
    [MinDist X=0.0] [MaxDist X=10.0]
    [Coord X] [MinCoord X=1] [MaxCoord X=10000] [Static]

Atoms from fromgroup are added to group groupname, provided they fulfill
the bond criterions to group togroup. Coord X is shorthand for MinCoord
X MaxCoord X, and specifies the desired coordination number. The group
"CorrespondingGroup" can be created, which consists of all the atoms in
the group togroup that fulfilled the distance criterions to the final
atoms in group "groupname". Coordination tables are automatically saved
for the atoms in groupname. IncludeToGroup will include the atoms in the
group togroup that fulfill the criterions (i.e. the corresponding group)
in the group groupname.

DefineGroup COORDINATIONTABLEINTERSECTION
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

::

    DefineGroup groupname COORDINATIONTABLEINTERSECTION group1 [group2
    group3...]

groupname becomes the atoms that exist in group1; only the coordination
tables are changed, so that the coordinated atoms in group2, group3...
are kept.

See also DefineGroup ADDCOORDINATION.

DefineGroup DIFF
~~~~~~~~~~~~~~~~

::

    DefineGroup groupname DIFF group1 [group2 group3...]

groupname becomes all elements of group1 that cannot be found in either
group2, group3, ... If group1 has a coordination table, this
coordination table is copied over to the atoms in groupname

DefineGroup FINDSHORTEST
~~~~~~~~~~~~~~~~~~~~~~~~

::

    DefineGroup groupname FINDSHORTEST FromGroup X ToGroup Y

for each X, find the nearest atom in group Y. The atoms in groupname
becomes the atoms in group \*Y*, with coordination tables to group \*X\*

::

   DefineGroup O_H FINDSHORTEST FromGroup H ToGroup O
   DefineGroup WaterO SUBGROUPCN O_H Coord 2
   DefineGroup HydroxideO SUBGROUPCN O_H Coord 1
   DefineGroup OxideO DIFF O O_H

DefineGroup INCLUDECOORDINATION
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

::

    DefineGroup groupname INCLUDECOORDINATION parentgroup

parentgroup must have a coordination table. The members of groupname
becomes the atoms in the parent group AND the atoms in the ocordination
table of parentgroup. The purpose is that the atom and their
coordination table come together internally within the group, so that if
you PrintGroup you will get them e.g. in the order "O H H O H H O H H O
H H" no matter if they were sorted asi ntact water molecules in the
input trajectory file. Some other analysis programs require water
molecules to be intact like that.

DefineGroup INTERSECTION
~~~~~~~~~~~~~~~~~~~~~~~~

::

    DefineGroup groupname INTERSECTION group1 [group2 group3...]

groupname becomes the atoms that exist in all the groups group1, group2,
group3... 

**If group1 has a coordination table, this coordination table is
copied over to the atoms in groupname**

The intersection is thus only applied to the group members, NOT to the
coordination tables!

DefineGroup INVERTCOORDINATION
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

::

    DefineGroup groupname INVERTCOORDINATION parentgroup

parentgroup must have a coordination table. the members of groupname
becomes the atoms in the ocordination table of parentgroup, with
coordination tables being the atoms in parentgroup

DefineGroup LIST
~~~~~~~~~~~~~~~~

::

    DefineGroup groupname LIST id1 [id2] [id3] [...] [Static]

Give the atomic IDs (first atom has id 1, second id 2, etc.).

DefineGroup MEMBERHISTORY
~~~~~~~~~~~~~~~~~~~~~~~~~

::

    DefineGroup groupname MEMBERHISTORY ParentGroup X MemberOf X DrawFrom X
    MaxHistory X [MinMemberTime X] [MaxMemberTime X]

Select atoms from "drawfrom" that have been a member of "memberof" at
least minmembertime and at most maxmembertime in the past maxhistory
timesteps.

DefineGroup READ
~~~~~~~~~~~~~~~~

::

    DefineGroup groupname READ filename_with_atomic_ids_one_line_per_timestep

DefineGroup REGION
~~~~~~~~~~~~~~~~~~

::

    DefineGroup groupname REGION [MinX x=-inf] [MaxX x=inf] [MinY x=-inf]
    [MaxY x=inf] [MinZ x=-inf] [MaxZ x=inf] [Static]

All atoms in the coordinate interval specified (:math:`\ge` Min,
:math:`<` Max)

DefineGroup SUBGROUP
~~~~~~~~~~~~~~~~~~~~

::

    DefineGroup groupname SUBGROUP parentgroup id1 [id2 id3...]

Creates a subgroup. 

.. important:: 

    The "id1 id2..." are NOT the atomic
    ids, but rather refer to a the index in the parentgroup (starting at 0,
    similar to the CenterOn in the PrintGroup command). 

If you know the
"proper" atomic ids use LIST instead. If you specify an id that is
greater than the (number of atoms-1) in the group, then that is quietly
ignored.

DefineGroup SUBGROUPCN
~~~~~~~~~~~~~~~~~~~~~~

::

    DefineGroup groupname SUBGROUPCN parentgroup [MinCoord X] [MaxCoord X]
    [Coord X] [Static]

Extract atoms in group parentgroup (that should be of type BOND or
defined in AngleRDF) that have the requested coordination numbers

DefineGroup SUBGROUPRANDOM
~~~~~~~~~~~~~~~~~~~~~~~~~~

::

    DefineGroup groupname SUBGROUPRANDOM parentgroup [nummembers=1]

Extract nummembers random members from parentgroup.

DefineGroup SUM
~~~~~~~~~~~~~~~

::

    DefineGroup groupname SUM group1 [group2 group3...]

groupname becomes the union of group1, group2, group3... 

**If the constituent groups have coordination tables, the new group’s
coordination table will be the union of the other groups’ coordination tables
for the same atom**

Density
~~~~~~~

::

    Density Axis x/y/z Group groupname Filename filename Resolution X
    [MinValue X] [MaxValue X] [NoPeriodic]

Gets you the number density along an axis coordinate of the atoms in the
group groupname. The default is to translate the atomic coordinate to
the interval [0,celllength]. If you do not want this specify NoPeriodic

DevNullIfFileExists
~~~~~~~~~~~~~~~~~~~

::

    DevNullIfFileExists

Do not overwrite output file if they already exist, instead print to
/dev/null. Sometimes nice to combine with Overwrite.

DihedralRDF
~~~~~~~~~~~

::

    DihedralRDF group1 mindist1-2 maxdist1-2 group2 group3 mindist2-3
    maxdist2-3 group4 mindist3-4 maxdist3-4 filename [MinAngle X=0.0]
    [MaxAngle X=180.0] [AngleResolution X=1.0] [Every X=1]

Expert input option (not well-tested).

DoubleCoordinationShortDelta
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

::

    DoubleCoordinationShortDelta Filename filename
    LHS group1 group2 [LGroup3MustBe X] 
    [NewGroupL1 ng1] [NewGroupL2 ng2] [NewGroupL3 ng3] 
    [NewGroupL1CoordinationGroup {2,3}]
    [NewGroupL2CoordinationGroup {1,3}] 
    [NewGroupL3CoordinationGroup {1,2}]
    [RHS group1 group2] [RGroup3MustBe X] 
    [NewGroupR1 ng1] [NewGroupR2 ng2] [NewGroupR3 ng3] 
    [NewGroupR1CoordinationGroup {2,3}]
    [NewGroupR2CoordinationGroup {1,3}] 
    [NewGroupR3CoordinationGroup {1,2}]
    [Every X=1] [MinValue X=0] [MaxValue X=2] [Resolution X=0.01] 
    [Min13 X=2] [Max13 X=3.5] [Resolution13 X=0.1] [WellAtZero]

Calculates delta slightly differently from ShortDelta. This one does it
the right way! Although there is in practice very little difference to the end
results.

This action prints the free energy landscape for both LHS and RHS in the
same file, and prints the x coordinate in the center of the bins.

group1 and group2 must have coordination tables. For each atom in
group1, the command goes through the coordination table. If an atom in
the coordination table is also in group 2, it goes through the
coordination table in group 2. Then it computes delta between the three
atoms.

If you don’t specify RHS, then LHS and RHS are taken to be the same.

You get a bonus output file suffixed "2d", which gives the
two-dimensional free-energy landscape as a function of both
:math:`\delta_\textrm{min}` and the group1-group3 distance. You can
control the allowed values and resolution of the second coordinate by
specifying Min13, Max13, and Resolution13. The WellAtZero option puts
the deepest well at a free energy of 0 (i.e. all other numbers are
positive, which might be easier to work with sometimes).

NOTE that the Min13 and Max13 values also impact the one-dimensional
free-energy landscape, i.e. the 13 distance must within the range for it
to be counted.

For proton transfer reactions, you want group1 to be the ACCEPTING
species, with coordination table to ALL donating hydrogens – EVEN the
hydrogens that you’re not necessarily interested in. Group2 should be
ALL hydrogens with coordination tables to their covalently bound oxygen
("SuperH"). If you want to limit your study to only a certain kind of
donor, put in the {L,R}Group3MustBe keywords. Then, :math:`\delta` will
only be counted if the resulting atom in group3 (that is the
coordination table of group2) is the species that you want.

DumpFile
~~~~~~~~

::

    DumpFile dumpfile1 [dumpfile2] [dumpfile3] [...]

Specify files containing the information about the atoms. If more than
one dumpfile is specified, they are read sequentially. DumpFile in the
input file can be overridden by the ``--dumpfile`` option on the command
line.

See also FrameNumbersMustIncrease.

DumpFileFormat
~~~~~~~~~~~~~~

::

    DumpFileFormat dumpfileformat

Specify format of the dump (trajectory) file, see
section :ref:`DumpFileFormats` for the allowed values and structure of the
corresponding trajectory files.

FinalPrintProperties
~~~~~~~~~~~~~~~~~~~~

::

    FinalPrintProperties [PrintEvery X] [MultipleLines] [NewLineSeparator]
    [TabSeparator] [NumEntries] 
    GROUPS group1 [group2...] 
    PROPERTIES property1 [property2...]

See also PrintProperties.

FinalShell
~~~~~~~~~~

::

    FinalShell shellcommand

Performs the shell command after all analysis has been done.

FindVacuum
~~~~~~~~~~

.. warning::

    Use with caution, not well-tested.

::

    FindVacuum groupname filename [Every X=1] [Resolution X=0.5]

Find the greatest empty volume, not counting any vdW volumes or such of
the atoms. The "volume" here is the greatest distance from one point to
any of the atoms in groupname. Output is timestep->iteration, x, y, z,
distance. The resolution specified is supposed to be 10 x the actual
resolution. First a "rough" scan is performed for Resolution, then a
finer scan is performed with Resolution/10 around the point with the
largest vacuum found in the first scan.

FrameNumbersMustIncrease
~~~~~~~~~~~~~~~~~~~~~~~~

::

    FrameNumbersMustIncrease

If set, a frame must have a higher frame number than the previous for it
to be read/manipulated. This only works for file formats that write
frame numbers (e.g. lammpsrdx and ceriotticellbohrxyz). The option is
useful for analyzing continued MD trajectories. Often, the initial
frames in the continuation run will have frame numbers that overlap with
the last frames of the previous run. By setting this flag, the analysis
can be run on the dump files in sequence without needing to cut one of
the trajectories beforehand.

See also :ref:`DumpFileFormats`.

GlobalDistanceGroup
~~~~~~~~~~~~~~~~~~~

::

    GlobalDistanceGroup groupname

Only calculate distances (bonds) within group X. X cannot depend on any
distances, so should be of e.g. ATOMICNUMBER or LIST type

GlobalDistanceGroups
~~~~~~~~~~~~~~~~~~~~

::

    GlobalDistanceGroups X1 X2 [Y1 Y2] [Z1 Z2] [...]

Only calculate distances (bonds) between atoms of groups X1 and X2,
between Y1 and Y2, etc.

Histogram
~~~~~~~~~

::

    Histogram filename [Every X=1] [PrintEvery X] 
    [MinVal X] [MaxVal X] [Resolution X] [DynamicRange] 
    GROUPS group1 [group2 ...] 
    PROPERTIES property1 [property2...]

Either specify a fixed range with MinVal and MaxVal (exclude any values
outside this range), or use DynamicRange to include all possible values
of the property.

HydrogenBond
~~~~~~~~~~~~

::

    HydrogenBond group1 group2 group3 [options ...]

This is an alias for:

::

    AngleRDF Group1 group1 Group2 group2 Group3 group3 MaxDist12 3.5
    MaxDist23 1.8 GroupMustBeInCoordinationShellOf 3 2 MaxDist13Frac12 1
    MaxAngle 30 [options ...]

Don’t forget that group2 MUST have a coordination shell to atoms in
group3.

group1 is the accepting O, group2 is the “donating” O, and group3 is the
donating H.

IntelligentUnwrap
~~~~~~~~~~~~~~~~~

::

    IntelligentUnwrap

Calculate unwrapped coordinates. **MUST be set when calculating TMSD**
unless unwrapped coordinates are given in the dump file.

Loop
~~~~

::

    Loop [VARIABLE v1 v2 ...] [VARIABLE w1 w2....] [...] [COMMAND cmd1] [COMMAND cmd2] [...]

Loops through the variables v1, v2.... and w1, w2.... (taking each
possible combination of these values) and performs the given commands
for them. The commands should be written like "DefineGroup @1@2
INTERSECTION @1 @2", where "@1" will be replaced by one of the v1, v2...
and @2 will be replaced by one of the w1, w2...

Examples:

-  Loop VARIABLE group1 group2 group3 COMMAND PrintGroup @1 XYZ @1.xyz

-  Loop VARIABLE 0 1 2 3 4 5 COMMAND DefineGroup Coord@1 SUBGROUPCN
   myparentgroup Coord @1

-  Loop VARIABLE 0 1 2 3 VARIABLE 0 1 2 3 COMMAND DefineGroup AccDon@1@2
   INTERSECTION Acc@1 Don@2 COMMAND PrintGroup AccDon@1@2 XYZ
   accdon@1@2.xyz

When the program is run, it will expand the Loop command into the
individual commands that it actually performs (and tell you what it
does)

MaxNumAtoms
~~~~~~~~~~~

::

    MaxNumAtoms x=3000

MaxTimestep
~~~~~~~~~~~

::

    MaxTimestep x=inf

Stop reading the trajectory after x frames.

MinTimestep
~~~~~~~~~~~

::

    MinTimestep x=1

Start the analysis from the x'th frame.

MaxTotalSize
~~~~~~~~~~~~

::

    MaxTotalSize groupname maxsize

Give the maximum total size (summed over all timesteps) of group
groupname. The program quits as soon as the maximum size is exceeded.
Especially useful if you think there is a group that *should* be empty, but
at some time becomes populated. Set MaxTotalSize groupname 1 and the
program will exit as soon as the group is nonempty.

ModifyGroup
~~~~~~~~~~~

::

    ModifyGroup group GROUPTYPE ...

The syntax here is identical to DefineGroup. This doesn’t really modify
the original group, rather it changes the name of the original group and
then creates a new group with the same name. The original group cannot
be accessed by any commands after the ModifyGroup command, but it CAN be
accessed on the actual ModifyGroup command. For example:

::

    ModifyGroup groupwithplentyofatoms SUBGROUP groupwithplentyofatoms 0

This would shrink groupwithplentyofatoms to only the first atom in the
group. 

Commands that come **before** the ModifyGroup and that depend on
groupwithplentyofatoms, will work on the OLD (i.e. containing several
atoms) group, and lines that come **after** will work on the NEW (containing
only a single atom) group.

MoveCenter
~~~~~~~~~~

.. warning::

    Expert option, not well-tested.

::

    MoveCenter Group group [MinVal X=0.9] [MaxFrac X=0.5] 
    [ProbabilityToMove X=0.1] [ProbabilityToChangeFrame X=1]

For each atom in group that has precisely **two** atoms in the
coordination shell, MOVE that atom (atom 2) randomly. The first atom in
the coordination shell (atom 1) should be the “covalently-bound” atom,
and the second atom (atom 3) the “hydrogen-bonded” atom (Here, I am
assuming that I want to move a H that is bound to an O and
hydrogenbonded to another O).

MinVal is the minimum distance that is allowed after the move.

MaxFrac specifies the maximum allowed distance between atoms 1 and 2
(after the move) as a fraction of the distance between atoms 1 and 3.

ProbabilityToMove specifies the probability that a given atom 2 will be
moved, given that the overall frame is changed.

ProbabilityToChangeFrame specifies the probability that the action will
act at all on the frame. It is thus possible to skip frames randomly by
setting this to a number to a smaller value than 1.

NoDistances
~~~~~~~~~~~

::

    NoDistances

Do not calculate the distance table. This gives significant speedup if you do not need distances.

Overwrite
~~~~~~~~~

::

    Overwrite

Overwrite output files without asking. See also: DevNullIfFileExists

Prefix
~~~~~~

::

    Prefix [x=""]

Prefix all output file names.

PrintGroup
~~~~~~~~~~

::

    PrintGroup groupname XYZ filename [Every X=1] [Unwrapped] 
    [CenterOn X=-1] [CenterOnId] [CenterAtOrigin]
    [CenterOnXYZ x y z]
    [NoEmptyFrames]
    [MinSize X=0] [MaxActualPrint=0]
    [IfGroupEmpty groupname] [IfGroupNotEmpty groupname]
    [FirstCenterOnStaticXYZ]
    [PrintIndividualCoordinationEnvironments] [PrintPolyhedra] 
    [ScaleBy X=1] [ScaleByMax X=1] [CTScaleBY X=ScaleBy] [CTMinDist X=0] 
    [Special group1 label1 group2 label2 group3 label3 ...]

If the group is empty, a single Hydrogen atom will be printed (unless
NoEmptyFrames is given). Gdis is not very happy with empty XYZ files...

Unwrapped will print unwrapped coordinates. Note that you probably want
to set IntelligentUnwrap before trying this. (or read in the unwrapped
coordinates from the dump file)

CenterOn X means that all the atoms in the group will be printed
"centered" around the atom with index X in groupname (this is NOT equal
to the atom id), the index starts at 0. X=0 would center the output
around the atom that was first added to the group (often the one with
the lowest atomic id), X=1 around the next atom. The CenterOn keyword is
useful if you for example want to study the coordination shell around an
ion, and you want to visualize the results (that way the molecule
"sticks together" even if it crosses a periodic boundary). The default
is to not have CenterOn turned on, i.e. the atomic coordinates (as read
from the dump file) are printed (this corresponds to the CenterOn value
of -1).

CenterOnId changes the meaning of CenterOn such that the ACTUAL atomic
id is specified instead

The Special keyword must come last on the line, and allows for changing
the printed atom types. This can help during visualization. For example,
all oxygen atoms coordinated to a metal atom can be colored differently.
It is also useful if the atom types read by the program are not the atom
types you want to print. For example, my LAMMPS program thinks of Zn as
atom type 14. Rather than printing 14 in the output (which would result
in a Si atom being printed), I would specify Special ZincAtoms Zn/30,
where ZincAtoms is a group of all zinc atoms (DefineGroup ZincAtoms
ATOMICNUMBER 14).

In the Special keyword, the groups are sorted according to priority. So
if an atom is both in group1 and group2, it will receive the group1
label.

ScaleBy provides a scaling factor for all coordinates (for x y AND z
components), and also scales the lattice parameters by the same amount.

ScaleByMax CHANGES the meaning of ScaleBy, so that the effective scale
factor becomes a RANDOM number between ScaleBy and ScaleByMax. But this
only happens if ScaleByMax is != 1, otherwise, the normal ScaleBy
behavior is done.

PrintIndividualCoordinationEnvironments will print ONE FRAME per
coordination polyhedron.

PrintPolyhedra prints the coordination table around each central atom
(using a "CenterOn" around the central atom). If the same atom is in the
coordination table by two or more atoms, there is a risk that it will be
printed twice or more times. You might want to postprocess with
mergexyz.awk.

CTScaleBy Only when PrintPolyhedra is set: scale the positions of the
coordination tables by this factor. The default is to scale by the same
amount as ScaleBy (that itself defaults to 1).

CTMinDist Only when PrintPolyhedra is set: set the minimum allowed
(printed) distance between a central ion and its coordination shell. For
example if you have a box of water molecules, with O coordinated to H,
and you use PrintPolyhedra ScaleBy 0.8, there’s a danger that some O-H
distances will be very short (all coordinates are scaled to 80 % of
their values). CTMinDist 0.9 will rescale those distances to 0.9 Å if
they are shorter than that.

PrintProperties
~~~~~~~~~~~~~~~

::

    PrintProperties filename [Every X=1] [MultipleLines] [NewLineSeparator]
    [TabSeparator] [NumEntries] 
    [ALLGROUPS] [GROUPS group1 [group2 group3 group4...]] 
    PROPERTIES [property1 [property2 property3...]]

Print properties for all atoms in group1, group2, group3, etc. The
GROUPS or ALLGROUPS directive must come before the PROPERTIES directive
on the configuration line (ALLGROUPS is equivalent to listing all groups
that have been defined)

MultipleLines prints one line per group per timestep (default is to
print all groups on the same line per timestep), and frames are
separated by "### FRAME ###"

NewLineSeparator inserts a newline before eac hnew atom’s properties are
printed

TabSeparator inserts a tab character before each new atom’s properties
are printed

The properties can be both per-atom (PA) or per-group (PG) or
per-timestep (PT).

**Per-atom properties (PA)**:

-  x - x-coordinate (PA)

-  y - y-coordinate (PA)

-  z - z-coordinate (PA)

-  q - charge (PA)

-  bonds - only if groupname is defined using the BOND directive (or
   with an AngleRDF), print the bonds from central atom to bonding
   partners in coordination table (PA)

-  bond0 - shortest bond (PA)

-  bond1 - second shortest bond (PA)

-  bond2 - third shortest bond (PA)

-  bond3 - fourth shortest bond (PA)

-  coordinationnumber - only if groupname is defined using the BOND
   directive (or with an AngleRDF), print the coordination number (PA)

-  coordinationtable - only if groupname is defined using the BOND
   directive (or with an AngleRDF), print the coordination table (PA)

-  id - atom id (integer from 1 to #atoms) (PA)

-  type - atom type (whatever was specified in the dump file) (PA)

**Per-group properties (PG)**:

-  atomtypes - the atom types (PG)

-  averagesize - average size of the group (PG)

-  coordinationtypes - the types of the atoms in the coordination table
   (PG)

-  distances - distances between all atoms in the group. If four atoms
   are present in the group and you print distances, the distances will
   be printed in the following order: 1-2 1-3 1-4 2-3 2-4 3-4 (PG)

-  groupname - name of the group. printed only once. (PG)

-  numatoms - number of atoms in the group. numatoms will only be
   printed once per timestep (not once per atom in the group) - for
   sensible output, make sure numatoms is the first property listed (PG)

-  totalsize - total size of the group (summed over all timesteps) (PG)

**Per-timestep properties (PT)**:

-  timestepnumber - the timestep number (given by TIMESTEP inside LAMMPS
   .cfg files... this number starts at -1000 for .xyz files) (PT)

-  timestepiteration - the timestep iteration : running count of the
   number of frames read (first frame = 1, second frame = 2 etc.) (PT)

The default output is written as one line per timestep the action is
performed (every X timesteps...) On each line, there will be (#atoms in
group)*(#of properties to write) words, if all properties belong to
[x,y,z,q,coordinationnumber,id,type]. the first (#properties to write)
elements will correspond to the first atom in the group, the next
(#properties to write) elements will correspond to the next atom, etc.

See also: Histogram.

ReadFile
~~~~~~~~

::

    ReadFile filename

Performs the commands in filename (filename should be given relative to
ReadFileBaseDir). If the program complains about an error in your
configuration file, it will print a line number, that is WRONG if you’ve
used the ReadFile command.

You can specify absolute paths starting with "/".

ReadFileBaseDir
~~~~~~~~~~~~~~~

::

    ReadFileBaseDir directory

Directory with files that can be read in using ReadFile. The default is
the current working directory.

RDF
~~~

::

    RDF fromgroup togroup filename [MinDist X=0.0] [MaxDist X=10.0]
    [Resolution X=0.01] [Every X=1] [PeriodicImages]

The RDF command is pretty self-explanatory. The normalization is done
with respect to the average sizes of fromgroup and togroup. Make sure
this is reasonable if you have groups with variable number of atoms. The
PeriodicImages flag calculates the distances in 27 periodic images
around the central cell.

SharedLigands
~~~~~~~~~~~~~

.. warning::

    Expert feature

::

    SharedLigands groupname filename1 filename2 
    CentralParentGroup centralparentgroup 
    LigandParentGroup ligandparentgroup 
    CentralGroups cgroup1 [cgroup2 cgroup3...] 
    LigandGroups lgroup1 [lgroup2 lgroup3...]

Calculates histograms of the number of shared ligands between between
all combinations of the different kinds of centralgroups (filename1), as
well as the weighted averages (filename2).

groupname should be the LIGANDS with coordination tables to the CENTRAL
IONS. The CentralGroups should be groups of CENTRAL IONS with
coordination tables to LIGANDS, e.g. different kinds of coordination
polyhedra. The LigandGroups is not really implemented yet, so just set
it to the LigandParentGroup (when implemented it is supposed to give the
number of each different kind of ligand in the polyhedra connections,
e.g. differentiate between hydroxide or aqua ligands that are shared).

Example:

::

      SharedLigands O_Na shared.dat wavg.dat \
      CentralParentGroup Na LigandParentGroup O  \
      CentralGroups seesaw tetrahedral squarepyramidal trigonalbipyramidal LigandGroups O

Shell
~~~~~

::

    Shell shellcmd

Performs the shell command IMMEDIATELY.

ShortDelta
~~~~~~~~~~

.. warning::

    Preferably use DoubleCoordinationShortDelta instead of this one.

::

    ShortDelta group1 group2 group3 filename MaxValue X ParentGroup X
    [Group3MustBe X] [ExcludeGroup1CoordinationTable] [Resolution X]
    [SaveExactDelta] [NewGroup1 ng1] [NewGroup2 ng2] [NewGroup3 ng3]
    [SetDelta 1/2/3/4] [DesiredWinner 1/3] [HistoryWinner X] [HistoryGroup groupX] 
    [Margin X] [NewGroup1CoordinationGroup {2,3}]
    [NewGroup2CoordinationGroup {1,3}] [NewGroup3CoordinationGroup {1,2}]
    [Every X=1]

Calculate "delta", which is found as follows: For EACH atom in group1,
find the SHORTEST bond to group2, and then the SHORTEST bond from that
atom in group2 to group3. delta is the absolute value of the DIFFERENCE
between the bond lengths (atom1 to atom2) and (atom2 to atom3).

delta can be rounded to the desired Resolution to simplify histogramming
later, OR the exact delta can be saved using SaveExactDelta. MaxValue
gives the maximum allowed value of delta. ParentGroup is the parent
group of group1 and group 3. Group3MustBe allows you to put the
additional condition that the atom from group3 that was found in the
above procedure must be a member of some specific group for success.
ExcludeGroup1CoordinationTable excludes any atoms in the coordination
shell around an atom in group1 as possible candidates for group2.
NewGroup1,2,3 and NewGroup{1,2,3}CoordinationGroup work as for AngleRDF.
If a successful triad is found, the atoms are added to new groups with
the desired coordination tables.

There are some pretty esoteric options that you can also set, that might
be useful (although in the end I found them to not be so useful):

SetDelta changes the "delta" properties of the atoms that form
successful triads. The delta is a property of the ATOM, so it is a
"global" kind of change.

Don’t mix different ShortDelta commands in the same configuration file
unless you know what you are doing (do not overwrite "delta" from the
previous run, since only one delta per atom can be saved!)

-  1 means set delta for atoms in group1;

-  2 means set delta for atoms in group2;

-  3 means set delta for atoms in group3;

-  4 means set delta for all of the atoms in the triad (group1 + group2
   + group3)

You can put additional conditions on the \*history\* of the atoms in
group1 and group3. The idea for the implementation was to separate a
hydroxide ion with some given delta into hydroxide ions that were
"previously" hydroxide ions, and hydroxide ions that were "previously"
water molecules (that have just become hydroxide ions because of proton
transfer). This is done by going back HistoryWinner time steps, and
checking if the atom in group1 has been a member in HistoryGroup for at
least Margin \*more\* time steps than the atom in group 3 has been a
member of HistoryGroup. Only in that case is delta updated and the atoms
added to NewGroup1,2,3.

If you want the atom in group3 to have the longer membership history in
HistoryGroup instead, specify DesiredWinner 3.

SingleEnvironment
~~~~~~~~~~~~~~~~~

::

    SingleEnvironment groupname maxdist filename [Special group1 label1 group2 label2...]

Prints the environment around a random member in groupname, short for

::

    DefineGroup X SUBGROUPRANDOM groupname
    DefineGroup Y BOND All X MaxDist maxdist
    DefineGroup Z SUM X Y
    PrintGroup Z XYZ filename CenterOn 0 NoEmptyFrames [Special group1 label1 group2 label2...]

SphericalHarmonics
~~~~~~~~~~~~~~~~~~

.. note::

    Only available if you explicitly compiled lionanalysis to support it. Expert feature

::

    SphericalHarmonics groupname filename 
    [Group newgroup1 orderparam1-1 min1-1 max1-1 [orderparam1-2 min1-2 max1-2] [...]] 
    [Group newgroup2 ...] 
    [DiffGroup diffgroup]

::

    SphericalHarmonics groupname filename [LibraryRange min max]
    [MaxLibraryError x] [Library group 1 op1 2 op2 3 op3...] 
    [Library group 1 op1 2 op2 3 op3....] [DiffGroup diffgroup]

The group groupname should consist of atoms with coordination tables.
SphericalHarmonics calculates the Steinhard order parameters (based on
spherical harmonic functions) using the central atom and the entire
coordination table. There are two ways to use this command:

-  Group: Specify ranges of allowed values of the order parameters, and
   assign atoms that fulfill ALL of them into groups.

-  Library: Specify reference (library) values of the order parameters,
   then assign atoms into groups based on which set of reference values
   the calculated order parameters are "nearest" to. The MaxLibraryError
   sets a cutoff for the maximum error that is allowed towards the
   reference values (the error is for the entire vector of reference
   values, not for the individual values). MaxLibraryRange specifies
   which range of order parameters to use for the comparison. For
   example, MaxLibrary 1 5 means to use the order parameters 1, 2, 3, 4,
   5, but you can still specify more order parameters on the Library
   lines (that are not used).

In both cases, the DiffGroup creates a group with all atoms in groupname
that were not assigned into any of the groups by the command. The
DiffGroup has to come AFTER all Group or Library commands.

The :math:`l`\ th order parameter for the cluster :math:`i` is:

.. math:: 

    Q_l^i = \sqrt{\frac{4 \pi}{2l +1} \sum_{m=-l}^{+l} | \frac{1}{N} \sum_{j=1}^N Y_l^m(\mathbf{r}_j)|^2 }

where :math:`Y` is the spherical harmonic and :math:`\mathbf{r}_j` is a
vector from the central atom to one of the coordinated atoms. The
cluster is thus characterized by a vector of :math:`Q`-values,
:math:`\mathbf{Q}^i = (Q_1^i, Q_2^i, ..., Q_n^i)`. In the Library
version, the error towards a reference :math:`\mathbf{Q}`-vector
:math:`\mathbf{Q}_j` is calculated as:

.. math:: 

    M(i,j) = 1 - \frac{|\mathbf{Q}_i - \mathbf{Q}_j|}{\sqrt{\mathbf{Q}_i^2 + \mathbf{Q}_j^2}} 

See for example "Clusters of polyhedra in spherical confinement", PNAS 2016.

StartByte
~~~~~~~~~

::

    StartByte X

Specify at which byte to start reading the dumpfile (I haven’t tested
this when more than one DumpFile is specified). The default is 0. (This
command is used by the olblock.sh utility).

Suffix
~~~~~~

::

    Suffix [x=""]

Suffix all output file names.

SuperEvery
~~~~~~~~~~

::

    SuperEvery x=1

Only read every SuperEvery frames from the trajectory.


T
~~~~~~

Time correlation functions. There is no command called "T", instead use TMSD,
TMSDFollow, TVACF, TResidenceTime, TResidenceTimeCoordinationShell,
TResidenceTimeSSP, or TResidenceTimeSSPCoordinationShell. The commands share
much of the same syntax:

::

    T ParentGroup X Group X Filename X MaxHistory X [RealTime] 
    [TimeUnit X=1] [Every X=1] [CorrelationFrequency X=1timestep] [TidyOld X]
    [PrintAllMembers] [PrintHeader] [NoHeader] 
    [PrintEvery X(timestepunits,unaffected by RealTime)] [WriteRestart filename]
    [ReadRestart filename] [DontUseAllTimeOrigins]
    [UncareAllWhenUncareRemaining] [ValuePrecedenceOverUncare]
    [OldMemberEscape{1,3}[!] history continuous max CONSEQUENCE group]
    [NewMemberEscape{1,3}[!] history continuous max CONSEQUENCE group]
    [OldValueEscape{1,2}[!] history continuous max CONSEQUENCE]
    [NewValueEscape{1,2}[!] history continuous max CONSEQUENCE]
    [OldCoordinationEscape1 history continuous max CONSEQUENCE groupoftypecoordinationgroup]
    [NewCoordinationEscape1 history continuous max CONSEQUENCE groupoftypecoordinationgroup]

.. note::

    The below is probably quite difficult to understand for somebody who hasn't developed the code.

    It's easiest/safest to find a working example and copy-paste that.

**Consequences**. CONSEQUENCE can be:

-  NoConsequence (no): no consequence (default - it is the same as not
   writing the flag at all.)

-  UncareThis (ut): do not add to the correlation function for THIS
   particular time origin and THIS particular time end (dt)

-  UncareRemaining (ur): do not add to the correlation function for THIS
   particular time origin and ANY time end that is greater than or equal
   to the current one (up to the maximum of MaxHistory)

-  ValueThis (vt): add 0 to the correlation function for THIS particular
   time origin and THIS particular time end (dt)

-  ValueRemaining (vr): add 0 to the correlation function for THIS
   particular time origin and ANY time end that is greater than or equal
   to the current one (up to the maximum of MaxHistory)

**Options**:

-  RealTime: MaxHistory and CorrelationFrequency (and parameters for
   OldEscape1 and NewEscape1) are specified in picoseconds
   (BasicTimeUnit must be set BEFORE this line).

-  DontUseAllTimeOrigins: Only use :math:`t_0` as time origin, if the
   trajectory doesn’t end before :math:`t_0 +`\ MaxHistory.

-  UncareAllWhenUncareRemaining: Modify the UncareRemaining consequence
   to UncareAll. That is, the consequence will uncare the ENTIRE range
   0–MaxHistory if it is triggered. This flag is not extremely
   well-tested yet.

-  ValuePrecedenceOverUncare: This means that ValueRemaining will take
   precedence over UncareRemaining, for times where both escape options
   are triggered, AND the ValueRemaining gets triggered *before* the
   UncareRemaining. So the normal kind of OldValueEscape1 0 0 0 UR
   normally has the greatest precedence in any case. The default is that
   UncareRemaining takes precedence over ValueRemaining. NOTE that
   ValuePrecedenceOverUncare only affects ValueRemaining and NOT
   ValueThis.

-  PrintAllMembers: Print, for each atom in ParentGroup, the time
   correlation function. (The default is to average over all members).

-  PrintEvery: Print output every X frames (NOT affected by RealTime).

-  CorrelationFrequency: Specify how often to actually calculate the
   time correlation function. THe default is every timestep, but it’s
   sometimes faster to set this to some large number.

**Escape options**:

There are three kinds of conditions that can be placed, a condition on
the MEMBERSHIP and a condition of the VALUE and a condition on
COORDINATION:

-  The condition on VALUE can only be done for boolean input data, i.e.
   for ResidenceTime/SSP/CoordinationShell

-  The condition on COORDINATION can only be done for
   ResidenceTimeCoordinationShell and ResidenceTimeSSPCoordinationShell

-  The condition on VALUE can make a distinction whether you’re doing it
   for the REACTANTS (1) or the PRODUCTS (2) in SSP/SSPCoordinationShell
   (if you do not use SSP, then always use 1)

The condition on MEMBERSHIP can make a distinction for the CENTER ATOM
(1) or the COORDINATION SHELL (3) (if you do not have a coordination
shell, always use 1).

You can negate using !, so that you get the consequence if the
Member/Value HASN’T escaped.

The "recommended" way to set up a calculation is to make Group1 (or
Reactants/Products) as "general" as possible, and then to limit the
scope of the time correlation function using the Escape options.

-  **OldMemberEscape1 history continuous max CONSEQUENCE groupname**

   Check the membership history of an atom in the Group groupname until
   the time origin 0.

   If "groupname" is the shorthand "@", use the group specified by the
   Group option (not available for OldMemberEscape3).

   history should be >=0 for this to make sense

   continuous and max as for NewEscape1

   Please note that that this checks only whether the atom was a MEMBER
   of group, NOT whether e.g. a specific \*bond\* was intact (for this
   use OldValueEscape1 instead)

   if history >=1, you probably have to use TidyOld >=3

   Special case 1: Member at time origin? OldMemberEscape1 0 0 0

-  **NewMemberEscape1 history continuous max CONSEQUENCE groupname**

   check the membership history of an atom in the Group groupname until
   the time end dt

   if "groupname" is the shorthand "@", use the group specified by the
   Group option (not available for NewMemberEscape3)

   history species how far "back" to check the history from the time end
   dt. if history < 0, then the history is checked between the time
   origin and time end.

   continuous is the maximum continuous escape time. set to <0 to
   invalidate

   max is the maximum total escape time. set to <0 to invalidate

   Please note that this checks only whether the atom was a MEMBER of
   group, NOT whether e.g. a specific bond was intact (for this use
   NewValueEscape1 instead)

   Special case 1: Continuous membership from time origin to time end:
   NewMemberEscape -1 0 0 (you probably want UR/VR as consequence here)

   Special case 2: t\* = 5 ("newmaxcontinuousescapetime"):
   NewMemberEscape -1 5 -1

-  **OldValueEscape1 history continuous max CONSEQUENCE**

   checks for TRUE values, otherwise CONSEQUENCE

   This is what you want to use to check whether a specific BOND was
   intact at e.g. the time origin

   if history >=1, you probably have to use TidyOld >=3

   Special case 1: OldValueEscape1 0 0 0

   You probably more or less always set this, and you probably want UR
   as consequence

-  **NewValueEscape1 history continuous max CONSEQUENCE**

   checks for TRUE values, otherwise CONSEQUENCE

   This is what you want to use to check whether a specific BOND was
   intact at e.g. the time origin

   Special case 1: Continuous membership from time origin to time end:
   NewValueEscape -1 0 0

   Special case 2: t\* = 5 ("newmaxcontinuousescapetime"):
   NewValueEscape -1 5 -1

   Special case 3: in SSP you you need NewValueEscape2! 0 0 0 VR for
   absorbing boundary conditions

   Special case 4: in TResidenceTimeSSPCoordinationShell you need
   NewValueEscape2! 0 0 0 VR for absorbing boundary conditions

-  **OldCoordinationEscape1 history continuous max CONSEQUENCE groupoftypecoordinationgroup**

   Checks the membership history of an atom in the group
   groupoftypecoordinationgroup (exactly as OldMemberEscape1 would do),
   AND checks if the atoms in the shellgroup are also in the
   coordination shell of groupoftypecoordinationtypegroup

-  **NewCoordinationEscape1 history continuous max CONSEQUENCE groupoftypecoordinationtypegroup** as above

Differences to the previous version of the code:

-  "NewMustBeMember" is implicit, i.e. always set. But you can recover
   the "plateau"-behavior by setting Group (in the normal
   TResidenceTime) to whatever the ParentGroup is, and then using
   NewValueEscape1 -1 X -1 VR.

-  "OverwriteInData" does not exist

Threads
~~~~~~~

::

    Threads X

Run on X threads. The default is whatever the environment variable
OMP_NUM_THREADS is set to.

TimeDensity
~~~~~~~~~~~

::

    TimeDensity Group X Region R Filename Y [TimeUnit X]

The region R should be defined using the DefineGroup REGION directive.
Gives the density (number density and mass density) of all the atoms in
Group X for each timestep For the mass density, the Group X needs masses
to be set. This must be done using DefineGroup ATOMICNUMBER ... Mass ...

Known bug: if you use QuickDefine, the masses do not get updated after
the first time step (if the groups are static). So manually use
DefineGroup ATOMICNUMBER ... Mass ... and do not set Static.

TMSD
~~~~

TMSD

Like T[], and: [X/Y/Z/XY/YZ/XZ/XYZ(default)]

**REMINDER: For TMSD you most likely want to specify IntelligentUnwrap as
a standalone command, unless unwrapped coordinates are written to the
dumpfile**.

TMSDFollow
~~~~~~~~~~

TMSDFollow

Specify ParentGroup as the kind of group that you want to follow! It’s
ok if the group size changes (althoug hthat will of course create a
"conflict" during the calculation of the MSD).

**REMINDER: For TMSDFollow you most likely want to specify
IntelligentUnwrap as a standalone command, unless unwrapped coordinates
are written to the dumpfile.**

TResidenceTime
~~~~~~~~~~~~~~

Uses the basic T syntax.

TResidenceTimeCoordinationShell
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

TResidenceTimeSSP
~~~~~~~~~~~~~~~~~

TResidenceTimeSSP like T, and:

Reactants X Products X [NewValueEscape2! 0 0 0 VR productgroup]

"Reactants X" is the same as "Group X", so use \*either\* Reactants or
Group. "Products X" is the same as "Group2 X", so use \*either\*
Products or Group2. In SSP you want probably absorbing boundary
conditions, so that as soon as something becomes the product, the
reaction is counted to have happened for all times greater than or equal
to the current dt. Thus, you most likely want to specify
NewValueEscape2! 0 0 0 VR

TResidenceTimeSSPCoordinationShell
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

TVACF
~~~~~

TVACF like T, and: [X/Y/Z/XY/YZ/XZ/XYZ(default)] [ManipulateOut filename
min max Peak/WAvg]

If you want to feed the output into the standalone ftvac utility, you
should set DontPrintRawNumbers and TimeUnit 1 and NoHeader.

ManipulateOut performs a fourier transform of the time correlation
function, and either takes the Peak or WAvg of the power spectrum in the
interval [min,max], where min and max are given in cm\ :math:`^{-1}`.
The idea is to get the “instantaneous” frequency at some time step. You
can then use the standalone correlate utility to get the
frequency-frequency time correlation function (i.e., spectral
diffusion). There are some parameters for the fourier-transform that at
the moment are hard-coded.

WhenGroupChanged
~~~~~~~~~~~~~~~~

::

    WhenGroupChanged Id X BeforeTime X AfterTime X Filename X 
    Group X(must be const size!) Property X [TimeUnit X] [RealTime] 
    [PrintEvery X(timestepuntis)] [ProductMustBe X]

This action is a snake in the grass and continuously saves the Property
for each member in the group. The Group MUST have a constant size.

The action is called by ChangeGroupTime when the reaction happens, and
receives the atom id. It keeps saving the property another AfterTime
timesteps, and the prints the results to the output file. (The results
are histogrammed, so you only get the average output and not each
individual output)

Currently, property can be:

-  bonds: average bond length, IGNORE if the coordination number has
   been zero in the time interval [-BeforeTime,AfterTime]

-  bond0: the shortest bond, IGNORE as above

-  bond1: the second shortest bond, IGNORE as above

-  bond2: the third shortest bond, IGNORE as above

-  bond3: the fourth shortest bond, IGNORE as above

-  delta: the atom delta value (from ShortDelta action)

-  coordinationnnumber: the coordination number

WhenGroupChangedDefineGroup
~~~~~~~~~~~~~~~~~~~~~~~~~~~

::

    WhenGroupChangedDefineGroup groupname [Id X] [ParentGroup X] 
    [Duration X(timestepunits)=1] [Delay X(timestepunits)=0]

This creates a group called groupname, with members at each timestep
populated by whatever atoms are fed into it through:

-  the ChangeGroupTime action that calls the action via the given Id (do
   NOT set ParentGroup), OR

-  the ParentGroup that is specified – NOTE that if ParentGroup is
   specified, then "WhenGroupChanged" is a bit of a misleading name for
   this action, since you only need the members in the ParentGroup, and
   they need not have "changed" from anything else

WARNING: You SHOULD NOT set ParentGroup if you set Id. That will NOT
GIVE YOU THE DESIRED FUNCTIONALITY. The Duration options allows you to
populate groupname for some continuous amount of time after an event.
The Delay option inserts a delay.

