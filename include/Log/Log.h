/*

* This is log libary, created by NeutrinoZh

* Before starting to use, you need to initialize the library:
	wd::Log::init();

* Some Simple example of usings: 
	wd::Log::debug() << "Hello, World";
	wd::Log::error() << "This is error";
	wd::Log::info() << "Thank NeutrinoZh! This is nice libary";
	wd::Log::warning() << "Debugging only with a print is bad!";
	wd::Log::write("MyTag") << "Example using my lib";

* You can also specify an error / warning code, etc: [0; UINT32_MAX]
	wd::Log::debug(7) << "This is debug message with 7 code";
	wd::Log::error(9) << "Don't forget to write the documentation explaining the error-codes :)";
	wd::Log::write("MyTag", 8) << "My libary, my error-code, ohhh.. all is mine";

* You can specify the color for the tags:
	default colors: 
		ERROR -> red,
		DEBUG -> grey,
		INFO -> light gray,
		WARNING -> yellow

	wd::Log::addColor("MyTag", 7); <--- the second argument is UINT8 [0; 15] denotes color,
			table of colors:
	0 - black,
	1 - dark blue
	2 - dark green
	3 - turquoise
	4 - dark red
	5 - purple
	6 - brown
	7 - light grey
	8 - grey
	9 - blue
	10 - light green
	11 - light blue
	12 - light red
	13 - pink
	14 - yellow
	15 - white

	wd::Log::addColor("SimpleTag", 11);
	wd::Log::write("SimpleTag") << "This message is blue color!";

* You can tweak some parameters of the library:
	output in console: tdLog::outConsole = false or true
	output in file: tdLog::outFile = false or true

	path of output folder: tdLog::folder = "PATH\\TO\\YOUR\\LOGS"
	--->> Attention! Set output folder before inizilization libary! <<---

* And you can filter the logs:
	wd::Log::setFilters({"ERROR", "WARRING"}); 
	
	wd::Log::error() << "This message will be logged";
	wd::Log::warning() << "And this message will be logged";
	wd::Log::debug() << "This message will not be logged";

* You can load previously saved logs:
	wd::Log::load("log\\savedLog.txt")

* To clear the folder with logs:

	wd::Log::clear();
	--->> Attention! This method will permanently delete the entire contents of the tdLog::folder! <<---

*/

#include "utils.h"

namespace wd {

    class Log {
    public:
        static bool outConsole;
        static bool outFile;
        static std::string folder;
    private:
        static std::vector<std::string> filters;
        static std::map<std::string, UINT8> colors;
        static HANDLE hstd;
        static std::ofstream file;
        static std::string path;
        static tm tInfo;

        static std::string getDate();
        static void close();
    public:
        static void setFilters(std::vector<std::string> _filters);
        static void addColor(std::string tag, UINT8 color);

        static void load(std::string path);
        static void clear();

        static void init();
    private:
        class Message {
        private:	
            std::stringstream message;
            std::string tag = "";
            UINT8 color = 15;

            unsigned int code;

            bool last = true;

            Message(const Message& copy);
        public:
            ~Message();
            Message(std::string tag, unsigned int code);

            template <typename T>
            Message operator<<(T t) {
                message << t;
                last = false;
                return *this;
            }
            
            /*template <>
            Message operator<<(bool b) {
                message << (b ? "true" : "false");
                last = false;
                return *this;
            }*/
        };
    public:
        static Message write(std::string tag, unsigned int code = (unsigned int)(-1)) {
            return Message(tag, code);
        }
        static Message info(unsigned int code = (unsigned int)(-1)) {
            return write("INFO", code);
        }
        static Message warning(unsigned int code = (unsigned int)(-1)) {
            return write("WARNING", code);
        }
        static Message error(unsigned int code = (unsigned int)(-1)) {
            return write("ERROR", code);
        }
        static Message debug(unsigned int code = (unsigned int)(-1)) {
            return write("DEBUG", code);
        }
    };
}