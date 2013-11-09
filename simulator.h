#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <vector>
#include <iostream>

class Simulator {
    public:
        Simulator(int y_low, int y_high, int x_low, int x_high);
        void simulate();
        void set_status(int y, int x, bool status);
        bool get_status(int y, int x);
        unsigned int to_x(int x);
        unsigned int to_y(int y);
        std::vector<std::vector<bool>>::const_iterator cbegin();
        std::vector<std::vector<bool>>::const_iterator cend();
        std::vector<std::vector<bool>>::const_reverse_iterator crbegin();
        std::vector<std::vector<bool>>::const_reverse_iterator crend();

    private:
        unsigned int count_live_neighbors(int y, int x); // x and y should be converted

    private:
        std::vector<std::vector<bool>> grid;
        const int y_low, y_high;
        const int x_low, x_high;
};

#endif /* !SIMULATOR_H */
