#include "base_funcs.hpp"

#include <cmath>
#include <iomanip>
#include <sstream>

namespace {

std::string FormatNumber(double value) {
    if (value == 0.0) {
        return "0";
    }
    std::ostringstream out;
    out << std::setprecision(15) << value;
    return out.str();
}

std::string PolynomialToString(const std::vector<double>& coefficients) {
    std::vector<std::string> terms;

    for (std::size_t i = 0; i < coefficients.size(); ++i) {
        const double coefficient = coefficients[i];
        if (coefficient == 0.0) {
            continue;
        }

        std::string term;
        if (i == 0) {
            term = FormatNumber(coefficient);
        } else {
            if (coefficient == 1.0) {
                term = "x";
            } else if (coefficient == -1.0) {
                term = "-x";
            } else {
                term = FormatNumber(coefficient) + "*x";
            }
            if (i > 1) {
                term += "^" + FormatNumber(static_cast<double>(i));
            }
        }
        terms.push_back(term);
    }

    if (terms.empty()) {
        return "0";
    }

    std::string result = terms.front();
    for (std::size_t i = 1; i < terms.size(); ++i) {
        const std::string& term = terms[i];
        if (!term.empty() && term.front() == '-') {
            result += " - " + term.substr(1);
        } else {
            result += " + " + term;
        }
    }
    return result;
}

void CheckPowerArguments(double x, double exponent) {
    if (x < 0.0 && exponent != std::floor(exponent)) {
        throw std::logic_error("Power: negative base with non-integer exponent");
    }
}

void CheckNonZero(double value) {
    if (value == 0.0) {
        throw std::logic_error("Division by zero");
    }
}

bool IsBasicNonPolynomial(const std::shared_ptr<TBaseFunction>& function) {
    TBaseFunction* raw = function.get();
    return dynamic_cast<TPolynomial*>(raw) == nullptr &&
           (dynamic_cast<TIdentity*>(raw) != nullptr ||
            dynamic_cast<TConstant*>(raw) != nullptr ||
            dynamic_cast<TPower*>(raw) != nullptr ||
            dynamic_cast<TExp*>(raw) != nullptr);
}

std::string WrapIfNeeded(const std::shared_ptr<TBaseFunction>& function, bool wrap) {
    const std::string text = function->ToString();
    return wrap ? "(" + text + ")" : text;
}

}  // namespace

// --- Базовые типовые функции ------------------------------------------------

TIdentity::TIdentity()
    : TBaseFunction([](double x) { return x; },
                    [](double) { return 1.0; },
                    [] { return std::string("x"); }) {}

TConstant::TConstant(double value)
    : TBaseFunction([value](double) { return value; },
                    [](double) { return 0.0; },
                    [value] { return FormatNumber(value); }) {}

TPower::TPower(double exponent)
    : TBaseFunction(
          [exponent](double x) {
              CheckPowerArguments(x, exponent);
              return std::pow(x, exponent);
          },
          [exponent](double x) {
              CheckPowerArguments(x, exponent);
              if (exponent == 0.0) {
                  return 0.0;
              }
              return exponent * std::pow(x, exponent - 1.0);
          },
          [exponent] { return "x^" + FormatNumber(exponent); }) {}

TExp::TExp()
    : TBaseFunction([](double x) { return std::exp(x); },
                    [](double x) { return std::exp(x); },
                    [] { return std::string("exp(x)"); }) {}

TPolynomial::TPolynomial(std::vector<double> coefficients)
    : TBaseFunction(
          [coefficients](double x) {
              double result = 0.0;
              for (std::size_t i = 0; i < coefficients.size(); ++i) {
                  result += coefficients[i] * std::pow(x, static_cast<double>(i));
              }
              return result;
          },
          [coefficients](double x) {
              double result = 0.0;
              for (std::size_t i = 1; i < coefficients.size(); ++i) {
                  result += static_cast<double>(i) * coefficients[i] *
                            std::pow(x, static_cast<double>(i - 1));
              }
              return result;
          },
          [coefficients] { return PolynomialToString(coefficients); }) {}

