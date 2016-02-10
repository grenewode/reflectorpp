#pragma once

#include <random>
#include <limits>
#include <vector>

template <class T, class... Args>
class Source {
   public:
    virtual T next(Args...) = 0;
};

template <class T>
class CountingSource : public Source<T> {
   public:
    explicit CountingSource(T inital = 0) : count(inital) {}

    T next() override { return count++; }

   private:
    T count;
};

template <class T, T min, T max>
class StaticRangedRandomSource : public Source<T> {
   public:
    T next() override { return (rand() % (max - min)) + min; }
};

template <class T, class Random>
class RandomSource : public Source<T>, public Source<T, T, T> {
   public:
    RandomSource(Random* engine) : engine(engine) {}

    T next() override { return next(std::numeric_limits<T>::min(), std::numeric_limits<T>::max()); }

    T next(T min, T max) override {
        distribution_t dist(min, max);
        return dist(*engine);
    }

   private:
    using distribution_t = std::conditional_t<std::is_integral<T>::value, std::uniform_int_distribution<T>, std::uniform_real_distribution<T>>;
    Random* engine;
};
