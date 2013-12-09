#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cstdio>
#include <QHBoxLayout>
#include <QApplication>
#include <QScrollArea>
#include "utils.h"
#include "miner_parser.h"
#include "simulator.h"
#include "LifeGrid.h"
#include "ControlDialog.h"

using namespace std;

int main(int argc, char *argv[])
{
    opterr = 0; // mute getopt error meesage
    int c;
    int generation{0}, grid_size{10};
    bool terrain_x_override{false}, terrain_y_override{false};
    bool window_x_override{false}, window_y_override{false};
    bool show_control{false};
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

    while ((c = getopt(argc, argv, "g:hcq:w:e:r:s:")) != -1)
    {
        switch (c)
        {
            case 'h':
                cerr << "Usage: life_gui [-h] [-c] [-g n] [-s n] [-tx l..h] [-ty l..h] [-wx l..h] [-wy l..h] [filename]" << endl << endl
                     << "  -c        Show the simulation control window" << endl
                     << "  -g n      Specify the desired generation number. If omitted, the default will be n = 0" << endl
                     << "  -h        Display this message and quit" << endl
                     << "  -s n      Specify the size of each cell, in pixel, defult: 10" << endl
                     << "  -tx l..h  Set the x range of the terrain; overrides values specified in the input file" << endl
                     << "  -ty l..h  Set the y range of the terrain; overrides values specified in the input file" << endl
                     << "            dead and alive cells" << endl
                     << "  -wx l..h  Set the x range of the output window; otherwise this defaults to the x range of the terrain" << endl
                     << "  -wy l..h  Set the y range of the output window; otherwise this defaults to the x range of the terrain" << endl
                     << endl;
                return 0;

            case 'c':
                show_control = true;
                break;

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

            case 's':
                grid_size = atoi(optarg);
                if (grid_size <= 0)
                    FATAL("grid size muse be at least 1 pixel");
                if (grid_size > 50)
                {
                    LOGI("grid size is too big, capped to 50 pixel");
                    grid_size = 50;
                }
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

    pair<string, Struct> input = parse_miner_string(in_buffer);

    if (input.second["Terrain"].get_type() != STRUCT)
    {
        FATAL("missing Terrain definition or incorrect value type for Terrain");
    }

    Struct terrain_range{input.second["Terrain"]};

    if (terrain_range["Xrange"].get_type() != RANGE || terrain_range["Yrange"].get_type() != RANGE)
    {
        FATAL("Xrange and Yrange for Terrain must be range type");
    }

    Struct window_range;

    if (!input.second.count("Window"))
    {
        window_range = terrain_range;
    }
    else
    {
        if (input.second["Window"].get_type() != STRUCT)
            FATAL("incorrect type of Window");

        window_range = input.second["Window"];

        if ((window_range["Xrange"].get_type() != RANGE && window_range["Xrange"].get_type() != INVALID) ||
            (window_range["Yrange"].get_type() != RANGE && window_range["Yrange"].get_type() != INVALID))
            FATAL("Xrange or Yrange of Window has incorrect type");

        if (window_range["Xrange"].get_type() != RANGE)
            window_range["Xrange"] = terrain_range["Xrange"];

        if (window_range["Yrange"].get_type() != RANGE)
            window_range["Yrange"] = terrain_range["Yrange"];
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

    Simulator *sim{nullptr};
    array<array<int, 3>, 4> table;

    if (!input.first.compare("Life"))
    {
        sim = new GoLSimulator{terrain_y_range.first, terrain_y_range.second,
                      terrain_x_range.first, terrain_x_range.second};

        if (input.second["Initial"].get_type() != STRUCT)
            FATAL("missing Initial defination or incorrect value type for Initial");

        Struct initial{input.second["Initial"]};
        if (initial["Alive"].get_type() == LIST)
        {
            List initial_alive{initial["Alive"]};

            for (unsigned int i = 0; i < initial_alive.size(); ++i)
            {
                pair<int, int> coord{initial_alive[i]};

                if (coord.first < terrain_x_range.first || coord.first > terrain_x_range.second)
                    FATAL("initial cell x coordinate out of bound, got %d", coord.first);

                if (coord.second < terrain_y_range.first || coord.second > terrain_y_range.second)
                    FATAL("initial cell y coordinate out of bound, got %d", coord.second);

                sim->set_status(coord.second, coord.first, GoLSimulator::STATE_ALIVE);
            }
        }
        else if (initial["Alive"].get_type() != INVALID)
            FATAL("incorrect type for Alive assignment");

        if (input.second["Colors"].get_type() != STRUCT)
            FATAL("missing Colors definition or incorrect value type");

        Struct colors{input.second["Colors"]};

        if (colors["Alive"].get_type() != TRIPLE || colors["Dead"].get_type() != TRIPLE)
            FATAL("missing Alive and Dead definition or incorrect value type");

        table[0] = colors["Dead"];
        table[1] = colors["Alive"];
    }
    else if (!input.first.compare("WireWorld"))
    {
        sim = new WWSimulator{terrain_y_range.first, terrain_y_range.second,
                      terrain_x_range.first, terrain_x_range.second};

        if (input.second["Initial"].get_type() != STRUCT)
            FATAL("missing Initial defination or incorrect value type for Initial");

        Struct initial{input.second["Initial"]};
        if (initial["Head"].get_type() == LIST)
        {
            List initial_list{initial["Head"]};

            for (unsigned int i = 0; i < initial_list.size(); ++i)
            {
                pair<int, int> coord{initial_list[i]};

                if (coord.first < terrain_x_range.first || coord.first > terrain_x_range.second)
                    FATAL("initial cell x coordinate out of bound, got %d", coord.first);

                if (coord.second < terrain_y_range.first || coord.second > terrain_y_range.second)
                    FATAL("initial cell y coordinate out of bound, got %d", coord.second);

                sim->set_status(coord.second, coord.first, WWSimulator::STATE_HEAD);
            }
        }

        if (initial["Tail"].get_type() == LIST)
        {
            List initial_list{initial["Tail"]};

            for (unsigned int i = 0; i < initial_list.size(); ++i)
            {
                pair<int, int> coord{initial_list[i]};

                if (coord.first < terrain_x_range.first || coord.first > terrain_x_range.second)
                    FATAL("initial cell x coordinate out of bound, got %d", coord.first);

                if (coord.second < terrain_y_range.first || coord.second > terrain_y_range.second)
                    FATAL("initial cell y coordinate out of bound, got %d", coord.second);

                sim->set_status(coord.second, coord.first, WWSimulator::STATE_TAIL);
            }
        }

        if (initial["Wire"].get_type() == LIST)
        {
            List initial_list{initial["Wire"]};

            for (unsigned int i = 0; i < initial_list.size(); ++i)
            {
                pair<int, int> coord{initial_list[i]};

                if (coord.first < terrain_x_range.first || coord.first > terrain_x_range.second)
                    FATAL("initial cell x coordinate out of bound, got %d", coord.first);

                if (coord.second < terrain_y_range.first || coord.second > terrain_y_range.second)
                    FATAL("initial cell y coordinate out of bound, got %d", coord.second);

                sim->set_status(coord.second, coord.first, WWSimulator::STATE_WIRE);
            }
        }

        if (input.second["Colors"].get_type() != STRUCT)
            FATAL("missing Colors definition or incorrect value type");

        Struct colors{input.second["Colors"]};

        if (colors["Empty"].get_type() != TRIPLE || colors["Head"].get_type() != TRIPLE ||
            colors["Wire"].get_type() != TRIPLE || colors["Tail"].get_type() != TRIPLE)
            FATAL("missing Empty or Head or Wire or Tail definition or incorrect value type");

        table[0] = colors["Empty"];
        table[1] = colors["Head"];
        table[2] = colors["Tail"];
        table[3] = colors["Wire"];
    }
    else if (!input.first.compare("Elementary"))
    {
        if (input.second["Rule"].get_type() != INTEGER)
            FATAL("missing Rule definition or incorrect value type for Rule");

        if (input.second["Rule"] < 0 || input.second["Rule"] > 255)
            FATAL("Rule must be greater or equal to 0 and less and equal to 255");

        sim = new ECSimulator{terrain_y_range.first, terrain_y_range.second,
                      terrain_x_range.first, terrain_x_range.second, input.second["Rule"]};

        if (input.second["Initial"].get_type() != STRUCT)
            FATAL("missing Initial defination or incorrect value type for Initial");

        Struct initial{input.second["Initial"]};
        if (initial["One"].get_type() == LIST)
        {
            List initial_one{initial["One"]};

            for (unsigned int i = 0; i < initial_one.size(); ++i)
            {
                pair<int, int> coord{initial_one[i]};

                if (coord.first < terrain_x_range.first || coord.first > terrain_x_range.second)
                    FATAL("initial cell x coordinate out of bound, got %d", coord.first);

                if (coord.second < terrain_y_range.first || coord.second > terrain_y_range.second)
                    FATAL("initial cell y coordinate out of bound, got %d", coord.second);

                sim->set_status(coord.second, coord.first, ECSimulator::STATE_ONE);
            }
        }
        else if (initial["One"].get_type() != INVALID)
            FATAL("incorrect type for Alive assignment");

        if (input.second["Colors"].get_type() != STRUCT)
            FATAL("missing colors definition or incorrect value type");

        Struct colors{input.second["Colors"]};

        if (colors["One"].get_type() != TRIPLE || colors["Zero"].get_type() != TRIPLE)
            FATAL("missing One or Zero definition or incorrect value type");

        table[0] = colors["Zero"];
        table[1] = colors["One"];
    }
    else
    {
        FATAL("unknown simulation type");
    }

    sim->set_reset();

    string title{" "};
    if (input.second["Name"].get_type() == STRING)
        title = (string) input.second["Name"];

    for (int i = 0; i < generation; i++)
        sim->simulate();

    QImage *img = sim->generate_qt_terrain(table, window_y_range, window_x_range);

    QApplication app(argc, argv);
    QWidget *window = new QWidget;
    window->setWindowTitle(title.c_str());

    QHBoxLayout *layout = new QHBoxLayout(window);
    layout->setSpacing(0);
    layout->setMargin(0);

    QScrollArea *scroll_area = new QScrollArea(window);
    scroll_area->setBackgroundRole(QPalette::Dark);
    scroll_area->setWidgetResizable(true);
    layout->addWidget(scroll_area);

    LifeGrid *l = new LifeGrid(scroll_area);
    l->setIconImage(*img);
    delete img;
    l->setZoomFactor(grid_size);
    scroll_area->setWidget(l);

    if (show_control)
    {
        ControlDialog *controls = new ControlDialog(sim, l, table, window_y_range, window_x_range, grid_size, window);
        controls->show();
    }

    window->setLayout(layout);
    window->show();
    return app.exec();
}

