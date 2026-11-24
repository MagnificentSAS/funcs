#include "base_funcs.hpp"

#include <gtest/gtest.h>

#include <memory>

namespace {

// ШАБЛОН ТЕСТА ДЛЯ НОВОЙ БАЗОВОЙ ФУНКЦИИ.
// 1. Замени TPower на имя нового класса из include/base_funcs.hpp.
// 2. Передай в конструктор нужные параметры новой функции.
// 3. Подставь ожидаемую строку, производную и значение в точке.
// Сейчас используется TPower как рабочий пример: файл компилируется и проходит.

TEST(TTemplateToStringTest, ReturnsExpectedString) {
    TFunction function(std::make_shared<TPower>(2.0));  // TODO: новый класс
    EXPECT_EQ(function.ToString(), "x^2");              // TODO: ожидаемая строка
}

TEST(TTemplateGetDerivTest, ReturnsExpectedDerivative) {
    TFunction function(std::make_shared<TPower>(2.0));  // TODO: новый класс
    EXPECT_DOUBLE_EQ(function.GetDeriv(3.0), 6.0);      // TODO: ожидаемая производная
}

TEST(TTemplateCallTest, ReturnsExpectedValue) {
    TFunction function(std::make_shared<TPower>(2.0));  // TODO: новый класс
    EXPECT_DOUBLE_EQ(function(10.0), 100.0);            // TODO: ожидаемое значение
}

}  // namespace
