#include "calculator.h"
#include <QDebug>
// Конструктор: инициализируем начальное состояние
Calculator::Calculator(QObject* parent)
    : QObject(parent) // Передаём parent в базовый класс
    , m_result(0.0) // Начальный результат
    , m_hasError(false) // Ошибок нет
{
    qRegisterMetaType<Fraction>("Fraction"); // Fraction можно передавать в сигналах
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
        m_errorMessage = "Деление на ноль невозможно!";
        emit errorOccurred(m_errorMessage); // Сообщаем об ошибке
        return; // Прерываем выполнение
    }
    setResult(a / b);
}
// Приватный метод: фиксирует ошибку и излучает общий сигнал errorOccurred
void Calculator::setError(const QString &message) {
    m_hasError = true;
    m_errorMessage = message;
    emit errorOccurred(m_errorMessage);
}
// Приватный метод: разбирает дробь из строки, при ошибке сообщает через errorOccurred
bool Calculator::parseFraction(const QString &text, Fraction &out) {
    QString err;
    const auto f = Fraction::parse(text, &err);
    if (!f) {
        setError(err);
        return false;
    }
    out = *f;
    return true;
}
// Приватный метод: общая логика всех операций над дробями
void Calculator::fractionOperation(const QString &symbol, const QString &a,
                                   const QString &b, FractionOp op) {
    Fraction fa, fb;
    if (!parseFraction(a, fa) || !parseFraction(b, fb))
        return; // Ошибка уже отправлена сигналом errorOccurred
    QString err;
    const auto r = op(fa, fb, &err);
    if (!r) { // Деление на нулевую дробь, переполнение и т.п.
        setError(err);
        return;
    }
    m_fractionResult = *r; // Сохраняем результат
    m_result = r->toDouble(); // Числовой результат остаётся согласованным (сигнал не излучаем)
    m_hasError = false; // Сбрасываем флаг ошибки
    m_errorMessage.clear();
    emit fractionOperationPerformed(QStringLiteral("%1 %2 %3 = %4")
                                        .arg(fa.toString(), symbol, fb.toString(), r->toString()));
    emit fractionResultReady(m_fractionResult); // Оповещаем всех подписчиков
}
// Слот: сложение дробей
void Calculator::addFractions(const QString &a, const QString &b) {
    qDebug() << "addFractions(" << a << "," << b << ")";
    fractionOperation("+", a, b, &Fraction::add);
}
// Слот: вычитание дробей
void Calculator::subtractFractions(const QString &a, const QString &b) {
    qDebug() << "subtractFractions(" << a << "," << b << ")";
    fractionOperation("-", a, b, &Fraction::subtract);
}
// Слот: умножение дробей
void Calculator::multiplyFractions(const QString &a, const QString &b) {
    qDebug() << "multiplyFractions(" << a << "," << b << ")";
    fractionOperation("*", a, b, &Fraction::multiply);
}
// Слот: деление дробей (с проверкой на нулевую дробь!)
void Calculator::divideFractions(const QString &a, const QString &b) {
    qDebug() << "divideFractions(" << a << "," << b << ")";
    fractionOperation("/", a, b, &Fraction::divide);
}
// Слот: сброс состояния
void Calculator::reset() {
    qDebug() << "reset()";
    m_result = 0.0;
    m_fractionResult = Fraction();
    m_hasError = false;
    m_errorMessage.clear();
    emit resultReady(m_result);
}