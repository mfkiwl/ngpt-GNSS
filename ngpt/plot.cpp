#include <qpen.h>
#include <qwt_series_data.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <qwt_polar_grid.h>
#include <qwt_polar_curve.h>
#include <qwt_polar_marker.h>
#include <qwt_scale_engine.h>

// #include "antex.hpp"
// #include "antenna.hpp"

#include "plot.hpp"
#include <iostream>

const QwtInterval radialInterval ( 0.0,  90.0 );
const QwtInterval azimuthInterval( 0.0, 360.0 );

class Data: public QwtSeriesData<QwtPointPolar>
{
public:
    Data( const QwtInterval &radialInterval,
          const QwtInterval &azimuthInterval, size_t size ):
        d_radialInterval( radialInterval ),
        d_azimuthInterval( azimuthInterval ),
        d_size( size )
    {
    }

    virtual size_t size() const
    {
        return d_size;
    }

protected:
    QwtInterval d_radialInterval;
    QwtInterval d_azimuthInterval;
    size_t d_size;
};

class PcvData
: public QwtSeriesData<QwtPointPolar>
{

public:
    PcvData(std::vector<QwtPointPolar> crd, std::vector<float> pcv)
    : crd_(std::move(crd)),
      pcv_(std::move(pcv))
    {}

    virtual std::size_t
    size()
    const
    { return pcv_.size(); }

    virtual QwtPointPolar
    sample(std::size_t i)
    const
    {
        if (i>=crd_.size())
        {
            std::cout <<"\nWTF????????? Out of range!";
            return QwtPointPolar(0,0);
        }
        return crd_[i]; 
    }

    virtual QRectF
    boundingRect()
    const
    {
        if ( d_boundingRect.width() < 0.0 )
            d_boundingRect = qwtBoundingRect( *this );

        return d_boundingRect;
    }

protected:
    std::vector<QwtPointPolar> crd_;
    std::vector<float>         pcv_;
};

class SpiralData: public Data
{
public:
    SpiralData( const QwtInterval &radialInterval,
            const QwtInterval &azimuthInterval, size_t size ):
        Data( radialInterval, azimuthInterval, size )
    {
    }

    virtual QwtPointPolar sample( size_t i ) const
    {
        std::cout << "\nCalling QwtPointPolar sample() with i = " << i;
        const double stepA = 4 * d_azimuthInterval.width() / d_size;
        const double a = d_azimuthInterval.minValue() + i * stepA;

        const double stepR = d_radialInterval.width() / d_size;
        const double r = d_radialInterval.minValue() + i * stepR;

        return QwtPointPolar( a, r );
    }

    virtual QRectF boundingRect() const
    {
        if ( d_boundingRect.width() < 0.0 )
            d_boundingRect = qwtBoundingRect( *this );

        return d_boundingRect;
    }
};

/*class RoseData: public Data
{
public:
    RoseData( const QwtInterval &radialInterval,
            const QwtInterval &azimuthInterval, size_t size ):
        Data( radialInterval, azimuthInterval, size )
    {
    }

    virtual QwtPointPolar sample( size_t i ) const
    {
        const double stepA = d_azimuthInterval.width() / d_size;
        const double a = d_azimuthInterval.minValue() + i * stepA;

        const double d = a / 360.0 * M_PI;
        const double r = d_radialInterval.maxValue() * qAbs( qSin( 4 * d ) );

        return QwtPointPolar( a, r );
    }

    virtual QRectF boundingRect() const
    {
        if ( d_boundingRect.width() < 0.0 )
            d_boundingRect = qwtBoundingRect( *this );

        return d_boundingRect;
    }
};*/

