#ifndef EXECUTOR_H
#define EXECUTOR_H
#include <atomic>
#include <map>
#include <string>
#include <thread>


// это не просто класс это творение ума. Это воплощение красоты языка.

/*
 * позоляет начать выполнять команду bash в отдельном потоке и завершить ее выполнение в любоее время по id
 * ползволяет получить id последней выпоненной и полностью законченной команды
 * за маленьким интерфейсом скрываеться красота.
 * пример работы;
 * Executop exec(time(0));
 * exec.startExecute("echo 1");
 * sleep(1);
 * exec.getLastExecutedCommand();
 */
// class Command{
// public:
//     Command(std::string command); // выделяет мультипроцессорную sharing память под bool
//     void execute(); // начинает выполнение
//     ~Command(); // освобождет память
//     std::string mCommand;
//     bool  * pState = nullptr;
//     bool isDone() const;
// private:
//     std::string _shmName;
//     std::string generateSharedMemoryName();// sharing память имеет инлефикатор. Генерирует его
//     std::atomic<bool> _completed{false};
// };
// class ExecutingCommand
// {
// public:
//     ExecutingCommand(Command * command, std::thread * thread); // этот класс своеобразная структура. Сохраняет значения
//     ExecutingCommand(); // пустой конструктор нужен видимо для map

//     ~ExecutingCommand(); //
//     Command * mCommand;
//     std::thread * mThread;
//     void stop(); // остановка выполнения (kill)
// }; это все стало не нужно  после рефакторинга


class Executor
{
public:
    Executor(int seed); // устонавливает seed для генерации индификаторов sharing памяти
    ~Executor(); // завершает все команды и потоки
    void startExecute(std::string command, int id); // запускает выполнение команды в отдельном потоке
    int getFirstExecutedId(); // возвращает id первой выполненной до конца команды
    bool stopExecuting(int id, bool isIterator = false); // завершает команлу и ждет ее завершения (блокировка)
    bool isDone(int id);
private:
    std::map< int , int> _commandList;
};

#endif // EXECUTOR_H
