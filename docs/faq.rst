.. _FAQ:

=================================
Frequently asked questions
=================================

#. I want to change the coordination table of the atoms in group G

   #. DefineGroup INTERSECTION will create a new group with coordination
      tables from the 1st specified group.

#. What’s going on with those weird escape options for time correlation
   functions in the T command?

   #. There are many possible things you might like to control.

      #. There is the VALUE of the tc function (ValueEscape1); for SSP
         you might put conditions on REACTANTS (ValueEscape1) or
         PRODUCTS (ValueEscape2).

      #. There is MEMBERSHIP of an atom, which might be CENTRAL ATOM
         (MemberEscape1) or COORDINATION SHELL (MemberEscape3).

      #. The distinction between VALUE and MEMBERSHIP is clear when
         “bond” lifetimes are calculated. The VALUE indicates whether
         the bond is intact, and the MEMBERSHIP what kinds of atoms make
         up the bond. For “identity” lifetimes, the VALUE is whether the
         atom is a member of Group. For “real-valued” time correlation
         functions like TMSD, the VALUE is the MSD (the valueescape
         option not particularly well-suited for real-valued functions).

      #. You might want to set conditions for the ESCAPE or for the
         ENTERING (!)

      #. Your might like to place conditions at the history leading up
         to the TIME ORIGIN (Old), or FOR THE ENTIRE CORRELATION PERIOD
         (New... -1), or for the history leading up to the TIME END
         (New).

      #. You might want to allow for some flexibility, by setting
         ALLOWED CONTINUOUS ESCAPE TIME ("t*"; Escape x **10** x),
         possibly combining it with an ALLOWED TOTAL ESCAPE TIME (Escape
         x x **100**).

      #. If one of the conditions get triggered, you might like to, for
         the particular time origin, IGNORE THE ENTIRE TIME PERIOD
         (UncareRemaining with UncareAllWhenUncareRemaining), or IGNORE
         THE TIME AFTER THE TRIGGER (UncareRemaining), or IGNORE ONLY
         THIS PARTICULAR time (UncareThis), or ZERO THE TIME AFTER THE
         TRIGGER (ValueRemaining) or ZERO THIS PARTICULAR TIME
         (ValueThis).

   The term “Value” is used in two senses. In OldValueEscape1, you’re
   *checking* whether the correlation function becomes 0, while in the
   consequence ValueRemaining, you’re *setting* the value to 0.

#. How do I calculate the hydrogen bond lifetime (I should double-check
   this)?

   #. HydrogenBond O O_H H CopyGroup3 H...o NewGroup3CoordinationGroup 1

      TResidenceTimeCoordinationShell Filename continuous.dat Group
      H...O ParentGroup H ShellGroup O MaxHistory 100 OldValueEscape1 0
      0 0 UR NewValueEscape1 -1 0 0 VR

      TResidenceTimeCoordinationShell Filename tstar10.dat Group H...O
      ParentGroup H ShellGroup O MaxHistory 100 OldValueEscape1 0 0 0 UR
      NewValueEscape1 -1 10 -1 VR

      TResidenceTimeCoordinationShell Filename intermittent.dat Group
      H...O ParentGroup H ShellGroup O MaxHistory 100 OldValueEscape1 0
      0 0 UR

