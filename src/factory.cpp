#include "factory.hpp"

TFunction TFactory::Create(const std::string& type) const {
    return TFunction(funcs_detail::SelectBaseFunction(type));
}

TFunction TFactory::Create(const std::string& type, double parameter) const {
    return TFunction(funcs_detail::SelectBaseFunction(type, parameter));
}

TFunction TFactory::Create(const std::string& type,
                           const std::vector<double>& parameters) const {
    return TFunction(funcs_detail::SelectBaseFunction(type, parameters));
}
