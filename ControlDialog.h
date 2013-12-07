#ifndef CONTROL_DIALOG_H
#define CONTROL_DIALOG_H
#include <array>
#include <utility>
#include <QDialog>
#include <QSpinBox>
#include <QSlider>
#include "simulator.h"
#include "LifeGrid.h"

class ControlDialog : public QDialog
{
    Q_OBJECT

    public:
        ControlDialog(Simulator *sim, LifeGrid *l, std::array<std::array<int, 3>, 4> &table,
                        std::pair<int, int> &window_y_range, std::pair<int, int> &window_x_range);

    protected:
        virtual void closeEvent(QCloseEvent*);

    private slots:
        void quitApplication();
        void step();
        void gridSizeChanged(int);

    private:
        Simulator *sim;
        LifeGrid *l;
        std::array<std::array<int, 3>, 4> &table;
        std::pair<int, int> &window_y_range, &window_x_range;
        unsigned int delay = 1000, grid_size = 10;
        // Widgets
        QSpinBox *grid_size_spin_box, *delay_spin_box;
        QSlider *grid_size_slider, *delay_slider;
};

#endif /* !CONTROL_DIALOG_H */