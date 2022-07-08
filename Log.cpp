#include "Log.h"

Log::Config Log::c;
std::ofstream Log::file;
tm Log::timeInfo;
int Log::numSection = 0;

//============================================================//

void Log::close() {
    Log::info() << "Completion of program execution";
    file.close();
}

//============================================================//

void Log::config(void (*config)(Config& self)) {
    config(Log::c);

    Log::c.hstd = GetStdHandle(STD_OUTPUT_HANDLE);

    time_t t = time(NULL);
    localtime_s(&Log::timeInfo, &t);

    int min = INT16_MAX, max = 0;
    int numFiles = 0;
    for (auto f : std::filesystem::directory_iterator(Log::c.folderToSave)) {
        std::string path = f.path().string();
        if (path.substr(path.size() - 4, path.size()) == ".log") {
            int i = atoi(f.path().filename().string().substr(0, path.size() - 4).c_str());

            if (i < min) min = i;
            if (i > max) max = i;

            numFiles++;
        }
    }

    if (numFiles >= 10) {
        remove((
                Log::c.folderToSave +
                std::to_string(min) + ".log"
            ).c_str()
        );
    }

    Log::file.open(
        Log::c.folderToSave +
        std::to_string(max + 1) + ".log"
    );

    if (file.is_open()) {
        atexit(Log::close);
    } else if (!Log::c.folderToSave.empty()) {
        Log::c.folderToSave = "";
    }

    Log::info() << "Logger started";
}

Log::Message Log::write(std::string tag) {
    return Log::Message(tag);
}

Log::Message Log::info() {
    return write("INFO");
}

Log::Message Log::debug() {
    return write("DEBUG");
}

Log::Message Log::warning() {
    return write("WARNING");
}

Log::Message Log::error() {
    return write("ERROR");
}


Log::Message Log::begin() {
    return write("BEGIN");
}

Log::Message Log::end() {
    return write("END");
}

//============================================================//

Log::Message::Message(const Message& copy) {
    this->last = true;
    this->message << copy.message.str();
    this->color = copy.color;
    this->tag = copy.tag;
}

Log::Message::Message(std::string tag) {
    this->tag = tag;
    this->last = true;

    if (c.colors.find(tag) != c.colors.end())
        this->color = c.colors[tag];
    else if (tag == "BEGIN" || tag == "END")
        this->color = c.colors["SECTION"];
    else this->color = 7;
}

Log::Message::~Message() {
    if (!last) return;

    bool begin = (tag == "BEGIN");

    Log::numSection -= (tag == "END");
    Log::numSection = Log::numSection < 0 ? 0 : Log::numSection;

    if (tag == "BEGIN" || tag == "END")
        tag = "SECTION";

    if (!c.filters.empty() && std::find(c.filters.begin(), c.filters.end(), tag) == c.filters.end() && tag != "SECTION")
        return;  

    std::stringstream ss;
    ss << std::string(Log::numSection * Log::c.numSpace, ' ') << "[" << clock() << "] " << "{" << tag << "} " << message.str() << "\n";

    if (Log::c.outConsole) {
        SetConsoleTextAttribute(c.hstd, (WORD)color);
        printf(ss.str().c_str());
        SetConsoleTextAttribute(c.hstd, (WORD)4);
    }

    if (!Log::c.folderToSave.empty()) {
        Log::file << color << ss.str();
        Log::file.flush();
    }

    Log::numSection += begin;
}