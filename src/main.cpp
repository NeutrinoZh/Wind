#include "Log/Log.h"

int main(int countArg, char* args[]) {
    system("read -N 1 -p 'Press any key to continue...\n'");

    Log::config([](auto& self) {
        self.outConsole = true;
        self.folderToSave = "./logs/";
        self.numSpace = 2;
        self.filters = { "INFO" };
    });

    Log::warning() << "Warning";
    Log::error() << false;
    Log::info() << true;
    Log::begin() << "Start section";
    Log::info() << "Hello";
    Log::begin() << "And more section";
    Log::debug() << "Haha";
    Log::end() << "End hello section";
    Log::info() << "...";
    Log::end() << "End section";
    Log::info() << "End program";

    system("read -N 1 -p 'Press any key to continue...'");

    return EXIT_SUCCESS;
}