#ifndef FRACTION_H
#define FRACTION_H

#include <QMetaType>
#include <QString>
#include <optional>

// Дробь: числитель/знаменатель. Всегда хранится в несократимом виде,
// знаменатель строго положителен, знак находится в числителе.
// Объект создаётся только через create()/parse(), поэтому
// дробь с нулевым знаменателем существовать не может.
class Fraction {
public:
    Fraction() = default; // 0/1
    // Фабрики: при ошибке возвращают nullopt и (если err != nullptr) пишут причину
    static std::optional<Fraction> create(qint64 numerator, qint64 denominator,
                                          QString *err = nullptr);
    static std::optional<Fraction> parse(const QString &text, QString *err = nullptr);
    // Арифметика с проверкой переполнения
    static std::optional<Fraction> add(const Fraction &a, const Fraction &b, QString *err = nullptr);
    static std::optional<Fraction> subtract(const Fraction &a, const Fraction &b, QString *err = nullptr);
    static std::optional<Fraction> multiply(const Fraction &a, const Fraction &b, QString *err = nullptr);
    static std::optional<Fraction> divide(const Fraction &a, const Fraction &b, QString *err = nullptr);
    qint64 numerator() const { return m_num; }
    qint64 denominator() const { return m_den; }
    double toDouble() const { return static_cast<double>(m_num) / static_cast<double>(m_den); }
    QString toString() const; // "3/4" или "5", если знаменатель равен 1
    bool operator==(const Fraction &o) const { return m_num == o.m_num && m_den == o.m_den; }
private:
    Fraction(qint64 n, qint64 d) : m_num(n), m_den(d) {}
    qint64 m_num = 0;
    qint64 m_den = 1;
};
// Нужно, чтобы Fraction можно было передавать в сигналах (в т.ч. через очередь)
Q_DECLARE_METATYPE(Fraction)

#endif // FRACTION_H