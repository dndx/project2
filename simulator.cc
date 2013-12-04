#include "simulator.h"
#include "utils.h"
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

void Simulator::set_status(int y, int x, char status)
{
    grid[to_y(y)][to_x(x)] = status;
}

char Simulator::get_status(int y, int x)
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

vector<vector<char>>::const_iterator Simulator::cbegin()
{
    return grid.cbegin();
}

vector<vector<char>>::const_iterator Simulator::cend()
{
    return grid.cend();
}

vector<vector<char>>::const_reverse_iterator Simulator::crbegin()
{
    return grid.crbegin();
}

vector<vector<char>>::const_reverse_iterator Simulator::crend()
{
    return grid.crend();
}

vector<vector<char>>::size_type Simulator::size() const noexcept
{
    return grid.size();
}

vector<char>::size_type Simulator::xsize() const noexcept
{
    if (size() > 0)
        return grid[0].size();
    else
        return 0;
}

vector<vector<char>>::const_reference Simulator::operator[] (vector<vector<char>>::size_type n) const
{
    return grid[n];
}

/*
 * table is the char for index 0-3
 */
void Simulator::show(array<unsigned char, 4> &table, pair<int, int> yrange, pair<int, int> xrange)
{
    yrange.first = to_y(yrange.first);
    yrange.second = to_y(yrange.second);
    xrange.first = to_x(xrange.first);
    xrange.second = to_x(xrange.second);

    for (int i = yrange.second; i >= yrange.first; --i)
    {
        if ((unsigned int) i < size() && i >= 0) // within terrain range
        {
            for (int i = xrange.first; i < 0; i++) // prefix cells not in terrain
                cout << table[0];

            for (auto ix = grid[i].cbegin(); ix != grid[i].cend(); ++ix)
            {
                int x_index = ix - grid[i].cbegin();

                if (x_index < xrange.first || x_index > xrange.second)
                    continue;

                if (*ix >= 0 && *ix < 4)
                {
                    cout << table[*ix];
                }
                else
                {
                    FATAL("state value overflow");
                }
            }

            for (int i = xsize() - 1; i < xrange.second; i++) // postfix cells not in terrain
                cout << table[0];
        }
        else // not in terrain range
        {
            for (int i = 0; i < xrange.second - xrange.first + 1; ++i)
                cout << table[0];
        }
        cout << endl;
    }

    return;
}

QImage *Simulator::generate_qt_terrain(array<array<int, 3>, 4> &table, pair<int, int> yrange, pair<int, int> xrange)
{
    yrange.first = to_y(yrange.first);
    yrange.second = to_y(yrange.second);
    xrange.first = to_x(xrange.first);
    xrange.second = to_x(xrange.second);

    QImage *img = new QImage(xrange.second - xrange.first + 1, 
                             yrange.second - yrange.first + 1, QImage::Format_ARGB32);
    img->fill(QColor(table[0][0], table[0][1], table[0][2]));

    for (auto iy = cbegin(); iy != cend(); ++iy)
    {
        int y = iy - cbegin();
        if (y < yrange.first || y > yrange.second)
            continue;

        for (auto ix = iy->cbegin(); ix != iy->cend(); ++ix)
        {
            int x = ix - iy->cbegin();
            if (x < xrange.first || x > xrange.second)
                continue;
            if (*ix >= 0 && *ix < 4)
            {
                img->setPixel(x - xrange.first, yrange.second - y, QColor(table[*ix][0], table[*ix][1], table[*ix][2]).rgba());
            }
        }
    }

    return img;
}

GoLSimulator::GoLSimulator(int y_low, int y_high, int x_low, int x_high) :
    Simulator(y_low, y_high, x_low, x_high) {}

