#ifndef HERBIVORE_H
#define HERBIVORE_H

#include "Organism.h"

//Herbivore definition 
class Herbivore : public Organism {
private:
    int hunger;
    static const int MAX_AGE = 50;
    static const int MAX_HUNGER = 10;

public:
    Herbivore() : hunger(0) {}

    const char* typeName() const override {
        return "Herbivore";
    }

    void liveOneDay(Forest& forest) override;

    void resetHunger() { hunger = 0; }
    int addHunger(int h) { hunger += h; return hunger; }
    int getHunger() const { return hunger; }
};

#endif