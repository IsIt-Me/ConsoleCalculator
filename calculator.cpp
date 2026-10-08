#include "calculator.h"
#include <QDebug>
// Конструктор: инициализируем начальное состояние
Calculator::Calculator(QObject* parent)
    : QObject(parent) // Передаём parent в базовый класс
    , m_result(0.0) // Начальный результат
    , m_hasError(false) // Ошибок нет
{
    qDebug() << "Calculator created";
}
// Приватный метод: устанавливает результат и излучает сигнал
void Calculator::setResult(double value) {
    m_result = value; // Сохраняем результат
    m_hasError = false; // Сбрасываем флаг ошибки
    m_errorMessage.clear();
    emit resultReady(m_result); // Оповещаем всех подписчиков
}
// Слот: сложение
void Calculator::add(double a, double b) {
    qDebug() << "add(" << a << "," << b << ")";
    setResult(a + b);
}
// Слот: вычитание
void Calculator::subtract(double a, double b) {
    qDebug() << "subtract(" << a << "," << b << ")";
    setResult(a - b);
}
// Слот: умножение
void Calculator::multiply(double a, double b) {
    qDebug() << "multiply(" << a << "," << b << ")";
    setResult(a * b);
}
// Слот: деление (с проверкой на ноль!)
void Calculator::divide(double a, double b) {
    qDebug() << "divide(" << a << "," << b << ")";
    // Критически важная проверка: деление на ноль недопустимо
    if (qFuzzyIsNull(b)) { // qFuzzyIsNull корректно сравнивает double с нулём
        m_hasError = true;
        m_errorMessage = "Division by zero is impossible!";
        emit errorOccurred(m_errorMessage); // Сообщаем об ошибке
        return; // Прерываем выполнение
    }
    setResult(a / b);
}
// Слот: сброс состояния
void Calculator::reset() {
    qDebug() << "reset()";
    m_result = 0.0;
    m_hasError = false;
    m_errorMessage.clear();
    emit resultReady(m_result);
}