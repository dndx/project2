#include <iostream>
#include <unistd.h>
#include "utils.h"
#include "miner_parser.h"
#include "simulator.h"

using namespace std;

void show_terrain(Simulator &sim)
{
    for (auto iy = sim.crbegin(); iy != sim.crend(); ++iy)
    {
        for (auto ix = iy->cbegin(); ix != iy->cend(); ++ix)
        {
            if (*ix) // alive
            {
                cout << '@';
            }
            else // dead
            {
                cout << '-';
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

    if (!input.count("Terrain"))
    {
        FATAL("missing Terrain definition");
    }

    Struct terrian_range{input["Terrain"]};
    Struct window_range;

    if (!input.count("Window"))
        window_range = terrian_range;
    else
        window_range = input["Window"];

    pair<int, int> terrian_x_range{terrian_range["Xrange"]};
    pair<int, int> terrian_y_range{terrian_range["Yrange"]};

    Simulator sim{terrian_y_range.first, terrian_y_range.second,
                  terrian_x_range.first, terrian_x_range.second};

    if (!input.count("Initial"))
        FATAL("missing Initial defination");

    Struct initial{input["Initial"]};
    if (!initial.count("Alive"))
        FATAL("missing Alive definition");

    List initial_alive{initial["Alive"]};

    for (unsigned int i = 0; i < initial_alive.size(); ++i)
    {
        pair<int, int> coord{initial_alive[i]};

        if (coord.first < terrian_x_range.first || coord.first > terrian_x_range.second)
            FATAL("initial cell x coordinate out of bound, got %d", coord.first);

        if (coord.second < terrian_y_range.first || coord.second > terrian_y_range.second)
            FATAL("initial cell y coordinate out of bound, got %d", coord.second);

        sim.set_status(coord.second, coord.first, true);
    }

    
    unsigned int iter{0};
    cout << "genetation " << iter << endl;
    show_terrain(sim);
    while (++iter)
    {
        sim.simulate();
        cout << "genetation " << iter << endl;
        show_terrain(sim);
        //sleep(1);
    }

    return 0;
}

