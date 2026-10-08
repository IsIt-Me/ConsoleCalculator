#include <QCoreApplication>
#include <QTextStream>
#include <QStringList>
#include <QFile>
#include "calculator.h"
// Функция для вывода списка доступных команд
void printHelp(QTextStream& out) {
    out << "Available commands:\n";
    out << " add <a> <b> - addition\n";
    out << " sub <a> <b> - subtraction\n";
    out << " mul <a> <b> - multiplication\n";
    out << " div <a> <b> - division\n";
    out << " fadd <a> <b> - fraction addition (e.g. fadd 1/2 1/3)\n";
    out << " fsub <a> <b> - fraction subtraction\n";
    out << " fmul <a> <b> - fraction multiplication\n";
    out << " fdiv <a> <b> - fraction division\n";
    out << " reset - reset calculator\n";
    out << " help - show this help\n";
    out << " quit - exit\n";
}
int main(int argc, char* argv[]) {
    // QCoreApplication используется вместо QApplication для консольного приложения
    QCoreApplication app(argc, argv);
    // Потоки ввода/вывода
    QTextStream in(stdin);
    QTextStream out(stdout);
    // Создаём калькулятор
    // Родитель не нужен, так как объект существует до конца программы
    Calculator calc;
    QFile historyFile("history.txt");
    historyFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream historyStream(&historyFile);
    QObject::connect(&calc, &Calculator::resultReady,
                     [&historyStream](double result) {
                         historyStream << "Result: " << result << "\n";
                         historyStream.flush();
                     });
    // История операций с дробями: записываем всю операцию, например "1/2 + 1/3 = 5/6"
    QObject::connect(&calc, &Calculator::fractionOperationPerformed,
                     [&historyStream](const QString& description) {
                         historyStream << "Fraction: " << description << "\n";
                         historyStream.flush();
                     });
    // СОЕДИНЕНИЯ: связываем сигналы калькулятора с обработчиками-лямбдами
    // 1. После успешного вычисления — выводим результат
    QObject::connect(&calc, &Calculator::resultReady,
                     [&out](double result) {
                         out << "Result: " << result << "\n";
                         out.flush();
                     });
    // 2. При возникновении ошибки — выводим сообщение об ошибке
    QObject::connect(&calc, &Calculator::errorOccurred,
                     [&out](const QString& msg) {
                         out << "Error: " << msg << "\n";
                         out.flush();
                     });
    // 3. После успешной операции с дробями — выводим операцию
    QObject::connect(&calc, &Calculator::fractionOperationPerformed,
                     [&out](const QString& description) {
                         out << description << "\n";
                         out.flush();
                     });
    // 4. После получения результата с дробями — выводим дробь и её десятичное значение
    QObject::connect(&calc, &Calculator::fractionResultReady,
                     [&out](const Fraction& result) {
                         out << "Fraction result: " << result.toString()
                         << " (" << result.toDouble() << ")\n";
                         out.flush();
                     });
    // Приветствие
    out << "=== Qt Console Calculator ===\n";
    printHelp(out);
    out << "\n> ";
    out.flush();
    // Основной цикл: читаем строки, разбираем команды и вызываем методы калькулятора
    QString line;
    while (in.readLineInto(&line)) {
        line = line.trimmed(); // Удаляем пробелы в начале и конце строки
        // Пустая строка — просто продолжаем работу
        if (line.isEmpty()) {
            out << "> ";
            out.flush();
            continue;
        }
        // Разбиваем введённую строку на токены по пробелам
        QStringList parts = line.split(' ', Qt::SkipEmptyParts);
        QString command = parts.value(0).toLower();
        // Обрабатываем команды выхода
        if (command == "quit" || command == "exit") {
            out << "Goodbye!\n";
            break;
        }
        // Справка
        if (command == "help") {
            printHelp(out);
            out << "> ";
            out.flush();
            continue;
        }
        // Сброс калькулятора
        if (command == "reset") {
            calc.reset();
            out << "> ";
            out.flush();
            continue;
        }
        // Команды работы с дробями: операнды передаются в виде строк (например, "3/4"),
        // разбор и обработка ошибок выполняются внутри калькулятора
        if (command == "fadd" || command == "fsub" || command == "fmul" || command == "fdiv") {
            if (parts.size() != 3) {
                out << "Error: invalid format. Use: " << command << " <a> <b>\n";
            }
            else if (command == "fadd") {
                calc.addFractions(parts[1], parts[2]);
            }
            else if (command == "fsub") {
                calc.subtractFractions(parts[1], parts[2]);
            }
            else if (command == "fmul") {
                calc.multiplyFractions(parts[1], parts[2]);
            }
            else {
                calc.divideFractions(parts[1], parts[2]);
            }
            out << "> ";
            out.flush();
            continue;
        }
        // Арифметические команды требуют 3 токена: команда + 2 числа
        if (parts.size() != 3) {
            out << "Error: invalid format. Use: <command> <a> <b>\n";
            out << "> ";
            out.flush();
            continue;
        }
        // Преобразуем операнды в числа
        bool ok1, ok2;
        double a = parts[1].toDouble(&ok1);
        double b = parts[2].toDouble(&ok2);
        if (!ok1 || !ok2) {
            out << "Error: failed to convert operands to numbers\n";
            out << "> ";
            out.flush();
            continue;
        }
        // Выполняем указанную команду
        // Используется обычный вызов метода; сигнал излучается внутри метода
        if (command == "add") {
            calc.add(a, b);
        }
        else if (command == "sub") {
            calc.subtract(a, b);
        }
        else if (command == "mul") {
            calc.multiply(a, b);
        }
        else if (command == "div") {
            calc.divide(a, b);
        }
        else {
            out << "Unknown command: " << command << "\n";
        }
        out << "> ";
        out.flush();
    }
    // QCoreApplication::exec() здесь не нужен.
    // Используется блокирующий цикл чтения вместо цикла обработки событий.
    // Если бы использовались QTimer, сетевое взаимодействие или другая
    // асинхронная функциональность, вызов app.exec() был бы необходим.
    return 0;
}