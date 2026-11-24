#include "base_funcs.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

namespace {

TEST(TFunctionCallTest, Identity) {
    TFunction function(std::make_shared<TIdentity>());
    EXPECT_DOUBLE_EQ(function(0.0), 0.0);
    EXPECT_DOUBLE_EQ(function(-2.5), -2.5);
}

TEST(TFunctionCallTest, Constant) {
    TFunction function(std::make_shared<TConstant>(-2.5));
    EXPECT_DOUBLE_EQ(function(0.0), -2.5);
    EXPECT_DOUBLE_EQ(function(100.0), -2.5);
}

TEST(TFunctionCallTest, Power) {
    TFunction function(std::make_shared<TPower>(2.0));
    EXPECT_DOUBLE_EQ(function(10.0), 100.0);
    EXPECT_DOUBLE_EQ(function(-3.0), 9.0);
}

TEST(TFunctionCallTest, Exp) {
    TFunction function(std::make_shared<TExp>());
    EXPECT_DOUBLE_EQ(function(0.0), 1.0);
    EXPECT_DOUBLE_EQ(function(1.0), std::exp(1.0));
}

TEST(TFunctionCallTest, PolynomialFromTask) {
    TFunction function(std::make_shared<TPolynomial>(std::vector<double>{7, 0, 3, 15}));
    EXPECT_DOUBLE_EQ(function(0.0), 7.0);
    EXPECT_DOUBLE_EQ(function(10.0), 15307.0);
}

TEST(TFunctionCallTest, PowerRejectsNegativeBaseWithFractionalExponent) {
    TFunction function(std::make_shared<TPower>(0.5));
    EXPECT_THROW(function(-1.0), std::logic_error);
}

TEST(TFunctionCallTest, MovedFromThrows) {
    TFunction source(std::make_shared<TIdentity>());
    TFunction moved(std::move(source));
    EXPECT_THROW(source(1.0), std::logic_error);
    EXPECT_DOUBLE_EQ(moved(1.0), 1.0);
}

TEST(TOperationCallTest, Sum) {
    auto lhs = std::make_shared<TPower>(2.0);
    auto rhs = std::make_shared<TConstant>(3.0);
    TFunction function(std::make_shared<TSum>(lhs, rhs));
    EXPECT_DOUBLE_EQ(function(2.0), 7.0);
}

TEST(TOperationCallTest, Sub) {
    auto lhs = std::make_shared<TPower>(2.0);
    auto rhs = std::make_shared<TConstant>(3.0);
    TFunction function(std::make_shared<TSub>(lhs, rhs));
    EXPECT_DOUBLE_EQ(function(2.0), 1.0);
}

TEST(TOperationCallTest, Mul) {
    auto lhs = std::make_shared<TPower>(2.0);
    auto rhs = std::make_shared<TPower>(3.0);
    TFunction function(std::make_shared<TMul>(lhs, rhs));
    EXPECT_DOUBLE_EQ(function(2.0), 32.0);
}

TEST(TOperationCallTest, Div) {
    auto lhs = std::make_shared<TPower>(2.0);
    auto rhs = std::make_shared<TConstant>(2.0);
    TFunction function(std::make_shared<TDiv>(lhs, rhs));
    EXPECT_DOUBLE_EQ(function(4.0), 8.0);
}

TEST(TOperationCallTest, DivByZeroThrows) {
    auto lhs = std::make_shared<TPower>(2.0);
    auto denominator = std::make_shared<TSub>(std::make_shared<TIdentity>(),
                                              std::make_shared<TConstant>(1.0));
    TFunction function(std::make_shared<TDiv>(lhs, denominator));
    EXPECT_THROW(function(1.0), std::logic_error);
}

TEST(TRecursiveCallTest, PolynomialPlusExpTimesPowerOverConstant) {
    TFunction polynomial(std::make_shared<TPolynomial>(std::vector<double>{7, 0, 3, 15}));
    TFunction exponent(std::make_shared<TExp>());
    TFunction power(std::make_shared<TPower>(2.0));
    TFunction constant(std::make_shared<TConstant>(2.0));

    TFunction expression = (polynomial + exponent) * (power / constant);

    const double x = 1.0;
    const double first = 7.0 + 3.0 * x * x + 15.0 * x * x * x + std::exp(x);
    const double second = x * x / 2.0;
    EXPECT_NEAR(expression(x), first * second, 1e-9);
}

TEST(TOperatorOverloadsTest, RejectNonTFunctionOperands) {
    TFunction function(std::make_shared<TIdentity>());
    EXPECT_THROW(function + "abc", std::logic_error);
    EXPECT_THROW("abc" - function, std::logic_error);
    EXPECT_THROW(function * 2.0, std::logic_error);
    EXPECT_THROW(2.0 / function, std::logic_error);
}

}  // namespace