Plot::Plot(QWidget* parent)
: QwtPolarPlot(QwtText( "Antenna Phase Center Variation Pattern" ), parent)
{
    d_curve = NULL;

    std::cout << "\nCalling Plot Constructor, Plot::Plot()";
    setAutoReplot( false );
    setPlotBackground( Qt::darkBlue );

    // scales
    setScale( QwtPolar::Azimuth,
        azimuthInterval.minValue(),
        azimuthInterval.maxValue(),
        azimuthInterval.width() / 12 );
    setScaleMaxMinor( QwtPolar::Azimuth, 2 );
    
    setScale( QwtPolar::Radius,
        radialInterval.minValue(),
        radialInterval.maxValue() );

    // grids, axes
    d_grid = new QwtPolarGrid();
    d_grid->setPen( QPen( Qt::white ) );
    for ( int scaleId = 0; scaleId < QwtPolar::ScaleCount; scaleId++ )
    {
        d_grid->showGrid( scaleId );
        d_grid->showMinorGrid( scaleId );

        QPen minorPen( Qt::gray );
#if 0
        minorPen.setStyle( Qt::DotLine );
#endif
        d_grid->setMinorGridPen( scaleId, minorPen );
    }
    d_grid->setAxisPen( QwtPolar::AxisAzimuth, QPen( Qt::black ) );

    d_grid->showAxis( QwtPolar::AxisAzimuth, true );
    d_grid->showAxis( QwtPolar::AxisLeft, false );
    d_grid->showAxis( QwtPolar::AxisRight, true );
    d_grid->showAxis( QwtPolar::AxisTop, true );
    d_grid->showAxis( QwtPolar::AxisBottom, false );
    d_grid->showGrid( QwtPolar::Azimuth, true );
    d_grid->showGrid( QwtPolar::Radius, true );
    d_grid->attach( this );

    /*
    // curves
    //for ( int curveId = 0; curveId < PlotSettings::NumCurves; curveId++ )
    //{
    //    d_curve[curveId] = createCurve( curveId );
    //    d_curve[curveId]->attach( this );
    std::cout << "\n[Plot::Plot()] Calling d_curve = createCurve()";
    d_curve = createCurve();
    std::cout << "\n[Plot::Plot()] Done creating curve.";
    d_curve->attach(this);
    std::cout << "\n[Plot::Plot()] curve attached.";
    //}
    */

    // markers
    /*
    QwtPolarMarker *marker = new QwtPolarMarker();
    marker->setPosition( QwtPointPolar( 57.3, 4.72 ) );
    marker->setSymbol( new QwtSymbol( QwtSymbol::Ellipse,
        QBrush( Qt::white ), QPen( Qt::green ), QSize( 9, 9 ) ) );
    marker->setLabelAlignment( Qt::AlignHCenter | Qt::AlignTop );

    QwtText text( "Marker" );
    text.setColor( Qt::black );
    QColor bg( Qt::white );
    bg.setAlpha( 200 );
    text.setBackgroundBrush( QBrush( bg ) );

    marker->setLabel( text );
    marker->attach( this );

    QwtLegend *legend = new QwtLegend;
    insertLegend( legend,  QwtPolarPlot::BottomLegend );
    */
    std::cout << "\nEnd of Construction, Plot::Plot()";
}

void
Plot::plot_pcv(ngpt::antex* atx, const QString& ant)
{
    std::cout<<"\nPlot::plot_pcv()";
    try{
    if ( atx )
    {
        if ( d_curve != NULL )
        {
            //delete d_curve->data();
            delete d_curve;
        }

        d_curve = NULL;
        d_curve = create_pcv_plot(atx, ant);

        if ( d_curve )
        {
            d_curve->attach( this );
        }
    }
    } catch (...)
    {
        std::cerr<<"\nEXCEPTION in Plot::plot_pcv()";
    }
    std::cout<<"\n...Going to replot!";
    replot();
    std::cout<<"\nPlot::plot_pcv() exit";
}

QwtPolarCurve* 
Plot::create_pcv_plot(ngpt::antex* atx, const QString& ant)
{
    std::cout <<"\nPlot::create_pcv_plot()";
    QwtPolarCurve *curve = new QwtPolarCurve();
    
    try{

    QByteArray array = ant.toLocal8Bit();
    ngpt::antenna antna ( array.data() );
    ngpt::antenna_pcv<ngpt::pcv_type> pcvs ( atx->get_antenna_pattern( antna ) );

    curve->setStyle( QwtPolarCurve::NoCurve );
    curve->setPen( QPen(Qt::yellow, 2) );
    curve->setSymbol( new QwtSymbol(QwtSymbol::Rect,
                                      QBrush(Qt::cyan),
                                      QPen(Qt::white),
                                      QSize(3, 3)) );
    std::vector<QwtPointPolar> polar_crd;
    std::vector<float>         pcv_vals;
    for ( float zen = .1; zen < 90.0 ; zen += 2.0 )
    {
        for ( float azi = .1; azi < 360.0 ; azi += 5.0 )
        {
            pcv_vals.emplace_back ( pcvs.azi_pcv(zen, azi, 0) );
            polar_crd.emplace_back( azi, zen );
        }
    }
    curve->setData( new PcvData(polar_crd, pcv_vals) );
    std::cout<<"\nNumber of points in curve: " << curve->data()->size();
    }
    catch(...)
    {
        std::cerr<<"\nEXCEPTION in Plot::create_pcv_plot()";
    }
    std::cout<<"\nPlot::create_pcv_plot() exit ("<<curve<<")";
    
    return curve;
}

