#ifndef ORGANISM_H
#define ORGANISM_H

#include <iostream>
using namespace std;

//Forest declaration
class Forest;

//Base class Organism
class Organism {
protected:
    static long nextId;
    long id;
    int age;
    bool alive;

public:
    Organism() : id(nextId++), age(0), alive(true) {}
    virtual ~Organism() {}

    virtual void liveOneDay(Forest&) = 0;  // pure virtual
    virtual const char* typeName() const = 0;

    long getId() const { return id; }
    int getAge() const { return age; }
    bool isAlive() const { return alive; }

    void kill() { alive = false; }
    Organism& operator++() { ++age; return *this; }

    friend ostream& operator<<(ostream& os, const Organism& o) {
        os << '[' << o.typeName() << " #" << o.id << " age=" << o.age << ']';
        return os;
    }

    void setId(long newId) { 
        id = newId; 
    }
    void setAge(int  newAge) { 
        age = newAge; 
    }
    void setAlive(bool a) {
        alive = a; 
    }
};

long Organism::nextId = 1;

#endif