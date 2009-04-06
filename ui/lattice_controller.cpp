/*
 * lattice_controller.cpp
 *
 *  Created on: 13.03.2009
 *      Author: rikebs
 */

#include <memory>

#include "lattice_controller.h"

#include "lattice_interface.h"
#include "lattice_scripter.h"
#include "plugin_kernel.h"
#include "configuration.h"

class LatticeController::PrivateData {
public:
    PrivateData() :
        lattice( 0 ), stepsAtOnce( 1 )
    {
    }
    typedef std::auto_ptr< LatticeInterface > LatticePtr;
    std::auto_ptr< LatticeInterface > lattice;
    PluginKernel TheKernel;
    LatticeScripter* latticeScripter;
    int stepsAtOnce;
};

/**
 * Legt einen LatticeController zu einem gegebenen Elternobjekt an.
 *
 * Das Elternobjekt sorgt gegebenenfalls für das Zerstören des Controllers.
 */
LatticeController::LatticeController(QObject* parent) :
    QObject( parent )
{
    d_data = new PrivateData;

    // Load Libraries
    QStringList filters;

    filters << config( "libraryPattern" ).toString();

    std::cout << "Checking if " << qPrintable(config("libraryDirectory").toString()) << " exists… ";
    QDir libDir( config( "libraryDirectory" ).toString() );
    if ( !libDir.exists() ) {
        std::cout << "No!" << std::endl;
        exit( -1 );
    } else {
        std::cout << "Yes." << std::endl;
    }
    libDir.setNameFilters( filters );
    QFileInfoList libs = libDir.entryInfoList();

    foreach (const QFileInfo &libInfo, libs)
        {
            std::cout << "Found library: " << qPrintable(libInfo.filePath()) << std::endl;
            d_data->TheKernel.loadPlugin( libInfo.filePath().toStdString() );
        }

    d_data->latticeScripter = new LatticeScripter( this );
}

LatticeController::~LatticeController()
{
    delete d_data;
}

/**
 * Liefert einen Zeiger auf die aktuelle Lattice zurück.
 */
const LatticeInterface* LatticeController::lattice() const
{
    return d_data->lattice.get();
}

/**
 * Liefert einen Zeiger auf die aktuelle Lattice zurück.
 */
LatticeInterface* LatticeController::lattice()
{
    return d_data->lattice.get();
}

/**
 * Prüft, ob eine aktuelle Lattice initialisiert ist.
 */
bool LatticeController::isValid()
{
    return d_data->lattice.get() != 0;
}

/**
 * Lädt eine neue Lattice aus einem Plugin und initialisiert sie mit der angegebenen Größe.
 *
 */
bool LatticeController::load(const std::string& name, int sizeX, int sizeY, int latticeSizeX,
                             int latticeSizeY)
{

    qDebug() << "Trying Model " << name.c_str();

    int n = 0;
    bool modelExists = false;

    LatticeServer& LS = d_data->TheKernel.getLatticeServer();
    for (size_t i = 0; i < LS.getDriverCount(); ++i) {
        std::string s = LS.getDriver( i ).getName();
        std::cout << s.c_str() << std::endl;
        if ( LS.getDriver( i ).getName() == name ) {
            n = i;
            modelExists = true;
        }
    }

    d_data->lattice = d_data->TheKernel.getLatticeServer().getDriver( n ).createRenderer(
        sizeX, sizeY, latticeSizeX, latticeSizeY );
    return modelExists;
}

/**
 * Zerstört die Lattice.
 */
void LatticeController::destroy()
{
    d_data->lattice.reset( 0 );
}

/**
 * Führt genau einen Schritt aus
 */
void LatticeController::stepOnce()
{
    d_data->lattice.get()->step();
    emit changed();
}

/**
 * Führt so viele Schritte mit der aktuellen Schrittweite aus, bis
 * time überschritten wurde.
 */
void LatticeController::stepUntil(double time)
{
    emit changed();
}

/**
 * Führt so viele Schritte aus, bis das System anderweitig angehalten wird.
 */
void LatticeController::startLoop()
{

}

/**
 * Stoppt das System.
 */
void LatticeController::stopLoop()
{

}

// Kann und sollte vielleicht in den Kernel ausgelagert werden.
std::list< std::string > LatticeController::getModelNames()
{
    std::list< std::string > names;

    LatticeServer& LS = d_data->TheKernel.getLatticeServer();
    for (size_t DriverIndex = 0; DriverIndex < LS.getDriverCount(); ++DriverIndex) {
        names.push_back( LS.getDriver( DriverIndex ).getName() );
    }
    return names;
}

void LatticeController::stepMany()
{
    stepNum( d_data->stepsAtOnce );
    emit changed();
}

void LatticeController::stepNum(int n)
{
    d_data->lattice.get()->step( n );
    emit changed();
}

void LatticeController::loop()
{

}

LatticeScripter* LatticeController::getLatticeScripter() const
{
    return d_data->latticeScripter;
}

void LatticeController::setToFixpoint(uint component, const QPointF& realPoint, uint size)
{
    d_data->lattice.get()->setSpotAtComponent(
        realPoint.x(), realPoint.y(), size, 0, component, true );
    emit changed();
}

void LatticeController::setComponentAt(uint component, const QPointF& realPoint, uint size,
                                       double value)
{
    d_data->lattice.get()->setSpotAtComponent(
        realPoint.x(), realPoint.y(), size, value, component, true );
    emit changed();
}

int LatticeController::sizeX() const
{
    return d_data->lattice->sizeX();
}

int LatticeController::sizeY() const
{
    return d_data->lattice->sizeY();
}

QSize LatticeController::size() const
{
    return QSize( sizeX(), sizeY() );
}

int LatticeController::latticeSizeX() const
{
    return d_data->lattice->latticeSizeX();
}

int LatticeController::latticeSizeY() const
{
    return d_data->lattice->latticeSizeY();
}

QSize LatticeController::latticeSize() const
{
    return QSize( latticeSizeX(), latticeSizeY() );
}
