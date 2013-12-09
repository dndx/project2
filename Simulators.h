#ifndef SIMULATORS_H
#define SIMULATORS_H

#include <vector>
#include <iostream>
#include <array>
#include <utility>
#include <QColor>
#include <QImage>
#include "miner_type.h"

class Simulator {
    public:
        /*
         * Constructor, takes terrain ranges as paramater
         */
        Simulator(int y_low, int y_high, int x_low, int x_high);
        virtual void simulate(); // abstract simulate method
        /*
         * Output current grid to stdout
         * table: the status -> char mapping table
         */
        void show(std::array<unsigned char, 4> &table, std::pair<int, int> yrange, std::pair<int, int> xrange);
        /*
         * Output current grid to stdout in Dr. Miner's format
         * virtual because simulator needs to know that kind of file this is
         * eg. Life = ...
         */
        virtual void show_file(std::pair<std::string, Struct> &input, std::pair<int, int> terrain_y_range, std::pair<int, int> terrain_x_range,
                     std::pair<int, int> window_y_range, std::pair<int, int> window_x_range) = 0;
        /*
         * Generate a QImage of current grid for our custom widget to use
         * table: the status -> rgba mapping table
         *
         * caller is responsible for freeing the returned QImage after use
         */
        QImage *generate_qt_terrain(std::array<std::array<int, 3>, 4> &table, std::pair<int, int> yrange, std::pair<int, int> xrange);
        void set_status(int y, int x, char status); // set the status of grid at x, y. xy are absolute coordinates
        char get_status(int y, int x); // get the status of grid at x, y. xy are absolute coordinates
        // transform an absolute coordinate to relative coordinate
        unsigned int to_x(int x) const noexcept;
        unsigned int to_y(int y) const noexcept;
        void set_reset(); // save current terrain
        void reset(); // reset to generation 0
        size_t get_generation(); // return the current generation number
        std::vector<char>::size_type xsize() const noexcept; // row size of terrain
        // various iterators for outputting
        std::vector<std::vector<char>>::const_iterator cbegin();
        std::vector<std::vector<char>>::const_iterator cend();
        std::vector<std::vector<char>>::const_reverse_iterator crbegin();
        std::vector<std::vector<char>>::const_reverse_iterator crend();
        std::vector<std::vector<char>>::size_type size() const noexcept;
        std::vector<std::vector<char>>::const_reference operator[] (std::vector<std::vector<char>>::size_type n) const;
        virtual ~Simulator() {};

    protected:
        /*
         * initial (for reset) and current grids
         */
        std::vector<std::vector<char>> grid, original;
        // terrain ranges
        const int y_low, y_high;
        const int x_low, x_high;
        // default generation is 0
        size_t generation = 0;
};

/*
 * Game of Life simulator
 */
class GoLSimulator : public Simulator {
    public:
        GoLSimulator(int y_low, int y_high, int x_low, int x_high);
        // grid status
        static const char STATE_ALIVE = 1;
        static const char STATE_DEAD = 0;
        virtual void simulate();
        virtual void show_file(std::pair<std::string, Struct> &input, std::pair<int, int> terrain_y_range, std::pair<int, int> terrain_x_range,
                     std::pair<int, int> window_y_range, std::pair<int, int> window_x_range);

    private:
        unsigned int count_live_neighbors(int y, int x); // x and y should be relative coordinates
};

/*
 * Wire World simulator
 */
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
        unsigned int count_head_neighbors(int y, int x); // x and y should be relative coordinates
};

/*
 * Elementary simulator
 */
class ECSimulator : public Simulator {
    public:
        ECSimulator(int y_low, int y_high, int x_low, int x_high, unsigned char rule);
        static const char STATE_ZERO = 0;
        static const char STATE_ONE = 1;
        virtual void simulate();
        virtual void show_file(std::pair<std::string, Struct> &input, std::pair<int, int> terrain_y_range, std::pair<int, int> terrain_x_range,
                     std::pair<int, int> window_y_range, std::pair<int, int> window_x_range);

    private:
        std::array<bool, 8> code; // pattern codes for new state
        unsigned char determine_state(int y, int x); // x and y should be relative coordinates and referring to a zero cell
};

#endif /* !SIMULATORS_H */

