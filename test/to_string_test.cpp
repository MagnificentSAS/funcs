#include "base_funcs.hpp"

#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

namespace {

TEST(TFunctionToStringTest, Identity) {
    TFunction function(std::make_shared<TIdentity>());
    EXPECT_EQ(function.ToString(), "x");
}

TEST(TFunctionToStringTest, Constant) {
    TFunction function(std::make_shared<TConstant>(7.0));
    EXPECT_EQ(function.ToString(), "7");
}

TEST(TFunctionToStringTest, NegativeConstant) {
    TFunction function(std::make_shared<TConstant>(-2.5));
    EXPECT_EQ(function.ToString(), "-2.5");
}

TEST(TFunctionToStringTest, Power) {
    TFunction function(std::make_shared<TPower>(2.0));
    EXPECT_EQ(function.ToString(), "x^2");
}

TEST(TFunctionToStringTest, Exp) {
    TFunction function(std::make_shared<TExp>());
    EXPECT_EQ(function.ToString(), "exp(x)");
}

TEST(TFunctionToStringTest, PolynomialFromTask) {
    TFunction function(std::make_shared<TPolynomial>(std::vector<double>{7, 0, 3, 15}));
    EXPECT_EQ(function.ToString(), "7 + 3*x^2 + 15*x^3");
}

TEST(TFunctionToStringTest, PolynomialWithNegativeCoefficient) {
    TFunction function(std::make_shared<TPolynomial>(std::vector<double>{0, 1, -1}));
    EXPECT_EQ(function.ToString(), "x - x^2");
}

TEST(TFunctionToStringTest, EmptyPolynomial) {
    TFunction function(std::make_shared<TPolynomial>(std::vector<double>{}));
    EXPECT_EQ(function.ToString(), "0");
}

TEST(TFunctionToStringTest, MovedFromThrows) {
    TFunction source(std::make_shared<TIdentity>());
    TFunction moved(std::move(source));
    EXPECT_THROW(source.ToString(), std::logic_error);
    EXPECT_EQ(moved.ToString(), "x");
}

TEST(TOperationToStringTest, Sum) {
    auto lhs = std::make_shared<TPower>(2.0);
    auto rhs = std::make_shared<TConstant>(3.0);
    TFunction function(std::make_shared<TSum>(lhs, rhs));
    EXPECT_EQ(function.ToString(), "x^2 + 3");
}

TEST(TOperationToStringTest, Sub) {
    auto lhs = std::make_shared<TPower>(2.0);
    auto rhs = std::make_shared<TConstant>(3.0);
    TFunction function(std::make_shared<TSub>(lhs, rhs));
    EXPECT_EQ(function.ToString(), "x^2 - 3");
}

TEST(TOperationToStringTest, SubWithPolynomialRightOperand) {
    auto lhs = std::make_shared<TPower>(2.0);
    auto rhs = std::make_shared<TPolynomial>(std::vector<double>{1, 1});
    TFunction function(std::make_shared<TSub>(lhs, rhs));
    EXPECT_EQ(function.ToString(), "x^2 - (1 + x)");
}

TEST(TOperationToStringTest, SubWithOperationRightOperand) {
    auto lhs = std::make_shared<TPower>(2.0);
    auto rhs = std::make_shared<TSum>(std::make_shared<TIdentity>(),
                                      std::make_shared<TConstant>(1.0));
    TFunction function(std::make_shared<TSub>(lhs, rhs));
    EXPECT_EQ(function.ToString(), "x^2 - (x + 1)");
}

TEST(TOperationToStringTest, Mul) {
    auto lhs = std::make_shared<TPower>(2.0);
    auto rhs = std::make_shared<TConstant>(3.0);
    TFunction function(std::make_shared<TMul>(lhs, rhs));
    EXPECT_EQ(function.ToString(), "x^2 * 3");
}

TEST(TOperationToStringTest, MulWithPolynomialLeftOperand) {
    auto lhs = std::make_shared<TPolynomial>(std::vector<double>{1, 1});
    auto rhs = std::make_shared<TPower>(2.0);
    TFunction function(std::make_shared<TMul>(lhs, rhs));
    EXPECT_EQ(function.ToString(), "(1 + x) * x^2");
}

TEST(TOperationToStringTest, MulWithOperationLeftOperand) {
    auto lhs = std::make_shared<TSum>(std::make_shared<TIdentity>(),
                                      std::make_shared<TConstant>(1.0));
    auto rhs = std::make_shared<TPower>(2.0);
    TFunction function(std::make_shared<TMul>(lhs, rhs));
    EXPECT_EQ(function.ToString(), "(x + 1) * x^2");
}

TEST(TOperationToStringTest, Div) {
    auto lhs = std::make_shared<TPower>(2.0);
    auto rhs = std::make_shared<TConstant>(3.0);
    TFunction function(std::make_shared<TDiv>(lhs, rhs));
    EXPECT_EQ(function.ToString(), "x^2 / 3");
}

TEST(TOperationToStringTest, DivWithPolynomialLeftOperand) {
    auto lhs = std::make_shared<TPolynomial>(std::vector<double>{1, 1});
    auto rhs = std::make_shared<TPower>(2.0);
    TFunction function(std::make_shared<TDiv>(lhs, rhs));
    EXPECT_EQ(function.ToString(), "(1 + x) / x^2");
}

TEST(TOperationToStringTest, DivWithOperationRightOperand) {
    auto lhs = std::make_shared<TPower>(2.0);
    auto rhs = std::make_shared<TSum>(std::make_shared<TIdentity>(),
                                      std::make_shared<TConstant>(1.0));
    TFunction function(std::make_shared<TDiv>(lhs, rhs));
    EXPECT_EQ(function.ToString(), "x^2 / (x + 1)");
}

TEST(TRecursiveToStringTest, PolynomialPlusExpTimesPowerOverConstant) {
    TFunction polynomial(std::make_shared<TPolynomial>(std::vector<double>{7, 0, 3, 15}));
    TFunction exponent(std::make_shared<TExp>());
    TFunction power(std::make_shared<TPower>(2.0));
    TFunction constant(std::make_shared<TConstant>(2.0));

    TFunction expression = (polynomial + exponent) * (power / constant);

    EXPECT_EQ(expression.ToString(),
              "(7 + 3*x^2 + 15*x^3 + exp(x)) * (x^2 / 2)");
}

}  // namespace
