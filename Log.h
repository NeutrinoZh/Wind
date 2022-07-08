#pragma once
#include "utils.h"

class Log {
private:

    struct Config {
        bool outConsole = false;
        std::string folderToSave = "";
        int numSpace = 1;
        HANDLE hstd;

        std::map<std::string, UINT8> colors = {
            { "ERROR", 4 },
            { "SECTION", 2 },
            { "WARNING", 3 },
            { "DEBUG", 1 },
            { "INFO", 7 },
        };

        std::vector<std::string> filters = {};
    };

private:

    static Config c;

    static std::ofstream file;
    static tm timeInfo;
    static int numSection;

    static void close();

public:

    static void config(void (*config)(Config& self));

public:

    class Message {
    private:
        bool last;
        std::stringstream message;

        std::string tag;
        UINT8 color;
    public:
        ~Message();
        Message(const Message& copy);
        Message(std::string tag);

        template <typename T>
        Message operator<<(T value) {
            this->last = false;
            this->message << value;
            return *this;
        }

        Message operator<<(bool value) {
            this->last = false;
            this->message << (value ? "true" : "false");
            return *this;
        }
    };

public:

    static Message write(std::string tag);

    static Message info();
    static Message debug();
    static Message warning();
    static Message error();

    static Message begin();
    static Message end();

};