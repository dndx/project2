#include "simulator.h"
#include <algorithm>

using namespace std;

Simulator::Simulator(int y_low, int y_high, int x_low, int x_high) : 
                    y_low(y_low), y_high(y_high), x_low(x_low), x_high(x_high)
{
    grid.resize(y_high - y_low + 1);
    
    for (auto i = grid.begin(); i != grid.end(); i++)
    {
        i->resize(x_high - x_low + 1);
    }
}

void Simulator::simulate()
{
    std::vector<std::vector<bool>> next_gen{grid};
    
    for (auto iy = grid.cbegin(); iy != grid.cend(); ++iy)
    {
        unsigned int y = iy - grid.cbegin();

        for (auto ix = iy->cbegin(); ix != iy->cend(); ++ix)
        {
            unsigned int x = ix - iy->cbegin();
            unsigned int neighbors = count_live_neighbors(y, x);

            //cout << y << ' ' << x << " is " << neighbors << endl;

            if (*ix) // alive
            {
                if (neighbors < 2 || neighbors > 3)
                    next_gen[y][x] = false;
            }
            else // dead
            {
                if (neighbors == 3)
                    next_gen[y][x] = true;
            }
        }
    }

    grid = move(next_gen);
}

unsigned int Simulator::count_live_neighbors(int y, int x)
{
    //cout << "counting" << y << ',' << x << endl;
    unsigned int result{0};
    //cout << "max(y - 1, 0)" << max(y - 1, 0) << " min(y + 1, (int) to_y(y_high))"
    //     << min(y + 1, (int) to_y(y_high)) << endl;
    for (int iy = max(y - 1, 0); iy <= min(y + 1, (int) to_y(y_high)); ++iy)
    {
    //cout << "max(x - 1, 0)" << max(x - 1, 0) << " min(x + 1, (int) to_x(x_high))"
    //     << min(x + 1, (int) to_x(x_high)) << endl;
        for (int ix = max(x - 1, 0); ix <= min(x + 1, (int) to_x(x_high)); ++ix)
        {
           //cout << "    " << iy << ',' << ix << endl;
            if (iy == y && ix == x)
                continue;
            if (grid[iy][ix])
                result++;
        }
    }
    return result;
}

void Simulator::set_status(int y, int x, bool status)
{
    grid[to_y(y)][to_x(x)] = status;
}

bool Simulator::get_status(int y, int x)
{
    return grid[to_y(y)][to_x(x)];
}

unsigned int Simulator::to_x(int x)
{
    return x - x_low;
}

unsigned int Simulator::to_y(int y)
{
    return y - y_low;
}

vector<vector<bool>>::const_iterator Simulator::cbegin()
{
    return grid.cbegin();
}

vector<vector<bool>>::const_iterator Simulator::cend()
{
    return grid.cend();
}

vector<vector<bool>>::const_reverse_iterator Simulator::crbegin()
{
    return grid.crbegin();
}

vector<vector<bool>>::const_reverse_iterator Simulator::crend()
{
    return grid.crend();
}

vector<vector<bool>>::size_type Simulator::size() const noexcept
{
    return grid.size();
}

