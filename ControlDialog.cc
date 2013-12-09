#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QCloseEvent>
#include <cstdio>
#include "ControlDialog.h"

using namespace std;

/*
 * Constructor for Control Dialog. 
 * simulator and window ranges are required for re-rendering
 * because C++ does not have closure :(
 */
ControlDialog::ControlDialog(Simulator *sim, LifeGrid *l, 
                             array<array<int, 3>, 4> &table,
                             pair<int, int> &window_y_range,
                             pair<int, int> &window_x_range, int grid_size, QWidget *parent) :
    QDialog(parent), sim(sim), l(l), table(table), 
    window_y_range(window_y_range), window_x_range(window_x_range)
{
    // add widgets and register events, nothing important

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
    connect(delay_spin_box, SIGNAL(valueChanged(int)), this, SLOT(delayChanged(int)));
    delay_layout->addWidget(delay_spin_box);

    delay_slider = new QSlider(Qt::Horizontal);
    delay_slider->setRange(0, 10000);
    delay_slider->setValue(delay);
    connect(delay_slider, SIGNAL(valueChanged(int)), this, SLOT(delayChanged(int)));
    delay_layout->addWidget(delay_slider);

    generation_text = new QLabel("Generation: 0");

    QHBoxLayout *control_layout = new QHBoxLayout;
    QPushButton *control_quit = new QPushButton("Quit");
    connect(control_quit, SIGNAL(released()), this, SLOT(quitApplication()));
    QPushButton *control_restart = new QPushButton("Restart");
    connect(control_restart, SIGNAL(released()), this, SLOT(restart()));
    control_play = new QPushButton("Play");
    connect(control_play, SIGNAL(released()), this, SLOT(startOrStop()));
    control_step = new QPushButton("Step");
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

    timer = new QTimer;
    timer->setSingleShot(false);
    connect(timer, SIGNAL(timeout()), this, SLOT(step()));

    setLayout(layout);
    setWindowTitle("Controls");

    // refresh generation label in case user started with certain generation
    refresh();
}

/*
 * Event handler for "Quit" button
 */
void ControlDialog::closeEvent(QCloseEvent *event)
{
    quitApplication();
}

/*
 * Utility function to quit the application
 */
void ControlDialog::quitApplication()
{
    QApplication::quit();
}

/*
 * Update the grid widget and the generation label
 */
void ControlDialog::refresh()
{
    QImage *img = sim->generate_qt_terrain(table, window_y_range, window_x_range);
    l->setIconImage(*img);
    delete img;
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "Generation: %lu", sim->get_generation());
    generation_text->setText(buffer);
}

/*
 * Event handler for "Step" button
 */
void ControlDialog::step()
{
    sim->simulate();
    refresh();
}

/*
 * Event handler for grid size spin box and scroll bar
 */
void ControlDialog::gridSizeChanged(int new_value)
{
    grid_size_slider->setValue(new_value);
    grid_size_spin_box->setValue(new_value);
    l->setZoomFactor(new_value);
}

/*
 * Event handler for delay spin box and scroll bar
 */
void ControlDialog::delayChanged(int new_value)
{
    delay_slider->setValue(new_value);
    delay_spin_box->setValue(new_value);
    timer->setInterval(new_value);
}

/*
 * Event handler for "Restart" button
 */
void ControlDialog::restart()
{
    sim->reset();
    refresh();
}

/*
 * Event handler for Start/Stop button,
 * do different things depends on Timer status
 */
void ControlDialog::startOrStop()
{
    if (timer->isActive())
    {
        timer->stop();
        control_play->setText("Play");
        control_step->setEnabled(true);
    }
    else
    {
        timer->start(delay_slider->value());
        control_play->setText("Pause");
        control_step->setEnabled(false);
    }
}

