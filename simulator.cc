#include "simulator.h"
#include <algorithm>

using namespace std;

Simulator::Simulator(int x_low, int x_high, int y_low, int y_high) : 
                    x_low(x_low), x_high(x_high), y_low(y_low), y_high(y_high)
{
    grid.resize(x_high - x_low + 1);
    
    for (auto i = grid.begin(); i != grid.end(); i++)
    {
        i->resize(y_high - y_low + 1);
    }
}

void Simulator::simulate()
{
    std::vector<std::vector<bool>> next_gen{grid};
    
    for (auto ix = grid.cbegin(); ix != grid.cend(); ix++)
    {
        unsigned int x = ix - grid.cbegin();

        for (auto iy = ix->cbegin(); iy != ix->cend(); iy++)
        {
            unsigned int y = iy - ix->cbegin();
            unsigned int neighbors = count_live_neighbors(x, y);

            if (*iy) // alive
            {
                if (neighbors < 2 || neighbors > 3)
                    next_gen[x][y] = false;
            }
            else // dead
            {
                if (neighbors == 3)
                    next_gen[x][y] = true;
            }
        }
    }

    grid = next_gen;
}

unsigned int Simulator::count_live_neighbors(int x, int y)
{
    unsigned int result{0};
    for (int ix = max(x - 1, 0); ix < min(x + 1, to_x(x_high)); ix++)
    {
        for (int iy = max(y - 1, 0); iy < min(y + 1, to_y(y_high)); iy++)
        {
            if (ix == x && iy == y)
                continue;
            if (get_status(ix, iy))
                result++;
        }
    }
    return result;
}

void Simulator::set_status(int x, int y, bool status)
{
    grid[to_x(x)][to_y(y)] = status;
}

bool Simulator::get_status(int x, int y)
{
    return grid[to_x(x)][to_y(y)];
}

int Simulator::to_x(int x)
{
    return x - x_low;
}

int Simulator::to_y(int y)
{
    return y - y_low;
}

