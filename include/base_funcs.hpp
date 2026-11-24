#pragma once

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// Полиморфная база: хранит функторы значения, производной и строкового представления.
class TBaseFunction {
public:
    virtual ~TBaseFunction() = default;

    double operator()(double x) const { return value_(x); }
    double GetDeriv(double x) const { return deriv_(x); }
    std::string ToString() const { return to_string_(); }

protected:
    using TValueFn = std::function<double(double)>;
    using TDerivFn = std::function<double(double)>;
    using TStringFn = std::function<std::string()>;

    TBaseFunction(TValueFn value, TDerivFn deriv, TStringFn to_string)
        : value_(std::move(value)),
          deriv_(std::move(deriv)),
          to_string_(std::move(to_string)) {}

private:
    TValueFn value_;
    TDerivFn deriv_;
    TStringFn to_string_;
};

// Базовые типовые функции.
class TIdentity : public TBaseFunction {
public:
    TIdentity();
};

class TConstant : public TBaseFunction {
public:
    explicit TConstant(double value);
};

class TPower : public TBaseFunction {
public:
    explicit TPower(double exponent);
};

class TExp : public TBaseFunction {
public:
    TExp();
};

class TPolynomial : public TBaseFunction {
public:
    explicit TPolynomial(std::vector<double> coefficients);
};

// Базовые функции-операции: операнды — такие же полиморфные функции.
class TSum : public TBaseFunction {
public:
    TSum(std::shared_ptr<TBaseFunction> lhs, std::shared_ptr<TBaseFunction> rhs);
};

class TSub : public TBaseFunction {
public:
    TSub(std::shared_ptr<TBaseFunction> lhs, std::shared_ptr<TBaseFunction> rhs);
};

class TMul : public TBaseFunction {
public:
    TMul(std::shared_ptr<TBaseFunction> lhs, std::shared_ptr<TBaseFunction> rhs);
};

class TDiv : public TBaseFunction {
public:
    TDiv(std::shared_ptr<TBaseFunction> lhs, std::shared_ptr<TBaseFunction> rhs);
};

// Клиентская обёртка над любой базовой функцией или выражением.
class TFunction {
public:
    explicit TFunction(std::shared_ptr<TBaseFunction> impl);

    TFunction(const TFunction&) = default;
    TFunction(TFunction&&) = default;
    TFunction& operator=(const TFunction&) = default;
    TFunction& operator=(TFunction&&) = default;

    double operator()(double x) const;
    double GetDeriv(double x) const;
    std::string ToString() const;

private:
    void EnsureImpl() const;

    std::shared_ptr<TBaseFunction> impl_;

    friend TFunction operator+(const TFunction& lhs, const TFunction& rhs);
    friend TFunction operator-(const TFunction& lhs, const TFunction& rhs);
    friend TFunction operator*(const TFunction& lhs, const TFunction& rhs);
    friend TFunction operator/(const TFunction& lhs, const TFunction& rhs);
};

TFunction operator+(const TFunction& lhs, const TFunction& rhs);
TFunction operator-(const TFunction& lhs, const TFunction& rhs);
TFunction operator*(const TFunction& lhs, const TFunction& rhs);
TFunction operator/(const TFunction& lhs, const TFunction& rhs);

// Любая смешанная операция (TFunction + не-TFunction или наоборот) — ошибка.
template <typename L, typename R,
          std::enable_if_t<
              (std::is_same_v<std::decay_t<L>, TFunction> ||
               std::is_same_v<std::decay_t<R>, TFunction>) &&
                  !(std::is_same_v<std::decay_t<L>, TFunction> &&
                    std::is_same_v<std::decay_t<R>, TFunction>),
              int> = 0>
TFunction operator+(L&&, R&&) {
    throw std::logic_error("operator+ requires two TFunction operands");
}

template <typename L, typename R,
          std::enable_if_t<
              (std::is_same_v<std::decay_t<L>, TFunction> ||
               std::is_same_v<std::decay_t<R>, TFunction>) &&
                  !(std::is_same_v<std::decay_t<L>, TFunction> &&
                    std::is_same_v<std::decay_t<R>, TFunction>),
              int> = 0>
TFunction operator-(L&&, R&&) {
    throw std::logic_error("operator- requires two TFunction operands");
}

template <typename L, typename R,
          std::enable_if_t<
              (std::is_same_v<std::decay_t<L>, TFunction> ||
               std::is_same_v<std::decay_t<R>, TFunction>) &&
                  !(std::is_same_v<std::decay_t<L>, TFunction> &&
                    std::is_same_v<std::decay_t<R>, TFunction>),
              int> = 0>
TFunction operator*(L&&, R&&) {
    throw std::logic_error("operator* requires two TFunction operands");
}

template <typename L, typename R,
          std::enable_if_t<
              (std::is_same_v<std::decay_t<L>, TFunction> ||
               std::is_same_v<std::decay_t<R>, TFunction>) &&
                  !(std::is_same_v<std::decay_t<L>, TFunction> &&
                    std::is_same_v<std::decay_t<R>, TFunction>),
              int> = 0>
TFunction operator/(L&&, R&&) {
    throw std::logic_error("operator/ requires two TFunction operands");
}