// --- Базовые функции-операции -----------------------------------------------

TSum::TSum(std::shared_ptr<TBaseFunction> lhs, std::shared_ptr<TBaseFunction> rhs)
    : TBaseFunction(
          [lhs, rhs](double x) { return (*lhs)(x) + (*rhs)(x); },
          [lhs, rhs](double x) { return lhs->GetDeriv(x) + rhs->GetDeriv(x); },
          [lhs, rhs] { return lhs->ToString() + " + " + rhs->ToString(); }) {}

TSub::TSub(std::shared_ptr<TBaseFunction> lhs, std::shared_ptr<TBaseFunction> rhs)
    : TBaseFunction(
          [lhs, rhs](double x) { return (*lhs)(x) - (*rhs)(x); },
          [lhs, rhs](double x) { return lhs->GetDeriv(x) - rhs->GetDeriv(x); },
          [lhs, rhs] {
              return lhs->ToString() + " - " +
                     WrapIfNeeded(rhs, !IsBasicNonPolynomial(rhs));
          }) {}

TMul::TMul(std::shared_ptr<TBaseFunction> lhs, std::shared_ptr<TBaseFunction> rhs)
    : TBaseFunction(
          [lhs, rhs](double x) { return (*lhs)(x) * (*rhs)(x); },
          [lhs, rhs](double x) {
              return lhs->GetDeriv(x) * (*rhs)(x) + (*lhs)(x) * rhs->GetDeriv(x);
          },
          [lhs, rhs] {
              return WrapIfNeeded(lhs, !IsBasicNonPolynomial(lhs)) + " * " +
                     WrapIfNeeded(rhs, !IsBasicNonPolynomial(rhs));
          }) {}

TDiv::TDiv(std::shared_ptr<TBaseFunction> lhs, std::shared_ptr<TBaseFunction> rhs)
    : TBaseFunction(
          [lhs, rhs](double x) {
              const double denominator = (*rhs)(x);
              CheckNonZero(denominator);
              return (*lhs)(x) / denominator;
          },
          [lhs, rhs](double x) {
              const double denominator = (*rhs)(x);
              CheckNonZero(denominator);
              return (lhs->GetDeriv(x) * denominator -
                      (*lhs)(x) * rhs->GetDeriv(x)) /
                     (denominator * denominator);
          },
          [lhs, rhs] {
              return WrapIfNeeded(lhs, !IsBasicNonPolynomial(lhs)) + " / " +
                     WrapIfNeeded(rhs, !IsBasicNonPolynomial(rhs));
          }) {}

// --- Клиентская обёртка ------------------------------------------------------

TFunction::TFunction(std::shared_ptr<TBaseFunction> impl) : impl_(std::move(impl)) {
    if (!impl_) {
        throw std::logic_error("TFunction: null implementation");
    }
}

void TFunction::EnsureImpl() const {
    if (!impl_) {
        throw std::logic_error("TFunction: operation on moved-from object");
    }
}

double TFunction::operator()(double x) const {
    EnsureImpl();
    return (*impl_)(x);
}

double TFunction::GetDeriv(double x) const {
    EnsureImpl();
    return impl_->GetDeriv(x);
}

std::string TFunction::ToString() const {
    EnsureImpl();
    return impl_->ToString();
}

// --- Перегруженные операторы -------------------------------------------------

TFunction operator+(const TFunction& lhs, const TFunction& rhs) {
    return TFunction(std::make_shared<TSum>(lhs.impl_, rhs.impl_));
}

TFunction operator-(const TFunction& lhs, const TFunction& rhs) {
    return TFunction(std::make_shared<TSub>(lhs.impl_, rhs.impl_));
}

TFunction operator*(const TFunction& lhs, const TFunction& rhs) {
    return TFunction(std::make_shared<TMul>(lhs.impl_, rhs.impl_));
}

TFunction operator/(const TFunction& lhs, const TFunction& rhs) {
    return TFunction(std::make_shared<TDiv>(lhs.impl_, rhs.impl_));
}
