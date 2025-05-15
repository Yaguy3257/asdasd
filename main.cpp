#include "Exceptions.h"
#include "Forest.h"
#include "Plant.h"
#include "Herbivore.h"
#include "Predator.h"
#include "WeatherEvent.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

void Forest::saveToFile(const std::string& filename) const {
    // создаём (перезаписывая) бинарный файл
    std::ofstream fout;
    fout.open(filename, std::ios::out | std::ios::binary);
    if (!fout.is_open()) {
        std::cerr << "Не удалось открыть файл для записи: "
            << filename << std::endl;
        return;
    }

    // 1) Записываем текущий день
    fout.write(reinterpret_cast<const char*>(&day), sizeof(day));

    // 2) Считаем живых организмов и записываем их число
    int count = 0;
    for (int i = 0; i < orgs.size(); ++i)
        if (orgs[i]->isAlive()) ++count;
    fout.write(reinterpret_cast<const char*>(&count), sizeof(count));

    // 3) Записываем построчно каждый объект
    //    Сначала тип ('P','H','R'), потом id, age, потом growth/hunger
    for (int i = 0; i < orgs.size(); ++i) {
        Organism* o = orgs[i];
        if (!o->isAlive()) continue;

        char t;
        if (dynamic_cast<Plant*>(o))     t = 'P';
        else if (dynamic_cast<Herbivore*>(o)) t = 'H';
        else                                  t = 'R';

        fout.write(&t, sizeof(t));

        long id = o->getId();
        int  age = o->getAge();
        fout.write(reinterpret_cast<const char*>(&id), sizeof(id));
        fout.write(reinterpret_cast<const char*>(&age), sizeof(age));

        if (t == 'P') {
            int g = static_cast<Plant*>(o)->getGrowth();
            fout.write(reinterpret_cast<const char*>(&g), sizeof(g));
        }
        else {
            int h = (t == 'H')
                ? static_cast<Herbivore*>(o)->getHunger()
                : static_cast<Predator*>(o)->getHunger();
            fout.write(reinterpret_cast<const char*>(&h), sizeof(h));
        }
    }

    fout.close();
}

bool Forest::loadFromFile(const std::string& filename) {
    std::ifstream fin;
    fin.open(filename, std::ios::in | std::ios::binary);
    if (!fin.is_open()) {
        // не страшно — просто файл первый раз не создан
        return false;
    }

    // 1) Читаем день
    int fileDay = 0;
    fin.read(reinterpret_cast<char*>(&fileDay), sizeof(fileDay));
    if (!fin) {
        std::cerr << "Ошибка чтения дня из файла\n";
        fin.close();
        return false;
    }
    day = fileDay;

    // 2) Читаем число записей
    int count = 0;
    fin.read(reinterpret_cast<char*>(&count), sizeof(count));
    if (!fin || count < 0) {
        std::cerr << "Ошибка чтения числа организмов\n";
        fin.close();
        return false;
    }

    // 3) Очищаем текущее состояние и восстанавливаем из файла
    clear();
    long   id;
    int    age, stat;
    for (int i = 0; i < count; ++i) {
        char t;
        fin.read(&t, sizeof(t));
        fin.read(reinterpret_cast<char*>(&id), sizeof(id));
        fin.read(reinterpret_cast<char*>(&age), sizeof(age));
        fin.read(reinterpret_cast<char*>(&stat), sizeof(stat));
        if (!fin) {
            std::cerr << "Ошибка чтения записи #" << i << "\n";
            fin.close();
            return false;
        }

        Organism* o = nullptr;
        if (t == 'P') { auto* p = new Plant();     p->setGrowth(stat); o = p; }
        else if (t == 'H') { auto* h = new Herbivore(); h->addHunger(stat); o = h; }
        else if (t == 'R') { auto* r = new Predator();   r->addHunger(stat); o = r; }
        else {
            std::cerr << "Неизвестный тип '" << t << "' в записи\n";
            fin.close();
            return false;
        }

        // Сеттеры для protected-полей (добавить в Organism.h)
        o->setId(id);
        o->setAge(age);
        o->setAlive(true);

        orgs.push_back(o);
    }

    fin.close();
    return true;
}