QwtPolarCurve* 
Plot::createCurve()
const
{
    std::cout << "\nIn Plot::createCurve.";
    const int numPoints = 200;

    QwtPolarCurve *curve = new QwtPolarCurve();
    curve->setStyle( QwtPolarCurve::Lines );
    
    curve->setTitle( "Spiral" );
    curve->setPen( QPen( Qt::yellow, 2 ) );
    curve->setSymbol( new QwtSymbol( QwtSymbol::Rect,
        QBrush( Qt::cyan ), QPen( Qt::white ), QSize( 3, 3 ) ) );
    std::cout << "\n[Plot::createCurve()] calling setData()";
    curve->setData(
        new SpiralData( radialInterval, azimuthInterval, numPoints ) );
    std::cout << "\n[Plot::createCurve()] setData() done!";
    
    //curve->setLegendAttribute( QwtPolarCurve::LegendShowLine, true );
    //curve->setLegendAttribute( QwtPolarCurve::LegendShowSymbol, true );
    /*
    switch( id )
    {
        case PlotSettings::Spiral:
        {
            curve->setTitle( "Spiral" );
            curve->setPen( QPen( Qt::yellow, 2 ) );
            curve->setSymbol( new QwtSymbol( QwtSymbol::Rect,
                QBrush( Qt::cyan ), QPen( Qt::white ), QSize( 3, 3 ) ) );
            curve->setData(
                new SpiralData( radialInterval, azimuthInterval, numPoints ) );
            break;
        }
        case PlotSettings::Rose:
        {
            curve->setTitle( "Rose" );
            curve->setPen( QPen( Qt::red, 2 ) );
            curve->setSymbol( new QwtSymbol( QwtSymbol::Rect,
                QBrush( Qt::cyan ), QPen( Qt::white ), QSize( 3, 3 ) ) );
            curve->setData(
                new RoseData( radialInterval, azimuthInterval, numPoints ) );
            break;
        }
    }
    */
    std::cout << "\n[Plot::createCurve()] returning the curve.";
    return curve;
}

PlotSettings
Plot::settings()
const
{
    std::cout << "\nCalling Plot::settings().";
    PlotSettings s;
    for ( int scaleId = 0; scaleId < QwtPolar::ScaleCount; scaleId++ )
    {
        s.flags[PlotSettings::MajorGridBegin + scaleId] =
            d_grid->isGridVisible( scaleId );
        s.flags[PlotSettings::MinorGridBegin + scaleId] =
            d_grid->isMinorGridVisible( scaleId );
    }
    for ( int axisId = 0; axisId < QwtPolar::AxesCount; axisId++ )
    {
        s.flags[PlotSettings::AxisBegin + axisId] =
            d_grid->isAxisVisible( axisId );
    }

    s.flags[PlotSettings::AutoScaling] =
        d_grid->testGridAttribute( QwtPolarGrid::AutoScaling );

    s.flags[PlotSettings::Logarithmic] = 
        scaleEngine( QwtPolar::Radius )->transformation();

    const QwtScaleDiv *sd = scaleDiv( QwtPolar::Radius );
    s.flags[PlotSettings::Inverted] = sd->lowerBound() > sd->upperBound();

    s.flags[PlotSettings::Antialiasing] =
        d_grid->testRenderHint( QwtPolarItem::RenderAntialiased );

    /*for ( int curveId = 0; curveId < PlotSettings::NumCurves; curveId++ )
    {
        s.flags[PlotSettings::CurveBegin + curveId] =
            d_curve[curveId]->isVisible();
    }*/

    std::cout << "\nPlot::settings() exit";
    return s;
}

void
Plot::applySettings(const PlotSettings& s)
{
    std::cout << "\nCalling Plot::applySettings().";
    for ( int scaleId = 0; scaleId < QwtPolar::ScaleCount; scaleId++ )
    {
        d_grid->showGrid( scaleId,
            s.flags[PlotSettings::MajorGridBegin + scaleId] );
        d_grid->showMinorGrid( scaleId,
            s.flags[PlotSettings::MinorGridBegin + scaleId] );
    }

    for ( int axisId = 0; axisId < QwtPolar::AxesCount; axisId++ )
    {
        d_grid->showAxis( axisId,
            s.flags[PlotSettings::AxisBegin + axisId] );
    }

    d_grid->setGridAttribute( QwtPolarGrid::AutoScaling,
        s.flags[PlotSettings::AutoScaling] );

    const QwtInterval interval =
        scaleDiv( QwtPolar::Radius )->interval().normalized();
    if ( s.flags[PlotSettings::Inverted] )
    {
        setScale( QwtPolar::Radius,
            interval.maxValue(), interval.minValue() );
    }
    else
    {
        setScale( QwtPolar::Radius,
            interval.minValue(), interval.maxValue() );
    }

    if ( s.flags[PlotSettings::Logarithmic] )
    {
        setScaleEngine( QwtPolar::Radius, new QwtLogScaleEngine() );
    }
    else
    {
        setScaleEngine( QwtPolar::Radius, new QwtLinearScaleEngine() );
    }

    d_grid->setRenderHint( QwtPolarItem::RenderAntialiased,
        s.flags[PlotSettings::Antialiasing] );

    /*
    for ( int curveId = 0; curveId < PlotSettings::NumCurves; curveId++ )
    {
        d_curve[curveId]->setRenderHint( QwtPolarItem::RenderAntialiased,
                                         s.flags[PlotSettings::Antialiasing] );
        d_curve[curveId]->setVisible(
            s.flags[PlotSettings::CurveBegin + curveId] );
    }
    */
    std::cout << "\nPlot::applySettings() exit";
    replot();
}
