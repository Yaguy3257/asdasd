#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

//DynamicArray
template<typename T>
class DynamicArray {
private:
    T* data;
    int sz;
    int cap;

    void grow() {
        int newCap = cap ? cap * 2 : 4;
        T* n = new T[newCap];
        for (int i = 0; i < sz; ++i) {
            n[i] = data[i];
        }
        delete[] data;
        data = n;
        cap = newCap;
    }

public:
    DynamicArray() : data(nullptr), sz(0), cap(0) {}
    ~DynamicArray() { delete[] data; }

    int size() const { return sz; }
    T& operator[](int i) { return data[i]; }
    const T& operator[](int i) const { return data[i]; }

    void push_back(const T& v) {
        if (sz == cap) grow();
        data[sz++] = v;
    }

    void erase(int idx) {
        for (int i = idx + 1; i < sz; ++i) {
            data[i - 1] = data[i];
        }
        --sz;
    }
};

#endif