void GoLSimulator::simulate()
{
    std::vector<std::vector<char>> next_gen{grid};
    
    for (auto iy = grid.cbegin(); iy != grid.cend(); ++iy)
    {
        unsigned int y = iy - grid.cbegin();

        for (auto ix = iy->cbegin(); ix != iy->cend(); ++ix)
        {
            unsigned int x = ix - iy->cbegin();
            unsigned int neighbors = count_live_neighbors(y, x);

            //cout << y << ' ' << x << " is " << neighbors << endl;

            if (*ix == STATE_ALIVE) // alive
            {
                if (neighbors < 2 || neighbors > 3)
                    next_gen[y][x] = STATE_DEAD;
            }
            else // dead
            {
                if (neighbors == 3)
                    next_gen[y][x] = STATE_ALIVE;
            }
        }
    }

    grid = move(next_gen);
}

unsigned int GoLSimulator::count_live_neighbors(int y, int x)
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
            if (grid[iy][ix] == STATE_ALIVE)
                result++;
        }
    }
    return result;
}

void GoLSimulator::show_file(pair<string, Struct> &input, pair<int, int> terrain_y_range, pair<int, int> terrain_x_range,
                          pair<int, int> window_y_range, pair<int, int> window_x_range)
{
    Struct tmp;
    
    // write terrain struct
    tmp["Xrange"] = terrain_x_range;
    tmp["Yrange"] = terrain_y_range;
    input.second["Terrain"] = tmp;

    // write window struct
    tmp = Struct();
    tmp["Xrange"] = window_x_range;
    tmp["Yrange"] = window_y_range;
    input.second["Window"] = tmp;

    List alive;

    for (auto iy = cbegin(); iy != cend(); ++iy)
    {
        int y = terrain_y_range.first + (iy - cbegin());
        
        if (y < window_y_range.first || y > window_y_range.second)
            continue;

        for (auto ix = iy->cbegin(); ix != iy->cend(); ++ix)
        {
            int x = terrain_x_range.first + (ix - iy->cbegin());

            if (x < window_x_range.first || x > window_x_range.second)
                continue;

            if (*ix) // alive
            {
                alive.push_back(pair<int, int>(x, y));
            }
        }
    }

    // alive cells
    tmp = Struct();
    
    if (alive.size())
        tmp["Alive"] = alive;
    input.second["Initial"] = tmp;

    cout << "Life" << " = " << input.second << ';' << endl;
}

WWSimulator::WWSimulator(int y_low, int y_high, int x_low, int x_high) :
    Simulator(y_low, y_high, x_low, x_high) {}

void WWSimulator::simulate()
{
    std::vector<std::vector<char>> next_gen{grid};
    
    for (auto iy = grid.cbegin(); iy != grid.cend(); ++iy)
    {
        unsigned int y = iy - grid.cbegin();

        for (auto ix = iy->cbegin(); ix != iy->cend(); ++ix)
        {
            unsigned int x = ix - iy->cbegin();
            unsigned int neighbors = count_head_neighbors(y, x);

            //cout << y << ' ' << x << " is " << neighbors << endl;

            switch (*ix)
            {
                case STATE_EMPTY:
                    break;

                case STATE_HEAD:
                    next_gen[y][x] = STATE_TAIL;
                    break;

                case STATE_TAIL:
                    next_gen[y][x] = STATE_WIRE;
                    break;

                case STATE_WIRE:
                    if (neighbors == 1 || neighbors == 2)
                        next_gen[y][x] = STATE_HEAD;
            }
        }
    }

    grid = move(next_gen);
}

unsigned int WWSimulator::count_head_neighbors(int y, int x)
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
            if (grid[iy][ix] == STATE_HEAD)
                result++;
        }
    }
    return result;
}

