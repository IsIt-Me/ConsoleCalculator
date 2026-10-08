#include "fraction.h"

#include <QStringList>
#include <QtNumeric>
#include <limits>

namespace {
qint64 gcd64(qint64 a, qint64 b) // a, b >= 0
{
    while (b != 0) {
        qint64 t = a % b;
        a = b;
        b = t;
    }
    return a;
}
qint64 abs64(qint64 v) { return v < 0 ? -v : v; }
std::nullopt_t fail(QString *err, const QString &msg)
{
    if (err)
        *err = msg;
    return std::nullopt;
}
QString overflowMessage()
{
    return QStringLiteral("Переполнение: числа слишком велики!");
}
} // namespace
std::optional<Fraction> Fraction::create(qint64 n, qint64 d, QString *err)
{
    if (d == 0)
        return fail(err, QStringLiteral("Знаменатель не может быть равен нулю!"));

    constexpr qint64 kMin = std::numeric_limits<qint64>::min();
    if (n == kMin || d == kMin) // у INT64_MIN нет противоположного числа
        return fail(err, overflowMessage());

    if (d < 0) {
        n = -n;
        d = -d;
    }
    const qint64 g = gcd64(abs64(n), d); // d > 0, значит g >= 1
    return Fraction(n / g, d / g);
}
std::optional<Fraction> Fraction::parse(const QString &text, QString *err)
{
    const QString t = text.trimmed();
    const QStringList p = t.split('/');
    if (t.isEmpty() || p.size() > 2)
        return fail(err, QStringLiteral("Некорректная дробь: '%1'").arg(text));

    bool ok1 = false;
    bool ok2 = true;
    const qint64 n = p[0].trimmed().toLongLong(&ok1);
    qint64 d = 1;
    if (p.size() == 2)
        d = p[1].trimmed().toLongLong(&ok2);

    if (!ok1 || !ok2)
        return fail(err, QStringLiteral("Некорректная дробь: '%1'").arg(text));
    return create(n, d, err);
}
std::optional<Fraction> Fraction::add(const Fraction &a, const Fraction &b, QString *err)
{
    // Через НОК знаменателей, чтобы реже переполняться
    const qint64 g = gcd64(a.m_den, b.m_den);
    const qint64 ad = a.m_den / g;
    const qint64 bd = b.m_den / g;
    qint64 x, y, n, d;
    if (qMulOverflow(a.m_num, bd, &x) || qMulOverflow(b.m_num, ad, &y)
        || qAddOverflow(x, y, &n) || qMulOverflow(ad, b.m_den, &d))
        return fail(err, overflowMessage());
    return create(n, d, err);
}
std::optional<Fraction> Fraction::subtract(const Fraction &a, const Fraction &b, QString *err)
{
    // create() гарантирует, что числитель != INT64_MIN, поэтому отрицание безопасно
    return add(a, Fraction(-b.m_num, b.m_den), err);
}
std::optional<Fraction> Fraction::multiply(const Fraction &a, const Fraction &b, QString *err)
{
    // Перекрёстное сокращение перед умножением
    const qint64 g1 = gcd64(abs64(a.m_num), b.m_den);
    const qint64 g2 = gcd64(abs64(b.m_num), a.m_den);
    qint64 n, d;
    if (qMulOverflow(a.m_num / g1, b.m_num / g2, &n)
        || qMulOverflow(a.m_den / g2, b.m_den / g1, &d))
        return fail(err, overflowMessage());
    return create(n, d, err);
}
std::optional<Fraction> Fraction::divide(const Fraction &a, const Fraction &b, QString *err)
{
    if (b.m_num == 0)
        return fail(err, QStringLiteral("Деление на нулевую дробь невозможно!"));
    // Обратная дробь; знак переносим в числитель
    const Fraction inv = b.m_num > 0 ? Fraction(b.m_den, b.m_num)
                                     : Fraction(-b.m_den, -b.m_num);
    return multiply(a, inv, err);
}
QString Fraction::toString() const
{
    return m_den == 1 ? QString::number(m_num)
                      : QStringLiteral("%1/%2").arg(m_num).arg(m_den);
}