#ifndef __MY_MAIN_QT_WINDOW__
#define __MY_MAIN_QT_WINDOW__

#include <QComboBox>

#include <qmainwindow.h>
#include "antenna.hpp"
#include "antex.hpp"

class Plot;
class SettingsEditor;
class QwtPolarPanner;
class QwtPolarMagnifier;

typedef std::ifstream::pos_type pos_type;
typedef std::pair<ngpt::antenna, pos_type> ant_pos_pair;

class MainWindow 
: public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = NULL);

private Q_SLOTS:
    void enableZoomMode(bool on);
    void printDocument();
    void exportDocument();
    void set_antex_file();
    void plot_pcv_pattern(const QString&);

private:
    Plot*              d_plot;
    QwtPolarPanner*    d_panner;
    QwtPolarMagnifier* d_zoomer;
    SettingsEditor*    d_settingsEditor;

    QComboBox*         d_cmbBox;

    ngpt::antex*              d_atx;
    std::vector<ant_pos_pair> d_antpos;
};

#endif
