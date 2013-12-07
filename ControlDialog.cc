#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QApplication>
#include <QCloseEvent>
#include "ControlDialog.h"

using namespace std;

ControlDialog::ControlDialog(Simulator *sim, LifeGrid *l, 
                             array<array<int, 3>, 4> &table,
                             pair<int, int> &window_y_range,
                             pair<int, int> &window_x_range) :
    QDialog(), sim(sim), l(l), table(table), 
    window_y_range(window_y_range), window_x_range(window_x_range)
{
    QHBoxLayout *grid_size_layout = new QHBoxLayout;

    QLabel *grid_size_text = new QLabel("Grid size:");
    grid_size_layout->addWidget(grid_size_text);

    grid_size_spin_box = new QSpinBox;
    grid_size_spin_box->setRange(1, 50);
    grid_size_spin_box->setValue(grid_size);
    connect(grid_size_spin_box, SIGNAL(valueChanged(int)), this, SLOT(gridSizeChanged(int)));
    grid_size_layout->addWidget(grid_size_spin_box);

    grid_size_slider = new QSlider(Qt::Horizontal);
    grid_size_slider->setRange(1, 50);
    grid_size_slider->setValue(grid_size);
    connect(grid_size_slider, SIGNAL(valueChanged(int)), this, SLOT(gridSizeChanged(int)));
    grid_size_layout->addWidget(grid_size_slider);


    QHBoxLayout *delay_layout = new QHBoxLayout;
    QLabel *delay_text = new QLabel("Delay:");
    delay_layout->addWidget(delay_text);
    delay_spin_box = new QSpinBox;
    delay_spin_box->setRange(0, 10000);
    delay_spin_box->setValue(delay);
    delay_layout->addWidget(delay_spin_box);
    delay_slider = new QSlider(Qt::Horizontal);
    delay_slider->setRange(0, 10000);
    delay_slider->setValue(delay);
    delay_layout->addWidget(delay_slider);

    QLabel *generation_text = new QLabel("Generation: 0");

    QHBoxLayout *control_layout = new QHBoxLayout;
    QPushButton *control_quit = new QPushButton("Quit");
    connect(control_quit, SIGNAL(released()), this, SLOT(quitApplication()));
    QPushButton *control_restart = new QPushButton("Restart");
    QPushButton *control_play = new QPushButton("Play");
    QPushButton *control_step = new QPushButton("Step");
    connect(control_step, SIGNAL(released()), this, SLOT(step()));
    control_layout->addWidget(control_quit);
    control_layout->addWidget(control_restart);
    control_layout->addWidget(control_play);
    control_layout->addWidget(control_step);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(grid_size_layout);
    layout->addLayout(delay_layout);
    layout->addWidget(generation_text);
    layout->addLayout(control_layout);

    setLayout(layout);
    setWindowTitle("Controls");
}

void ControlDialog::closeEvent(QCloseEvent *event)
{
    quitApplication();
}

void ControlDialog::quitApplication()
{
    QApplication::quit();
}

void ControlDialog::step()
{
    sim->simulate();
    QImage *img = sim->generate_qt_terrain(table, window_y_range, window_x_range);
    l->setIconImage(*img);
    delete img;
}

void ControlDialog::gridSizeChanged(int new_value)
{
    grid_size_slider->setValue(new_value);
    grid_size_spin_box->setValue(new_value);
}

