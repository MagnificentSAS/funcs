#include "base_funcs.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

namespace {

TEST(TFunctionGetDerivTest, Identity) {
    TFunction function(std::make_shared<TIdentity>());
    EXPECT_DOUBLE_EQ(function.GetDeriv(0.0), 1.0);
    EXPECT_DOUBLE_EQ(function.GetDeriv(-100.0), 1.0);
}

TEST(TFunctionGetDerivTest, Constant) {
    TFunction function(std::make_shared<TConstant>(7.0));
    EXPECT_DOUBLE_EQ(function.GetDeriv(42.0), 0.0);
}

TEST(TFunctionGetDerivTest, PowerAtPoint) {
    TFunction function(std::make_shared<TPower>(2.0));
    EXPECT_DOUBLE_EQ(function.GetDeriv(3.0), 6.0);
}

TEST(TFunctionGetDerivTest, PowerWithZeroExponent) {
    TFunction function(std::make_shared<TPower>(0.0));
    EXPECT_DOUBLE_EQ(function.GetDeriv(5.0), 0.0);
}

TEST(TFunctionGetDerivTest, PowerWithIntegerExponent) {
    TFunction function(std::make_shared<TPower>(3.0));
    EXPECT_DOUBLE_EQ(function.GetDeriv(2.0), 12.0);
}

TEST(TFunctionGetDerivTest, Exp) {
    TFunction function(std::make_shared<TExp>());
    EXPECT_DOUBLE_EQ(function.GetDeriv(0.0), 1.0);
    EXPECT_DOUBLE_EQ(function.GetDeriv(1.0), std::exp(1.0));
}

TEST(TFunctionGetDerivTest, PolynomialFromTask) {
    TFunction function(std::make_shared<TPolynomial>(std::vector<double>{7, 0, 3, 15}));
    EXPECT_DOUBLE_EQ(function.GetDeriv(1.0), 51.0);
    EXPECT_DOUBLE_EQ(function.GetDeriv(3.0), 423.0);
}

TEST(TFunctionGetDerivTest, PowerRejectsNegativeBaseWithFractionalExponent) {
    TFunction function(std::make_shared<TPower>(0.5));
    EXPECT_THROW(function.GetDeriv(-1.0), std::logic_error);
}

TEST(TFunctionGetDerivTest, MovedFromThrows) {
    TFunction source(std::make_shared<TIdentity>());
    TFunction moved(std::move(source));
    EXPECT_THROW(source.GetDeriv(1.0), std::logic_error);
    EXPECT_DOUBLE_EQ(moved.GetDeriv(1.0), 1.0);
}

TEST(TOperationGetDerivTest, Sum) {
    auto lhs = std::make_shared<TPower>(2.0);
    auto rhs = std::make_shared<TConstant>(3.0);
    TFunction function(std::make_shared<TSum>(lhs, rhs));
    EXPECT_DOUBLE_EQ(function.GetDeriv(2.0), 4.0);
}

TEST(TOperationGetDerivTest, Sub) {
    auto lhs = std::make_shared<TPower>(2.0);
    auto rhs = std::make_shared<TConstant>(3.0);
    TFunction function(std::make_shared<TSub>(lhs, rhs));
    EXPECT_DOUBLE_EQ(function.GetDeriv(2.0), 4.0);
}

TEST(TOperationGetDerivTest, Mul) {
    auto lhs = std::make_shared<TPower>(2.0);
    auto rhs = std::make_shared<TPower>(3.0);
    TFunction function(std::make_shared<TMul>(lhs, rhs));
    EXPECT_DOUBLE_EQ(function.GetDeriv(2.0), 80.0);
}

TEST(TOperationGetDerivTest, Div) {
    auto lhs = std::make_shared<TPower>(2.0);
    auto rhs = std::make_shared<TConstant>(2.0);
    TFunction function(std::make_shared<TDiv>(lhs, rhs));
    EXPECT_DOUBLE_EQ(function.GetDeriv(4.0), 4.0);
}

TEST(TOperationGetDerivTest, DivByZeroThrows) {
    auto lhs = std::make_shared<TPower>(2.0);
    auto denominator = std::make_shared<TSub>(std::make_shared<TIdentity>(),
                                              std::make_shared<TConstant>(1.0));
    TFunction function(std::make_shared<TDiv>(lhs, denominator));
    EXPECT_THROW(function.GetDeriv(1.0), std::logic_error);
}

TEST(TRecursiveGetDerivTest, PolynomialPlusExpTimesPowerOverConstant) {
    TFunction polynomial(std::make_shared<TPolynomial>(std::vector<double>{7, 0, 3, 15}));
    TFunction exponent(std::make_shared<TExp>());
    TFunction power(std::make_shared<TPower>(2.0));
    TFunction constant(std::make_shared<TConstant>(2.0));

    TFunction expression = (polynomial + exponent) * (power / constant);

    const double x = 1.0;
    const double first = 7.0 + 3.0 * x * x + 15.0 * x * x * x + std::exp(x);
    const double firstDeriv = 6.0 * x + 45.0 * x * x + std::exp(x);
    const double second = x * x / 2.0;
    const double secondDeriv = x;
    EXPECT_NEAR(expression.GetDeriv(x), firstDeriv * second + first * secondDeriv, 1e-9);
}

}  // namespace
