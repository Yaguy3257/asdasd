#ifndef PREDATOR_H
#define PREDATOR_H

#include "Organism.h"

//Predator definition
class Predator : public Organism {
private:
    int hunger;
    static const int MAX_AGE = 60;
    static const int MAX_HUNGER = 12;

public:
    Predator() : hunger(0) {}

    const char* typeName() const override {
        return "Predator";
    }

    void liveOneDay(Forest& forest) override;

    void resetHunger() { hunger = 0; }
    int addHunger(int h) { hunger += h; return hunger; }
    int getHunger() const { return hunger; }
};

#endif