#ifndef BASE_H
#define BASE_H

#include <string>
#include <vector>
#include <memory>

#include "tools.hpp"

namespace log {
    class BaseLogger {
    protected:
        int _level;
        BaseLogger();
        std::ostream *_outputter;
    public:
        static BaseLogger& getInstance();

        void debug(const std::string &message) const;
        void info(const std::string &message) const;
        void warning(const std::string &message) const;
        void error(const std::string &message) const;
        void set_level(int level);
        int level();
        void flush();
        virtual ~BaseLogger();
    private:
        virtual void log(const std::string &message, const std::string &level_name, int level) const;
    };
};

#endif