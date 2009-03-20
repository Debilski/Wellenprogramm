/*
 * lattice_controller.cpp
 *
 *  Created on: 13.03.2009
 *      Author: rikebs
 */

#include "lattice_controller.h"

/**
 * Liefert einen Zeiger auf die aktuelle Lattice zurück.
 */
const LatticeInterface* LatticeController::lattice() const
{
    return lattice_.get();
}

/**
 * Liefert einen Zeiger auf die aktuelle Lattice zurück.
 */
LatticeInterface* LatticeController::lattice()
{
    return lattice_.get();
}

/**
 * Prüft, ob eine aktuelle Lattice initialisiert ist.
 */
bool LatticeController::isValid()
{
    return lattice_.get() != 0;
}

/**
 * Legt einen LatticeController zu einem gegebenen Elternobjekt an.
 *
 * Das Elternobjekt sorgt gegebenenfalls für das Zerstören des Controllers.
 */
LatticeController::LatticeController(QObject* parent)
: QObject(parent), lattice_( 0 ), stepsAtOnce_( 1 )
{
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

    foreach (const QFileInfo &libInfo, libs) {
            std::cout << "Found library: " << qPrintable(libInfo.filePath()) << std::endl;
            TheKernel.loadPlugin(libInfo.filePath().toStdString());
    }

    latticeScripter_ = new LatticeScripter( this );
}

LatticeController::~LatticeController()
{

}

/**
 * Lädt eine neue Lattice aus einem Plugin und initialisiert sie mit der angegebenen Größe.
 *
 */
bool LatticeController::load(const std::string& name, int sizeX, int sizeY, int latticeSizeX, int latticeSizeY)
{

    qDebug() << "Trying Model " << name.c_str();

    int n = 0;
    bool modelExists = false;

    LatticeServer& LS = TheKernel.getLatticeServer();
    for(size_t i=0; i<LS.getDriverCount(); ++i) {
        std::string s = LS.getDriver(i).getName();
        std::cout << s.c_str() << std::endl;
        if (LS.getDriver(i).getName() == name) {
            n = i;
            modelExists = true;
        }
    }

    lattice_ = TheKernel.getLatticeServer().getDriver(n).createRenderer( sizeX, sizeY, latticeSizeX, latticeSizeY );
    return modelExists;
}

/**
 * Zerstört die Lattice.
 */
void LatticeController::destroy()
{
    lattice_.reset(0);
}


/**
 * Führt genau einen Schritt aus
 */
void LatticeController::stepOnce()
{
    lattice_.get()->step();
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
std::list<std::string> LatticeController::getModelNames() {
    std::list<std::string> names;

    LatticeServer& LS = TheKernel.getLatticeServer();
    for (size_t DriverIndex = 0; DriverIndex < LS.getDriverCount(); ++DriverIndex) {
        names.push_back( LS.getDriver( DriverIndex ).getName() );
    }
    return names;
}

void LatticeController::stepMany() {
    stepNum( stepsAtOnce_ );
    emit changed();
}

void LatticeController::stepNum(int n) {
    lattice_.get()->step(n);
    emit changed();
}

void LatticeController::loop() {

}

LatticeScripter* const LatticeController::getLatticeScripter() const
{
    return latticeScripter_;
}

void LatticeController::setToFixpoint(uint component, const QPointF& realPoint)
{
    lattice_.get()->setSpotAtComponent(realPoint.x(), realPoint.y(), 1.0, 0, component, true);
    emit changed();
}
