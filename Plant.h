#ifndef PLANT_H
#define PLANT_H
#include "Organism.h"
#include <iostream>

//Plant definition
class Plant : public Organism {
private:
    int growth;
    static const int MAX_AGE = 30;
    static const int MAX_GROWTH = 20;
    static const int REPRO_GROWTH = 6;

public:
    Plant() : growth(0) {}

    const char* typeName() const override {
        return "Plant";
    }

    void liveOneDay(Forest& forest) override;

    void reduceGrowth(int g) {
        growth -= g;
        if (growth <= 0) kill();
    }

    int getGrowth() const { return growth; }
    void setGrowth(int g) { growth = g; }
};

#endif