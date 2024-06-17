#ifndef ACTIONTYPE_H
#define ACTIONTYPE_H

#include <iostream>
#include <string>

enum class ActionType {
    AngleRDF,
    ChangeGroupTime,
    CheckOverlap,
    DefineGroup,
    DefineGroupAtomicNumber,
    Density,
    DoubleShortDelta,
    DihedralRDF,
    FinalShell,
    FindVacuum,
    Histogram,
    MoveCenter,
    MSD,
    MSDFollow,
    PrintProperties,
    PrintStructure,
    RDF,
    ResidenceTime,
    ResidenceTimeSSP,
    ResidenceTimeSSPCoordinationShell,
    ResidenceTimeContinuous,
    ResidenceTimeCoordinationShell,
    ResidenceTimeSwitchBond,
    Simple,
    SharedLigands,
    ShortDelta,
    SphericalHarmonics,
    SumVelocities,
    TimeCorrelationFunction,
    TimeDensity,
    TwoDimSDF,
    Unknown,
    VACF,
    WhenGroupChanged,
    WhenGroupChangedDefineGroup
};
std::ostream& operator<<(std::ostream&, const ActionType);
std::string ActionTypeToString(const ActionType);

#endif
