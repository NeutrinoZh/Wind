#include "Log/Log.h"

namespace wd {
    
    tm Log::tInfo;
    bool Log::outConsole = true;
    bool Log::outFile = true;
    std::string Log::folder = "log\\";
    std::string Log::path;
    std::ofstream Log::file;
    std::map<std::string, UINT8> Log::colors;
    HANDLE Log::hstd;
    std::vector<std::string> Log::filters;

    std::string Log::getDate() {
        return  std::to_string(tInfo.tm_mday) + "." +
                std::to_string(tInfo.tm_mon + 1) + "." +
                std::to_string(tInfo.tm_year + 1900) + " (" +
                std::to_string(tInfo.tm_hour) + "." +
                std::to_string(tInfo.tm_min) + "." +
                std::to_string(tInfo.tm_sec) + ")";
    }

     void Log::close() {
        SetConsoleTextAttribute(hstd, 15);
        file.close();
    }

    void Log::setFilters(std::vector<std::string> _filters) {
        filters = _filters;

        file.close();
        system("clear"); 
    
        load(path + ".txt");

        if (outFile) {
            file.open(path + "-filter.txt");
            if (!file.is_open()) {
                outFile = false;
                std::cout << "Failed to open file to write logs:" << path << "\n";
            }
        }
    }

    void Log::addColor(std::string tag, UINT8 color) {
        colors.insert(std::make_pair(tag, color));
    }

    void Log::load(std::string path) {
        std::ifstream read(path);
        if (read.is_open()) {
            std::string line = "";
            while (std::getline(read, line)) {
                line += "\n";
                
                std::string tag = "";
                try {
                    size_t begin = line.find("{") + 2,
                        end   = line.find("}") - 2;

                    for (size_t i = begin; i <= end; ++i)
                        tag += line[i];
                } catch (...) {
                    std::cout << "The download was interrupted because the log file is damaged:" << path << "\n";
                    break;
                }

                if (!filters.empty() && std::find(filters.begin(), filters.end(), tag) == filters.end())
                    continue;

                if (outFile && file.is_open())
                    file << line;

                if (outConsole) {
                    SetConsoleTextAttribute(hstd, (WORD) line[0]);
                    line.erase(line.begin());
                    printf(line.c_str());
                }
            }

            if (outFile)
                file.flush();
            read.close();
        } else
            std::cout << "Failed to open file to read the log:" << path << "\n";
    }

    void Log::clear() {
        try {
            for (auto f : std::filesystem::directory_iterator(folder))
                remove(f.path().string().c_str()); 
        } catch (std::filesystem::filesystem_error& ex) {
            std::cout << "This directory cannot be cleared: " << ex.what() << "\n";
        } catch (...) {
            std::cout << "Unknown error while cleaning directory\n";
        }
    }

    void Log::init() {
        system("clear"); 
        
        time_t t = time(NULL);
        tInfo = *localtime(&t);

        hstd = GetStdHandle(STD_OUTPUT_HANDLE);

        addColor("INFO", 7);
        addColor("DEBUG", 8);
        addColor("WARRING", 14);
        addColor("ERROR", 4);

        path = folder + getDate();
        
        file.open(path + ".txt");
        if (file.is_open()) {
            atexit(close);
        } else if (outFile) {
            outFile = false;
            std::cout << "Failed to create file to write logs: " << path << "\n";
        }
    }

    Log::Message::Message(const Log::Message& copy) {
        this->message << copy.message.str();
        this->tag = copy.tag;
        this->code = copy.code;
        this->color = copy.color;
    }

    Log::Message::~Message() {
        if (!last) return;

        if (!filters.empty() && std::find(filters.begin(), filters.end(), tag) == filters.end())
            return;

        std::stringstream ss;
        ss << "[ " << clock() << " ] " << "{ " << tag << " } ";
        if (code != (unsigned int)(-1))
            ss << "( " << code << " ) ";
        ss << message.str() << "\n";

        if (Log::outConsole) {
            SetConsoleTextAttribute(hstd, (WORD) color);
            printf(ss.str().c_str());
        }
        
        if (outFile) {
            file << color << ss.str();
            file.flush();
        }
    }

    Log::Message::Message(std::string tag, unsigned int code) {
        this->tag = tag;
        this->code = code;
        if (colors.find(tag) != colors.end())
            this->color = colors[tag];
    }
}