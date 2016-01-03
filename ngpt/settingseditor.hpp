#ifndef _SETTINGS_EDITOR_H_
#define _SETTINGS_EDITOR_H_

#include <qframe.h>
#include "plot.hpp"

class QCheckBox;

class SettingsEditor
: public QFrame
{
    Q_OBJECT

public:
    SettingsEditor( QWidget* parent = NULL );

    void showSettings( const PlotSettings & );
    PlotSettings settings() const;

Q_SIGNALS:
    void edited( const PlotSettings& );

private Q_SLOTS:
    void edited();

private:
    void updateEditor();
    QString label( int flag ) const;
    QCheckBox* d_checkBox[PlotSettings::NumFlags];
};

#endif
