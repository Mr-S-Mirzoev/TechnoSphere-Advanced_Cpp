#ifndef TOOLS_H
#define TOOLS_H

#include <string>

typedef int level_t;

namespace log {
    enum LevelNo{
        NIL, // Nil value to define an error in passed Level Number

        DEBUG,
        INFO,
        WARNING,
        ERROR,

        QUANTITY // The constant to know  now many levels do we have
    };

    class Level {
        level_t _level;
    public:
        Level(level_t level);
        level_t to_int () const;
        bool allowed(Level checked);
        std::string to_string() const;
    };

    void pr_debug(const std::string &s);
};

#endif