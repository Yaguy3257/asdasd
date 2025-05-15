#ifndef FOREST_H
#define FOREST_H

#include "DynamicArray.h"
#include "Organism.h"
#include "Plant.h"
#include "Herbivore.h"
#include "Predator.h"
#include "WeatherEvent.h"
#include <iostream>
using namespace std;

// Forest controller
class Forest {
private:
    DynamicArray<Organism*> orgs;
    int day;

public:
    static const int MAX_POP = 200;   // Carry capacity

    Forest() : day(0) {}
    ~Forest() { clear(); }

    // Population helpers
    int livingCount() const {
        int c = 0;
        for (int i = 0; i < orgs.size(); ++i) {
            if (orgs[i]->isAlive()) ++c;
        }
        return c;
    }

    void add(Organism* o) {
        if (livingCount() >= MAX_POP) {
            cerr << "Population limit reached. Birth aborted.\n";
            delete o;
            return;
        }
        orgs.push_back(o);
    }

    void clear() {
        for (int i = 0; i < orgs.size(); ++i) {
            delete orgs[i];
        }
    }

    // Feeding helpers
    bool consumePlant();
    bool consumeHerbivore();

    // Sweep corpses
    void removeDead() {
        for (int i = 0; i < orgs.size();) {
            if (!orgs[i]->isAlive()) {
                delete orgs[i];
                orgs.erase(i);
            }
            else {
                ++i;
            }
        }
    }

    // One simulation step
    void nextDay();

    // Disaster hook
    void applyEvent(WeatherEvent& e);

    // Utility counts
    int countPlants() const;
    int countHerbivores() const;
    int countPredators() const;

    // Listing
    void list() const {
        cout << "\n--- Forest status (day " << day << ") ---\n";
        cout << "Plants: " << countPlants() << "  Herbivores: " << countHerbivores()
            << "  Predators: " << countPredators() << "\n";

        for (int i = 0; i < orgs.size(); ++i) {
            if (orgs[i]->isAlive()) {
                cout << *orgs[i] << "\n";
            }
        }
    }

    // Expose organisms for events
    DynamicArray<Organism*>& organisms() {
        return orgs;
    }

    void saveToFile(const std::string& filename) const;
    bool loadFromFile(const std::string& filename);
};

#endif