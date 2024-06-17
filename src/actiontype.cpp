#include "actiontype.h"

std::string ActionTypeToString(const ActionType value) {
    std::string s="";
    switch(value){
        case ActionType::AngleRDF: s="AngleRDF"; break;
        case ActionType::DefineGroup: s="DefineGroup"; break;
        case ActionType::DihedralRDF: s="DihedralRDF"; break;
        case ActionType::FinalShell: s="FinalShell"; break;
        case ActionType::FindVacuum: s="FindVacuum"; break;
        case ActionType::MSD: s="MSD"; break;
        case ActionType::PrintProperties: s="PrintProperties"; break;
        case ActionType::PrintStructure: s="PrintStructure"; break;
        case ActionType::RDF: s="RDF"; break;
        case ActionType::ResidenceTime: s="ResidenceTime"; break;
        case ActionType::ResidenceTimeCoordinationShell: s="ResidenceTimeCoordinationShell"; break;
        case ActionType::Simple: s="Simple"; break;
        case ActionType::TimeCorrelationFunction: s="TimeCorrelationFunction"; break;
        case ActionType::Unknown: s="Unknown"; break;
        case ActionType::VACF: s="VACF"; break;
        default: s="XXX"; break;
    }
    return s;
}
std::ostream& operator<<(std::ostream& out, const ActionType value){
    return out << ActionTypeToString(value);
}
