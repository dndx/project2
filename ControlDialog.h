#ifndef CONTROL_DIALOG_H
#define CONTROL_DIALOG_H
#include <array>
#include <utility>
#include <QDialog>
#include <QSpinBox>
#include <QSlider>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include "simulator.h"
#include "LifeGrid.h"

class ControlDialog : public QDialog
{
    Q_OBJECT

    public:
        ControlDialog(Simulator *sim, LifeGrid *l, std::array<std::array<int, 3>, 4> &table,
                        std::pair<int, int> &window_y_range, std::pair<int, int> &window_x_range, int grid_size, QWidget *parent = 0);

    protected:
        virtual void closeEvent(QCloseEvent*);
        void refresh();

    private slots:
        void quitApplication();
        void step();
        void restart();
        void startOrStop();
        void gridSizeChanged(int);
        void delayChanged(int new_value);

    private:
        Simulator *sim;
        LifeGrid *l;
        std::array<std::array<int, 3>, 4> &table;
        std::pair<int, int> &window_y_range, &window_x_range;
        const unsigned int delay = 1000;
        // Widgets
        QSpinBox *grid_size_spin_box, *delay_spin_box;
        QSlider *grid_size_slider, *delay_slider;
        QLabel *generation_text;
        QTimer *timer;
        QPushButton *control_play, *control_step;
};

#endif /* !CONTROL_DIALOG_H */
