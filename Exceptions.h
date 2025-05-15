#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <string>
using namespace std;

//Exceptions
class SimulationException {
private:
    string msg;
public:
    explicit SimulationException(const string& m) : msg(m) {}
    const string& what() const { return msg; }
};

class InputException : public SimulationException {
public:
    InputException(const string& m) : SimulationException(m) {}
};

class ParseException : public SimulationException {
public:
    ParseException(const string& m) : SimulationException(m) {}
};

#endif