bool Forest::consumePlant() {
    for (int i = 0; i < orgs.size(); ++i) {
        if (orgs[i]->isAlive()) {
            Plant* p = dynamic_cast<Plant*>(orgs[i]);
            if (p) {
                p->kill();
                return true;
            }
        }
    }
    return false;
}

bool Forest::consumeHerbivore() {
    for (int i = 0; i < orgs.size(); ++i) {
        if (orgs[i]->isAlive()) {
            Herbivore* h = dynamic_cast<Herbivore*>(orgs[i]);
            if (h) {
                h->kill();
                return true;
            }
        }
    }
    return false;
}

int Forest::countPlants() const {
    int c = 0;
    for (int i = 0; i < orgs.size(); ++i) {
        if (orgs[i]->isAlive()) {
            const Plant* p = dynamic_cast<const Plant*>(orgs[i]);
            if (p) ++c;
        }
    }
    return c;
}

int Forest::countHerbivores() const {
    int c = 0;
    for (int i = 0; i < orgs.size(); ++i) {
        if (orgs[i]->isAlive()) {
            const Herbivore* h = dynamic_cast<const Herbivore*>(orgs[i]);
            if (h) ++c;
        }
    }
    return c;
}

int Forest::countPredators() const {
    int c = 0;
    for (int i = 0; i < orgs.size(); ++i) {
        if (orgs[i]->isAlive()) {
            const Predator* p = dynamic_cast<const Predator*>(orgs[i]);
            if (p) ++c;
        }
    }
    return c;
}



void Forest::nextDay() {
    ++day;

    for (int i = 0; i < orgs.size(); ++i) {
        if (orgs[i]->isAlive())
            orgs[i]->liveOneDay(*this);
    }
    removeDead();

    int predCount = countPredators();
    int groups = predCount / 3;
    int killedHerb = 0;
    for (int i = 0; i < groups; ++i) {
        if (consumeHerbivore()) ++killedHerb;
    }
    cout << "Group hunt: " << killedHerb
        << " herbivores slain by " << groups << " predator-groups.\n";

    int toFeed = killedHerb * 3;
    int fed = 0;
    for (int i = 0; i < orgs.size() && fed < toFeed; ++i) {
        if (Predator* p = dynamic_cast<Predator*>(orgs[i])) {
            if (p->isAlive()) { p->resetHunger(); ++fed; }
        }
    }
    removeDead();

    if (day % 3 == 0 && countPredators() >= 4) {
        for (int j = 0; j < 2; ++j)
            add(new Predator());
        cout << "Predators reproduction: 2 new predators born.\n";
    }
}

void Plant::liveOneDay(Forest& forest) {
    ++(*this);
    growth += 1;

    if (age > MAX_AGE || growth > MAX_GROWTH) {
        kill();
        return;
    }

    if (growth >= REPRO_GROWTH && forest.livingCount() < Forest::MAX_POP) {
        forest.add(new Plant());
        growth = 0;
    }
}

void Herbivore::liveOneDay(Forest& forest) {
    ++(*this);
    bool fed = false;

    if (forest.consumePlant()) {
        resetHunger();
        fed = true;
    }
    else if (addHunger(2) > MAX_HUNGER) {
        kill();
        return;
    }

    if (getAge() > MAX_AGE) {
        kill();
        return;
    }

    if (fed && getAge() >= 4 && forest.livingCount() < Forest::MAX_POP) {
        forest.add(new Herbivore());
    }
}

void Predator::liveOneDay(Forest& forest) {
    ++(*this);
    if (addHunger(3) > MAX_HUNGER) {
        kill();
        return;
    }
    if (getAge() > MAX_AGE) {
        kill();
    }
}


// Weather event
void FireEvent::apply(Forest& f) {
    int burnt = 0;
    auto& o = f.organisms();

    for (int i = 0; i < o.size() && burnt < victims; ++i) {
        Plant* p = dynamic_cast<Plant*>(o[i]);
        if (p && p->isAlive()) {
            p->kill();
            ++burnt;
        }
    }

    cout << "Fire destroyed " << burnt << " plants.\n";
}

