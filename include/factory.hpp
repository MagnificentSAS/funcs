#pragma once

#include "base_funcs.hpp"

#include <string>
#include <vector>

class TFactory {
public:
    TFunction Create(const std::string& type) const;
    TFunction Create(const std::string& type, double parameter) const;
    TFunction Create(const std::string& type, const std::vector<double>& parameters) const;
};
