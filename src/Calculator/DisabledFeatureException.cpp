#include "DisabledFeatureException.h"

DisabledFeatureException::DisabledFeatureException(
    std::string featureName,
    std::string featureType) : message("Error: " + featureName + " " + featureType + " is disabled") {}

DisabledFeatureException::DisabledFeatureException(
    std::string featureName
) : message (featureName + " are disabled") {}

const char *DisabledFeatureException::what() const throw() {
    return message.c_str();
}
