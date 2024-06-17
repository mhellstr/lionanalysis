#include "tcfcondition.h"
int TCFConditionTypeToBoolInfoIndex(const TCFConditionType a) {
    switch (a) {
        case TCFConditionType::OldIsMember1:
        case TCFConditionType::OldMustBeMemberFor1:
        case TCFConditionType::NewIsMember1:
        case TCFConditionType::NewMustBeMemberFor1:
        case TCFConditionType::NewMustBeContinuousMember1:
        case TCFConditionType::NewIsNotMember1:
            return BoolInfoIndex::IsMember1;
        case TCFConditionType::OldIsMember2:
        case TCFConditionType::OldMustBeMemberFor2:
        case TCFConditionType::NewIsMember2:
        case TCFConditionType::NewMustBeMemberFor2:
        case TCFConditionType::NewMustBeContinuousMember2:
        case TCFConditionType::NewIsNotMember2:
            return BoolInfoIndex::IsMember2;
        case TCFConditionType::OldIsMember3:
        case TCFConditionType::OldMustBeMemberFor3:
        case TCFConditionType::NewIsMember3:
        case TCFConditionType::NewMustBeMemberFor3:
        case TCFConditionType::NewMustBeContinuousMember3:
        case TCFConditionType::NewIsNotMember3:
            return BoolInfoIndex::IsMember3;
        case TCFConditionType::OldIsMember4:
        case TCFConditionType::OldMustBeMemberFor4:
        case TCFConditionType::NewIsMember4:
        case TCFConditionType::NewMustBeMemberFor4:
        case TCFConditionType::NewMustBeContinuousMember4:
        case TCFConditionType::NewIsNotMember4:
            return BoolInfoIndex::IsMember4;
        case TCFConditionType::OldValueIsTrueFor1:
        case TCFConditionType::OldValueIsFalseFor1:
        case TCFConditionType::NewValueIsTrueFor1:
        case TCFConditionType::NewValueIsFalseFor1:
            return BoolInfoIndex::Value1;
    }
    return BoolInfoIndex::Unknown;
}
