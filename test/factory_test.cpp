#include "factory.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace {

template <typename T, typename = void>
struct HasToString : std::false_type {};

template <typename T>
struct HasToString<T, std::void_t<decltype(std::declval<const T&>().ToString())>>
    : std::true_type {};

template <typename T, typename = void>
struct HasCallOperator : std::false_type {};

template <typename T>
struct HasCallOperator<T, std::void_t<decltype(std::declval<const T&>()(
                              std::declval<double>()))>> : std::true_type {};

template <typename T, typename = void>
struct HasGetDeriv : std::false_type {};

template <typename T>
struct HasGetDeriv<T, std::void_t<decltype(std::declval<const T&>().GetDeriv(
                         std::declval<double>()))>> : std::true_type {};

TEST(TFactoryTypeTest, CreateReturnsTFunction) {
    TFactory factory;
    static_assert(std::is_same_v<decltype(factory.Create("ident")), TFunction>);
    static_assert(std::is_same_v<decltype(factory.Create("const", 1.0)), TFunction>);
    static_assert(std::is_same_v<
                  decltype(factory.Create("polynomial", std::vector<double>{})),
                  TFunction>);
}

TEST(TFactoryApiTest, ReturnedObjectHasRequiredMembers) {
    static_assert(HasToString<TFunction>::value, "TFunction must provide ToString()");
    static_assert(HasCallOperator<TFunction>::value, "TFunction must provide operator()");
    static_assert(HasGetDeriv<TFunction>::value, "TFunction must provide GetDeriv()");
}

TEST(TFactoryCreateTest, CreatesAllBasicFunctions) {
    TFactory factory;

    TFunction ident = factory.Create("ident");
    TFunction constant = factory.Create("const", 7.0);
    TFunction power = factory.Create("power", 2);  // int -> double
    TFunction exponent = factory.Create("exp");
    TFunction polynomial = factory.Create("polynomial", {7, 0, 3, 15});

    EXPECT_DOUBLE_EQ(ident(3.0), 3.0);
    EXPECT_DOUBLE_EQ(constant(3.0), 7.0);
    EXPECT_DOUBLE_EQ(power(3.0), 9.0);
    EXPECT_DOUBLE_EQ(exponent(0.0), 1.0);
    EXPECT_DOUBLE_EQ(polynomial(10.0), 15307.0);

    EXPECT_EQ(ident.ToString(), "x");
    EXPECT_EQ(constant.ToString(), "7");
    EXPECT_EQ(power.ToString(), "x^2");
    EXPECT_EQ(exponent.ToString(), "exp(x)");
    EXPECT_EQ(polynomial.ToString(), "7 + 3*x^2 + 15*x^3");

    EXPECT_DOUBLE_EQ(ident.GetDeriv(3.0), 1.0);
    EXPECT_DOUBLE_EQ(constant.GetDeriv(3.0), 0.0);
    EXPECT_DOUBLE_EQ(power.GetDeriv(3.0), 6.0);
    EXPECT_DOUBLE_EQ(exponent.GetDeriv(0.0), 1.0);
    EXPECT_DOUBLE_EQ(polynomial.GetDeriv(1.0), 51.0);
}

TEST(TFactoryErrorTest, RejectsUnknownTypeAndWrongParameters) {
    TFactory factory;

    EXPECT_THROW(factory.Create("unknown"), std::logic_error);
    EXPECT_THROW(factory.Create("unknown", 1.0), std::logic_error);
    EXPECT_THROW(factory.Create("power"), std::logic_error);
    EXPECT_THROW(factory.Create("const"), std::logic_error);
    EXPECT_THROW(factory.Create("polynomial"), std::logic_error);
    EXPECT_THROW(factory.Create("ident", 1.0), std::logic_error);
    EXPECT_THROW(factory.Create("exp", 1.0), std::logic_error);
    EXPECT_THROW(factory.Create("polynomial", 3.0), std::logic_error);
    EXPECT_THROW(factory.Create("const", std::vector<double>{1.0}), std::logic_error);
}

TEST(TFactoryOperationsTest, ArithmeticOnFactoryObjects) {
    TFactory factory;

    TFunction power = factory.Create("power", 2.0);
    TFunction polynomial = factory.Create("polynomial", {7, 0, 3, 15});
    TFunction constant = factory.Create("const", 2.0);
    TFunction exponent = factory.Create("exp");

    TFunction sum = power + polynomial;
    EXPECT_EQ(sum.ToString(), "x^2 + 7 + 3*x^2 + 15*x^3");
    EXPECT_DOUBLE_EQ(sum(10.0), 15407.0);
    EXPECT_DOUBLE_EQ(sum.GetDeriv(1.0), 53.0);

    TFunction sub = power - polynomial;
    EXPECT_EQ(sub.ToString(), "x^2 - (7 + 3*x^2 + 15*x^3)");
    EXPECT_DOUBLE_EQ(sub(2.0), -135.0);
    EXPECT_DOUBLE_EQ(sub.GetDeriv(1.0), -49.0);

    TFunction mul = power * constant;
    EXPECT_EQ(mul.ToString(), "x^2 * 2");
    EXPECT_DOUBLE_EQ(mul(3.0), 18.0);
    EXPECT_DOUBLE_EQ(mul.GetDeriv(3.0), 12.0);

    TFunction div = power / constant;
    EXPECT_EQ(div.ToString(), "x^2 / 2");
    EXPECT_DOUBLE_EQ(div(4.0), 8.0);
    EXPECT_DOUBLE_EQ(div.GetDeriv(4.0), 4.0);
}

TEST(TFactoryOperationsTest, RecursiveExpressionFromFactoryObjects) {
    TFactory factory;

    TFunction polynomial = factory.Create("polynomial", {7, 0, 3, 15});
    TFunction exponent = factory.Create("exp");
    TFunction power = factory.Create("power", 2.0);
    TFunction constant = factory.Create("const", 2.0);

    TFunction expression = (polynomial + exponent) * (power / constant);

    EXPECT_EQ(expression.ToString(), "(7 + 3*x^2 + 15*x^3 + exp(x)) * (x^2 / 2)");

    const double x = 1.0;
    const double first = 7.0 + 3.0 * x * x + 15.0 * x * x * x + std::exp(x);
    const double firstDeriv = 6.0 * x + 45.0 * x * x + std::exp(x);
    const double second = x * x / 2.0;
    const double secondDeriv = x;

    EXPECT_NEAR(expression(x), first * second, 1e-9);
    EXPECT_NEAR(expression.GetDeriv(x), firstDeriv * second + first * secondDeriv, 1e-9);
}

TEST(TFactoryOperationsTest, RejectsNonTFunctionOperands) {
    TFactory factory;
    TFunction function = factory.Create("ident");

    EXPECT_THROW(function + "abc", std::logic_error);
    EXPECT_THROW("abc" * function, std::logic_error);
}

}  // namespace
