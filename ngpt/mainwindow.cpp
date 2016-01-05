#include <QPushButton>
#include <QCoreApplication>
#include <QMessageBox>

#include <qregexp.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
//#include <qprinter.h>
//#include <qprintdialog.h>
#include <qfiledialog.h>
#include <qimagewriter.h>
#include <qfileinfo.h>
#include <qlayout.h>

#ifdef QT_SVG_LIB
    #include <qsvggenerator.h>
#endif

#include <qwt_polar_panner.h>
#include <qwt_polar_magnifier.h>
#include <qwt_polar_renderer.h>

#include "mainwindow.hpp"
#include "plot.hpp"
#include "settingseditor.hpp"
#include "pixmaps.hpp"

#include <iostream>

MainWindow::MainWindow(QWidget* parent)
: QMainWindow( parent )
{
    d_atx = NULL;

    QWidget* w = new QWidget( this );

    d_plot = new Plot( w );

    d_panner = new QwtPolarPanner( d_plot->canvas() );
    d_panner->setEnabled( false );

    d_zoomer = new QwtPolarMagnifier( d_plot->canvas() );
    d_zoomer->setEnabled( false );

    d_settingsEditor = new SettingsEditor( w );

    d_settingsEditor->showSettings( d_plot->settings() );
    connect( d_settingsEditor,
             SIGNAL(edited(const PlotSettings&) ),
             d_plot,
             SLOT(applySettings(const PlotSettings&) ) );

    QHBoxLayout* layout = new QHBoxLayout( w );
    layout->addWidget( d_settingsEditor, 0 );
    layout->addWidget( d_plot, 10 );

    setCentralWidget( w );

    QToolBar* toolBar = new QToolBar( this );

    QToolButton* btnZoom = new QToolButton(toolBar);

    const QString zoomHelp =
        "Use the wheel to zoom in/out.\n"
        "When the plot is zoomed in,\n"
        "use the left mouse button to move it.";

    btnZoom->setText( "Zoom" );
    btnZoom->setIcon( QPixmap( zoom_xpm ) );
    btnZoom->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
    btnZoom->setToolTip( zoomHelp );
    btnZoom->setCheckable( true );
    toolBar->addWidget( btnZoom );
    connect( btnZoom,
             SIGNAL(toggled(bool)),
             SLOT(enableZoomMode(bool)) );

    QToolButton* btnPrint = new QToolButton(toolBar);
    btnPrint->setText( "Print" );
    btnPrint->setIcon( QPixmap( print_xpm ) );
    btnPrint->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
    toolBar->addWidget( btnPrint );
    connect( btnPrint,
             SIGNAL( clicked() ),
             SLOT( printDocument() ) );

#ifdef QT_SVG_LIB
    QToolButton* btnExport = new QToolButton(toolBar);
    btnExport->setText( "Export" );
    btnExport->setIcon( QPixmap( print_xpm ) );
    btnExport->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
    toolBar->addWidget( btnExport );

    connect( btnExport,
             SIGNAL(clicked()),
             SLOT(exportDocument()));
#endif

    QPushButton* btnOpen = new QPushButton("Select Antex", toolBar);
    toolBar->addWidget( btnOpen );
    //addToolBar( toolBar );
    connect( btnOpen,
             SIGNAL(clicked()),
             SLOT(set_antex_file()) );

    d_cmbBox = new QComboBox(toolBar);
    d_cmbBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    toolBar->addWidget(d_cmbBox);
    connect( d_cmbBox,
             SIGNAL(currentTextChanged(const QString&)),
             SLOT(set_current_antenna(const QString&)) );

    QPushButton* btnPlot = new QPushButton("Plot", toolBar);
    toolBar->addWidget( btnPlot );
    connect( btnPlot,
             SIGNAL(clicked()),
             SLOT(plot_pcv_pattern()) );
    
    addToolBar(toolBar);
}

void
MainWindow::printDocument()
{
    /*
    QPrinter printer( QPrinter::HighResolution );

    QString docName = d_plot->title().text();
    if ( !docName.isEmpty() )
    {
        docName.replace ( QRegExp ( QString::fromLatin1 ( "\n" ) ), tr ( " -- " ) );
        printer.setDocName ( docName );
    }

    printer.setCreator( "polar plot demo example" );
    printer.setOrientation( QPrinter::Landscape );

    QPrintDialog dialog( &printer );
    if ( dialog.exec() )
    {
        QwtPolarRenderer renderer;
        renderer.renderTo( d_plot, printer );
    }
    */
    return;
}

void
MainWindow::exportDocument()
{
    QString fileName = "polarplot.pdf";

    QwtPolarRenderer renderer;
    renderer.exportTo( d_plot, "polarplot.pdf" );
}

void
MainWindow::enableZoomMode(bool on)
{
    d_panner->setEnabled( on );
    d_zoomer->setEnabled( on );
}

void
MainWindow::set_antex_file()
{
    std::cout << "\nMainWindow::set_antex_file()";
    try {
        std::cout<<"\n\td_cmbBox->clear()";
    d_cmbBox->clear();

    QString atx_filename = QFileDialog::getOpenFileName(
                            this,
                            tr("Select Antex File"),
                            QCoreApplication::applicationDirPath(),
                            tr("Antex (*.atx *.ATX)")
                            );
    std::cout<<"\n\tChecking d_atx";
    if ( d_atx != NULL )
    {
        delete d_atx;
    }

    std::cout<<"\n\tCreating d_atx";
    d_atx = new ngpt::antex (atx_filename.toStdString().c_str());

    std::cout<<"\n\tCreating pairs";
    std::vector<ant_pos_pair> pairs = d_atx->get_antenna_list();
    
    for ( auto i : pairs )
    {
        d_cmbBox->addItem( i.first.to_string().c_str() );
        std::cout <<"\n\t\tAdding " << i.first.to_string() << " to combo.";
    }

    } catch(...)
    {
        std::cerr<<"\nEXCEPTION in MainWindow::set_antex_file()";
    }

}

void
MainWindow::set_current_antenna(const QString& ant)
{
    std::cout << "\nMainWindow::set_current_antenna()";
    try {
    if ( !cur_antenna )
    {
        cur_antenna = new QString;
    }
    *cur_antenna = ant;
    } catch (...)
    {
        std::cerr<<"\nEXCEPTION in MainWindow::set_current_antenna()";
    }
    std::cout <<"\nMainWindow::set_current_antenna() exit";
}

void
MainWindow::plot_pcv_pattern()
{
    std::cout << "\nMainWindow::plot_pcv_pattern()";
    try{
    if ( !cur_antenna )
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("No antenna model selected!");
        msgBox.exec();
        return;
    }
    QString ant ( *cur_antenna );
    d_plot->plot_pcv(d_atx, ant);
    }catch(...)
    {
        std::cerr<<"\nEXCEPTION in MainWindow::plot_pcv_pattern()";
    }
    std::cout<<"\nMainWindow::plot_pcv_pattern() exit";
    return;
}
