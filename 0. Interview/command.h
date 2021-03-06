#ifndef COMMAND_H
#define COMMAND_H

#include <string>
#include <stdexcept>

class Command {
public:
    virtual std::string apply(const std::string &s) { throw std::runtime_error ("Wrong virtuality"); };
};

class ToUpperCommand: public Command {
public:
    std::string apply(const std::string &s);
};

class ToLowerCommand: public Command {
public:
    std::string apply(const std::string &s);
};

class ReplaceCommand: public Command {
    char A, B;
public:
    ReplaceCommand (char a, char b);
    std::string apply(const std::string &s);
};

#endif