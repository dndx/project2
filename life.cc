#include <iostream>
#include "utils.h"
#include "miner_parser.h"
#include "simulator.h"

using namespace std;

void show_terrain(Simulator &sim, unsigned char alive, unsigned char dead, pair<int, int> yrange, pair<int, int> xrange)
{
    yrange.first = sim.to_y(yrange.first);
    yrange.second = sim.to_y(yrange.second);
    xrange.first = sim.to_x(xrange.first);
    xrange.second = sim.to_x(xrange.second);

    for (auto iy = sim.crbegin(); iy != sim.crend(); ++iy)
    {
        int y = iy - sim.crbegin();
        if (y < yrange.first || y > yrange.second)
            continue;
        for (auto ix = iy->cbegin(); ix != iy->cend(); ++ix)
        {
            int x = ix - iy->cbegin();
            if (x < xrange.first || x > xrange.second)
                continue;
            if (*ix) // alive
            {
                cout << alive;
            }
            else // dead
            {
                cout << dead;
            }
        }
        cout << endl;
    }
}

int main(int argc, char *argv[])
{
    std::string in_buffer;

    int ch;
    while ((ch = cin.get()) != EOF)
    {
        in_buffer.push_back(ch);
    }

    Struct input = parse_miner_string(in_buffer);

    if (input["Terrain"].get_type() != STRUCT)
    {
        FATAL("missing Terrain definition or incorrect value type for Terrain");
    }

    Struct terrain_range{input["Terrain"]};

    if (terrain_range["Xrange"].get_type() != RANGE || terrain_range["Yrange"].get_type() != RANGE)
    {
        FATAL("Xrange and Yrange for Terrain must be range type");
    }

    Struct window_range;

    if (!input.count("Window"))
    {
        window_range = terrain_range;
    }
    else
    {
        if (input["Window"].get_type() != STRUCT)
            FATAL("incorrect type of Window");
        window_range = input["Window"];
        if (window_range["Xrange"].get_type() != RANGE || window_range["Yrange"].get_type() != RANGE)
            FATAL("missing Xrange and Yrange of Window or incorrect type");
    }

    pair<int, int> terrain_x_range{terrain_range["Xrange"]};
    pair<int, int> terrain_y_range{terrain_range["Yrange"]};
    pair<int, int> window_y_range{window_range["Yrange"]};
    pair<int, int> window_x_range{window_range["Xrange"]};

    Simulator sim{terrain_y_range.first, terrain_y_range.second,
                  terrain_x_range.first, terrain_x_range.second};

    if (input["Initial"].get_type() != STRUCT)
        FATAL("missing Initial defination or incorrect value type for Initial");

    Struct initial{input["Initial"]};
    if (initial["Alive"].get_type() != LIST)
        FATAL("missing Alive or incorrect type");

    List initial_alive{initial["Alive"]};

    for (unsigned int i = 0; i < initial_alive.size(); ++i)
    {
        pair<int, int> coord{initial_alive[i]};

        if (coord.first < terrain_x_range.first || coord.first > terrain_x_range.second)
            FATAL("initial cell x coordinate out of bound, got %d", coord.first);

        if (coord.second < terrain_y_range.first || coord.second > terrain_y_range.second)
            FATAL("initial cell y coordinate out of bound, got %d", coord.second);

        sim.set_status(coord.second, coord.first, true);
    }

    if (window_y_range.first < terrain_y_range.first || window_y_range.second > terrain_y_range.second)
        FATAL("Y range of Window out of bound");

    if (window_x_range.first < terrain_x_range.first || window_x_range.second > terrain_x_range.second)
        FATAL("X range of Window out of bound");

    if (input["Chars"].get_type() != STRUCT)
        FATAL("missing Chars definition or incorrect value type");

    Struct chars{input["Chars"]};

    if (chars["Alive"].get_type() != INTEGER || chars["Dead"].get_type() != INTEGER)
        FATAL("missing Alive and Dead definition or incorrect value type");

    unsigned char alive{chars["Alive"]}, dead{chars["Dead"]};

    unsigned int iter{0};
    cout << "genetation " << iter << endl;
    show_terrain(sim, alive, dead, window_y_range, window_x_range);
    sim.simulate();
    show_terrain(sim, alive, dead, window_y_range, window_x_range);

    return 0;
}

