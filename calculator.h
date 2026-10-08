#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QObject>
#include <QString>
#include <optional>
#include "fraction.h"

class Calculator : public QObject {
    Q_OBJECT // Обязательный макрос для работы метасистемы
public:
    explicit Calculator(QObject *parent = nullptr);
    // Геттеры для текущего состояния
    double result() const { return m_result; }
    Fraction fractionResult() const { return m_fractionResult; }
    bool hasError() const { return m_hasError; }
    QString errorMessage() const { return m_errorMessage; }
public slots:
    // Слоты для арифметических операций
    void add(double a, double b);
    void subtract(double a, double b);
    void multiply(double a, double b);
    void divide(double a, double b);
    // Слоты для операций над дробями. Операнды передаются строками
    // ("3/4", "-2", "5/10"), поэтому ошибки разбора обрабатываются здесь же
    // и уходят в общий сигнал errorOccurred.
    void addFractions(const QString &a, const QString &b);
    void subtractFractions(const QString &a, const QString &b);
    void multiplyFractions(const QString &a, const QString &b);
    void divideFractions(const QString &a, const QString &b);
    // Слот для сброса состояния калькулятора
    void reset();
signals:
    // Сигнал, излучаемый после успешного вычисления
    void resultReady(double result);
    // Сигнал, излучаемый после успешной операции над дробями
    void fractionResultReady(const Fraction &result);
    // Описание выполненной операции над дробями, например "1/2 + 1/3 = 5/6"
    void fractionOperationPerformed(const QString &description);
    // Сигнал, излучаемый при ошибке (общий для обычных операций и дробей)
    void errorOccurred(const QString &message);
private:
    using FractionOp = std::optional<Fraction> (*)(const Fraction &, const Fraction &, QString *);

    double m_result; // Последний результат вычисления
    Fraction m_fractionResult; // Последний результат операции над дробями
    bool m_hasError; // Флаг ошибки
    QString m_errorMessage; // Текст последней ошибки
    // Вспомогательный метод для установки результата
    void setResult(double value);
    // Вспомогательные методы для дробей
    void fractionOperation(const QString &symbol, const QString &a, const QString &b, FractionOp op);
    bool parseFraction(const QString &text, Fraction &out);
    void setError(const QString &message);
};

#endif // CALCULATOR_H