void WWSimulator::show_file(pair<string, Struct> &input, pair<int, int> terrain_y_range, pair<int, int> terrain_x_range,
                          pair<int, int> window_y_range, pair<int, int> window_x_range)
{
    Struct tmp;
    
    // write terrain struct
    tmp["Xrange"] = terrain_x_range;
    tmp["Yrange"] = terrain_y_range;
    input.second["Terrain"] = tmp;

    // write window struct
    tmp = Struct();
    tmp["Xrange"] = window_x_range;
    tmp["Yrange"] = window_y_range;
    input.second["Window"] = tmp;

    List head, tail, wire;

    for (auto iy = cbegin(); iy != cend(); ++iy)
    {
        int y = terrain_y_range.first + (iy - cbegin());
        
        if (y < window_y_range.first || y > window_y_range.second)
            continue;

        for (auto ix = iy->cbegin(); ix != iy->cend(); ++ix)
        {
            int x = terrain_x_range.first + (ix - iy->cbegin());

            if (x < window_x_range.first || x > window_x_range.second)
                continue;

            switch (*ix)
            {
                case STATE_HEAD:
                    head.push_back(pair<int, int>(x, y));
                    break;
                    
                case STATE_TAIL:
                    tail.push_back(pair<int, int>(x, y));
                    break;

                case STATE_WIRE:
                    wire.push_back(pair<int, int>(x, y));
                    break;
            }
        }
    }

    // alive cells
    tmp = Struct();
    
    if (head.size())
        tmp["Head"] = head;
    if (tail.size())
        tmp["Tail"] = tail;
    if (wire.size())
        tmp["Wire"] = wire;

    input.second["Initial"] = tmp;

    cout << "WireWorld" << " = " << input.second << ';' << endl;
}

ECSimulator::ECSimulator(int y_low, int y_high, int x_low, int x_high, unsigned char rule) :
    Simulator(y_low, y_high, x_low, x_high)
{
    for (int i = 0; i < 8; ++i)
    {
        code[i] = rule & (0x01U << i);
    }
}

void ECSimulator::simulate()
{
    std::vector<std::vector<char>> next_gen{grid};
    
    for (auto iy = grid.cbegin(); iy != grid.cend(); ++iy)
    {
        unsigned int y = iy - grid.cbegin();

        for (auto ix = iy->cbegin(); ix != iy->cend(); ++ix)
        {
            unsigned int x = ix - iy->cbegin();

            if (*ix == STATE_ZERO)
            {
                unsigned char new_state = determine_state(y, x);
                next_gen[y][x] = new_state;
            }
        }
    }

    grid = move(next_gen);
}

unsigned char ECSimulator::determine_state(int y, int x)
{
    unsigned char pattern{0};

    unsigned int iy = y + 1; // above row
    if (iy <= to_y(y_high))
    {
        for (int ix = max(x - 1, 0); ix <= min(x + 1, (int) to_x(x_high)); ++ix)
        {
            pattern |= (grid[iy][ix] & 0x01U) << (2 - (ix - (x - 1)));
        }
        return code[pattern];
    }
    return code[0];
}

void ECSimulator::show_file(pair<string, Struct> &input, pair<int, int> terrain_y_range, pair<int, int> terrain_x_range,
                          pair<int, int> window_y_range, pair<int, int> window_x_range)
{
    Struct tmp;
    
    // write terrain struct
    tmp["Xrange"] = terrain_x_range;
    tmp["Yrange"] = terrain_y_range;
    input.second["Terrain"] = tmp;

    // write window struct
    tmp = Struct();
    tmp["Xrange"] = window_x_range;
    tmp["Yrange"] = window_y_range;
    input.second["Window"] = tmp;

    List one;

    for (auto iy = cbegin(); iy != cend(); ++iy)
    {
        int y = terrain_y_range.first + (iy - cbegin());
        
        if (y < window_y_range.first || y > window_y_range.second)
            continue;

        for (auto ix = iy->cbegin(); ix != iy->cend(); ++ix)
        {
            int x = terrain_x_range.first + (ix - iy->cbegin());

            if (x < window_x_range.first || x > window_x_range.second)
                continue;

            if (*ix == STATE_ONE)
                one.push_back(pair<int, int>(x, y));
        }
    }

    // alive cells
    tmp = Struct();
    
    if (one.size())
        tmp["One"] = one;

    input.second["Initial"] = tmp;

    cout << "Elementary" << " = " << input.second << ';' << endl;
}

