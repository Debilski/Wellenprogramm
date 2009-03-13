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

LatticeInterface* LatticeController::lattice()
{
    return lattice_.get();
}

bool LatticeController::isValid()
{
    return lattice_.get() != 0;
}

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
}

LatticeController::~LatticeController()
{

}

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
}

/**
 * Führt so viele Schritte mit der aktuellen Schrittweite aus, bis
 * time überschritten wurde.
 */
void LatticeController::stepUntil(double time)
{

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
}

void LatticeController::stepNum(int n) {
    lattice_.get()->step(n);
}

void LatticeController::loop() {

}
