#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cstdio>
#include <QHBoxLayout>
#include <QApplication>
#include "utils.h"
#include "miner_parser.h"
#include "simulator.h"
#include "LifeGrid.h"

using namespace std;

QImage *generate_terrain(Simulator &sim, array<int, 3> alive, array<int, 3> dead, pair<int, int> yrange, pair<int, int> xrange)
{
    yrange.first = sim.to_y(yrange.first);
    yrange.second = sim.to_y(yrange.second);
    xrange.first = sim.to_x(xrange.first);
    xrange.second = sim.to_x(xrange.second);

    QImage *img = new QImage(xrange.second - xrange.first + 1, 
                             yrange.second - yrange.first + 1, QImage::Format_ARGB32);
    img->fill(QColor(dead[0], dead[1], dead[2]));

    for (auto iy = sim.crbegin(); iy != sim.crend(); ++iy)
    {
        int y = iy - sim.crbegin();
        //if (y < yrange.first || y > yrange.second)
        //    continue;
        for (auto ix = iy->cbegin(); ix != iy->cend(); ++ix)
        {
            int x = ix - iy->cbegin();
        //    if (x < xrange.first || x > xrange.second)
        //        continue;
            if (*ix) // alive
            {
                img->setPixel(x, y, QColor(alive[0], alive[1], alive[2]).rgba());
            }
        }
    }

    return img;
}

int main(int argc, char *argv[])
{
    opterr = 0; // mute getopt error meesage
    int c;
    int generation{0};
    bool terrain_x_override{false}, terrain_y_override{false};
    bool window_x_override{false}, window_y_override{false};
    pair<int, int> terrain_y_range, terrain_x_range, window_y_range, window_x_range;

    for (int i = 1; i < argc; i++)
    {
        string arg{argv[i]};

        if (arg == "-ty")
        {
            argv[i][1] = 'q';
            argv[i][2] = '\0';
        }
        else if (arg == "-tx")
        {
            argv[i][1] = 'w';
            argv[i][2] = '\0';
        }
        else if (arg == "-wy")
        {
            argv[i][1] = 'e';
            argv[i][2] = '\0';
        }
        else if (arg == "-wx")
        {
            argv[i][1] = 'r';
            argv[i][2] = '\0';
        }
    }

    while ((c = getopt(argc, argv, "g:hq:w:e:r:")) != -1)
    {
        switch (c)
        {
            case 'h':
                cerr << "Usage: life [-fhv] [-g n] [-tx l..h] [-ty l..h] [-wx l..h] [-wy l..h] [filename]" << endl << endl
                     << "  -g n      Specify the desired generation number. If omitted, the default should be n = 0" << endl
                     << "  -h        Display this message and quit" << endl
                     << "  -tx l..h  Set the x range of the terrain; overrides values specified in the input file" << endl
                     << "  -ty l..h  Set the y range of the terrain; overrides values specified in the input file" << endl
                     << "            dead and alive cells" << endl
                     << "  -wx l..h  Set the x range of the output window; otherwise this defaults to the x range of the terrain" << endl
                     << "  -wy l..h  Set the y range of the output window; otherwise this defaults to the x range of the terrain" << endl
                     << endl;
                return 0;

            case 'g':
                generation = atoi(optarg);
                if (generation < 0)
                    FATAL("generation number must equal or greater than zero");
                break;

            case 'q': // ty
                terrain_y_override = true;
                terrain_y_range = extract_pair(optarg);
                break;

            case 'w': // tx
                terrain_x_override = true;
                terrain_x_range = extract_pair(optarg);
                break;

            case 'e': // wy
                window_y_override = true;
                window_y_range = extract_pair(optarg);
                break;

            case 'r': // wx
                window_x_override = true;
                window_x_range = extract_pair(optarg);
                break;

            case '?':
                if (optopt == 'q')
                    cerr << "need argument for option -ty" << endl;
                else if (optopt == 'w')
                    cerr << "need argument for option -tx" << endl;
                else if (optopt == 'e')
                    cerr << "need argument for option -wy" << endl;
                else if (optopt == 'r')
                    cerr << "need argument for option -wx" << endl;
                else
                    cerr << "unknown argument -" << (char) optopt << endl;
                return 1;
        }
    }

    std::string in_buffer;

    if (optind < argc) // read from argument
    {
        ifstream in_file{argv[optind]};
        if (!in_file.is_open())
            FATAL("Could not open file %s", argv[optind]);

        int ch;
        while ((ch = in_file.get()) != EOF)
        {
            in_buffer.push_back(ch);
        }
        in_file.close();
    }
    else
    {
        int ch;
        while ((ch = cin.get()) != EOF)
        {
            in_buffer.push_back(ch);
        }
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

    if (!terrain_y_override)
        terrain_y_range = (terrain_range["Yrange"]);
    if (!terrain_x_override)
        terrain_x_range = (terrain_range["Xrange"]);

    if (!window_y_override)
    {
        if (terrain_y_override)
            window_y_range = terrain_y_range;
        else
            window_y_range = (window_range["Yrange"]);
    }
    if (!window_x_override)
    {
        if (terrain_x_override)
            window_x_range = terrain_x_range;
        else
            window_x_range = (window_range["Xrange"]);
    }

    if (terrain_y_range.first > terrain_y_range.second || terrain_x_range.first > terrain_x_range.second)
        FATAL("terrain value must be low..high, got high..low");

    if (window_y_range.first > window_y_range.second || window_x_range.first > window_x_range.second)
        FATAL("window value must be low..high, got high..low");

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

    if (input["Colors"].get_type() != STRUCT)
        FATAL("missing Colors definition or incorrect value type");

    Struct colors{input["Colors"]};

    if (colors["Alive"].get_type() != TRIPLE || colors["Dead"].get_type() != TRIPLE)
        FATAL("missing Alive and Dead definition or incorrect value type");

    array<int, 3> alive = (array<int, 3>) colors["Alive"], dead = (array<int, 3>) colors["Dead"];

    cerr << alive[0];

    string title;
    if (input["Name"].get_type() == STRING)
        title = (string) input["Name"];

    for (int i = 0; i < generation; i++)
        sim.simulate();

    QImage *img = generate_terrain(sim, alive, dead, window_y_range, window_x_range);

    QApplication app(argc, argv);
    QWidget *window = new QWidget;
    window->setWindowTitle(title.c_str());

    LifeGrid *l = new LifeGrid();
    l->setIconImage(*img);
    delete img;
    l->setZoomFactor(20);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(l);
    window->setLayout(layout);
    window->show();
    return app.exec();
}

