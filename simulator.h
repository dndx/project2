#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <vector>
#include <iostream>

class Simulator {
    public:
        Simulator(int x_low, int x_high, int y_low, int y_high);
        void simulate();
        void set_status(int x, int y, bool status);
        bool get_status(int x, int y);

    private:
        int to_x(int x);
        int to_y(int y);
        unsigned int count_live_neighbors(int x, int y); // x and y should be converted

    private:
        std::vector<std::vector<bool>> grid;
        const int x_low, x_high;
        const int y_low, y_high;
};

#endif /* !SIMULATOR_H */