void DroughtEvent::apply(Forest& f) {
    int affected = 0;
    auto& o = f.organisms();

    for (int i = 0; i < o.size(); ++i) {
        Plant* p = dynamic_cast<Plant*>(o[i]);
        if (p && p->isAlive()) {
            p->reduceGrowth(5);
            ++affected;
        }
    }

    cout << "Drought weakened " << affected << " plants.\n";
}

void StormEvent::apply(Forest& f) {
    int total = victims;
    const int species = 3;
    int base = total / species;
    int rem = total % species;
    int killCounts[3] = { base, base, base };
    for (int i = 0; i < rem; ++i) killCounts[i]++;

    int kp = 0, kh = 0, kpr = 0;
    auto& o = f.organisms();

    //Burn plants
    for (int i = 0; i < o.size() && kp < killCounts[0]; ++i) {
        if (Plant* p = dynamic_cast<Plant*>(o[i])) {
            if (p->isAlive()) { p->kill(); ++kp; }
        }
    }
    //Kill herbivores
    for (int i = 0; i < o.size() && kh < killCounts[1]; ++i) {
        if (Herbivore* h = dynamic_cast<Herbivore*>(o[i])) {
            if (h->isAlive()) { h->kill(); ++kh; }
        }
    }
    //Kill predators
    for (int i = 0; i < o.size() && kpr < killCounts[2]; ++i) {
        if (Predator* pr = dynamic_cast<Predator*>(o[i])) {
            if (pr->isAlive()) { pr->kill(); ++kpr; }
        }
    }

    cout << "Storm killed " << kp << " plants, "
        << kh << " herbivores, "
        << kpr << " predators.\n";
}


void Forest::applyEvent(WeatherEvent& e) {
    e.apply(*this);
    removeDead();
}

//Menu
void showMenu() {
    cout << "\n=== Forest Simulator ===\n"
        << "1) Add Plant\n"
        << "2) Add Herbivore\n"
        << "3) Add Predator\n"
        << "4) Show Status\n"
        << "5) Next Day\n"
        << "6) Trigger Disaster\n"
        << "0) Exit\n"
        << "Choose option: ";
}

void addOrganismMenu(Forest& f, int choice) {
    if (choice == 1) {
        f.add(new Plant());
    }
    else if (choice == 2) {
        f.add(new Herbivore());
    }
    else if (choice == 3) {
        f.add(new Predator());
    }
}

void disasterMenu(Forest& forest) {
    cout << "\n-- Select disaster --\n"
        << "1) Wildfire (specify victims)\n"
        << "2) Drought\n"
        << "3) Storm (specify victims)\n"
        << "0) Back\n"
        << "Choice: ";

    int d;
    if (!(cin >> d)) throw InputException("Non‑numeric disaster menu");

    switch (d) {
    case 1: {
        cout << "Victims (plants): ";
        int v;
        if (!(cin >> v)) throw InputException("Bad victims");
        FireEvent e(v);
        forest.applyEvent(e);
        break;
    }
    case 2: {
        DroughtEvent e;
        forest.applyEvent(e);
        break;
    }
    case 3: {
        cout << "Victims (any): ";
        int v;
        if (!(cin >> v)) throw InputException("Bad victims");
        StormEvent e(v);
        forest.applyEvent(e);
        break;
    }
    case 0:
    default:
        break;
    }
}


//Main
int main() {
    Forest forest;

    if (!forest.loadFromFile("forest.dat")) {
        std::cout << "Файл состояния не найден — начинаем новый симулятор\n";
    }

    bool run = true;

    while (run) {
        try {
            showMenu();
            int op;

            if (!(cin >> op)) throw InputException("Non‑numeric menu input");

            switch (op) {
            case 1:
            case 2:
            case 3:
                addOrganismMenu(forest, op);
                cout << "Added!\n";
                break;
            case 4:
                forest.list();
                break;
            case 5:
                forest.nextDay();
                cout << "Day advanced.\n";
                break;
            case 6:
                disasterMenu(forest);
                break;
            case 0:
                run = false;
                break;
            default:
                cout << "Unknown option\n";
                break;
            }
        }
        catch (const SimulationException& e) {
            cerr << "Error: " << e.what() << "\n";
            cin.clear();
            cin.ignore(10000, '\n');
        }
    }

    forest.saveToFile("forest.dat");

    return 0;
}