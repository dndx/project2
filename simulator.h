#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <vector>
#include <iostream>
#include <array>
#include <utility>
#include <QColor>
#include <QImage>
#include "miner_type.h"

class Simulator {
    public:
        Simulator(int y_low, int y_high, int x_low, int x_high);
        virtual void simulate();
        void show(std::array<unsigned char, 4> &table, std::pair<int, int> yrange, std::pair<int, int> xrange);
        virtual void show_file(std::pair<std::string, Struct> &input, std::pair<int, int> terrain_y_range, std::pair<int, int> terrain_x_range,
                     std::pair<int, int> window_y_range, std::pair<int, int> window_x_range) = 0;
        QImage *generate_qt_terrain(std::array<std::array<int, 3>, 4> &table, std::pair<int, int> yrange, std::pair<int, int> xrange);
        void set_status(int y, int x, char status);
        char get_status(int y, int x);
        unsigned int to_x(int x);
        unsigned int to_y(int y);
        void set_reset(); // save current terrain
        void reset(); // reset to generation 0
        size_t get_generation();
        std::vector<char>::size_type xsize() const noexcept;
        std::vector<std::vector<char>>::const_iterator cbegin();
        std::vector<std::vector<char>>::const_iterator cend();
        std::vector<std::vector<char>>::const_reverse_iterator crbegin();
        std::vector<std::vector<char>>::const_reverse_iterator crend();
        std::vector<std::vector<char>>::size_type size() const noexcept;
        std::vector<std::vector<char>>::const_reference operator[] (std::vector<std::vector<char>>::size_type n) const;
        virtual ~Simulator() {};

    protected:
        std::vector<std::vector<char>> grid, original;
        const int y_low, y_high;
        const int x_low, x_high;
        size_t generation = 0;
};

class GoLSimulator : public Simulator {
    public:
        GoLSimulator(int y_low, int y_high, int x_low, int x_high);
        static const char STATE_ALIVE = 1;
        static const char STATE_DEAD = 0;
        virtual void simulate();
        virtual void show_file(std::pair<std::string, Struct> &input, std::pair<int, int> terrain_y_range, std::pair<int, int> terrain_x_range,
                     std::pair<int, int> window_y_range, std::pair<int, int> window_x_range);

    private:
        unsigned int count_live_neighbors(int y, int x); // x and y should be converted
};

class WWSimulator : public Simulator {
    public:
        WWSimulator(int y_low, int y_high, int x_low, int x_high);
        static const char STATE_EMPTY = 0;
        static const char STATE_HEAD = 1;
        static const char STATE_TAIL = 2;
        static const char STATE_WIRE = 3;
        virtual void simulate();
        virtual void show_file(std::pair<std::string, Struct> &input, std::pair<int, int> terrain_y_range, std::pair<int, int> terrain_x_range,
                     std::pair<int, int> window_y_range, std::pair<int, int> window_x_range);

    private:
        unsigned int count_head_neighbors(int y, int x); // x and y should be converted
};

class ECSimulator : public Simulator {
    public:
        ECSimulator(int y_low, int y_high, int x_low, int x_high, unsigned char rule);
        static const char STATE_ZERO = 0;
        static const char STATE_ONE = 1;
        virtual void simulate();
        virtual void show_file(std::pair<std::string, Struct> &input, std::pair<int, int> terrain_y_range, std::pair<int, int> terrain_x_range,
                     std::pair<int, int> window_y_range, std::pair<int, int> window_x_range);

    private:
        std::array<bool, 8> code;
        unsigned char determine_state(int y, int x); // x and y should be converted and referring to a zero cell
};

#endif /* !SIMULATOR_H */

