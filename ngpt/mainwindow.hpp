#ifndef __MY_MAIN_QT_WINDOW__
#define __MY_MAIN_QT_WINDOW__

#include <qmainwindow.h>

class Plot;
class SettingsEditor;
class QwtPolarPanner;
class QwtPolarMagnifier;

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

private:
    Plot*              d_plot;
    QwtPolarPanner*    d_panner;
    QwtPolarMagnifier* d_zoomer;
    SettingsEditor*    d_settingsEditor;
};

#endif
