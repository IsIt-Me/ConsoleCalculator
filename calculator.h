#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QObject>
#include <QString>
class Calculator : public QObject {
    Q_OBJECT // Обязательный макрос для работы метасистемы
public:
    explicit Calculator(QObject *parent = nullptr);
    // Геттеры для текущего состояния
    double result() const { return m_result; }
    bool hasError() const { return m_hasError; }
    QString errorMessage() const { return m_errorMessage; }
public slots:
    // Слоты для арифметических операций
    void add(double a, double b);
    void subtract(double a, double b);
    void multiply(double a, double b);
    void divide(double a, double b);
    // Слот для сброса состояния калькулятора
    void reset();
signals:
    // Сигнал, излучаемый после успешного вычисления
    void resultReady(double result);
    // Сигнал, излучаемый при ошибке
    void errorOccurred(const QString &message);
private:
    double m_result; // Последний результат вычисления
    bool m_hasError; // Флаг ошибки
    QString m_errorMessage; // Текст последней ошибки
    // Вспомогательный метод для установки результата
    void setResult(double value);
};

#endif // CALCULATOR_H