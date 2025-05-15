#ifndef WEATHER_EVENT_H
#define WEATHER_EVENT_H

#include <iostream>

//Forest declaration
class Forest;

// Weather / Disaster system
class WeatherEvent {
public:
    virtual ~WeatherEvent() {}
    virtual void apply(Forest&) = 0;
    virtual const char* name() const = 0;
};

class FireEvent : public WeatherEvent {
private:
    int victims;

public:
    explicit FireEvent(int v) : victims(v) {}

    const char* name() const override {
        return "Wildfire";
    }

    void apply(Forest& f) override;
};

class DroughtEvent : public WeatherEvent {
public:
    const char* name() const override {
        return "Drought";
    }

    void apply(Forest& f) override;
};

class StormEvent : public WeatherEvent {
private:
    int victims;

public:
    explicit StormEvent(int v) : victims(v) {}

    const char* name() const override {
        return "Storm";
    }

    void apply(Forest& f) override;
};

#endif // WEATHER_EVENT_H