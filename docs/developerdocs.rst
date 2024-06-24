.. _DeveloperDocs:

Developer documentation
===========================

Actions
-------

The code works by reading each frame of the dump file and for each frame
executing Actions. Each action has at least mMinOuts and at most
mMaxOuts output files, and has at least mMinGroups and at most
mMaxGroups associated with it (although this is not so strict for some
type of actions). Each action is performed every mEvery times it is
called. Pointers to the output streams are stored in mvpOuts[].

The first time an action is called internalInitialize() is called. This
function should be overridden by all inherited classes.

The actual "action" should be defined in internalAction().

The function internalPrint() should be used to print the output.

The function internalFinalAction is called by the main program after it
has finished reading all the frames. internalFinalAction() might call,
for example, internalPrint(), or do some cleanup operations.

When inheriting form Action, the constructor should read, for example,

.. code:: c

   // minimum 0 groups, maximuim 1000 groups, minimum 1 output file, maximum 1 output file
   ActionRDF::ActionRDF() :Action(ActionType::RDF, 0, 1000, 1, 1) { }

Implementing new features
-------------------------

You need to modify readconfigfile.cpp (exception: the time correlation
function keywords are in utility.cpp for some reason) so that the new
keyword can be read in. If you have implemented a new type of Action,
then create a pointer to that type of action near the top of the file.

When reading names of potential output files from the config file, do as
follows:

::

   totalfilename=o.outputfileprefix+ppfilename+o.outputfilesuffix;
   if (!permission_to_write(totalfilename, o.overwritefiles)) throw string("no permission to write to " + totalfilename);

where "ppfilename" is the string entered by the user.

After defining the Action attributes, add the action to the vpActions
vector:

::

   addAction(vpActions, pActionWhenGroupChangedDefineGroup);

For new Actions: modify actiontype.h to include your new action.

For new Groups: modify group.h to include your new group.
