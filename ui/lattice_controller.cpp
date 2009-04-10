/*
 * lattice_controller.cpp
 *
 *  Created on: 13.03.2009
 *      Author: rikebs
 */


#include "lattice_controller.h"

#include <memory>
#include <QtCore>

#include "lattice_interface.h"
#include "lattice_scripter.h"
#include "plugin_kernel.h"
#include "configuration.h"


LoopThread::LoopThread(QObject *parent) :
    lattice( 0 ), QThread( parent )
{
    abort = false;
    restart = false;
    stopped = true;
}
LoopThread::~LoopThread()
{
    mutex.lock();
    abort = true;
    stopped = true;
    condition.wakeOne();
    mutex.unlock();

    wait();
}

void LoopThread::setLattice(LatticeInterface* lattice)
{
    QMutexLocker locker( &mutex );
    this->lattice = lattice;
}

void LoopThread::loop(int stepsAtOnce)
{
    QMutexLocker locker( &mutex );
    this->stepsAtOnce = stepsAtOnce;

    stopped = false;
    if ( !isRunning() ) {
        start( LowPriority );
    } else {
        restart = true;
        condition.wakeOne();
    }
}
void LoopThread::stop()
{
    QMutexLocker locker( &mutex );
    stopped = true;

}

void LoopThread::run()
{
    forever {
        mutex.lock();
        int stepsAtOnce = this->stepsAtOnce;
        mutex.unlock();
        while (true) {
            if ( restart )
                break;
            if ( stopped )
                break;
            lattice->step( stepsAtOnce );
        }
        mutex.lock();
        if ( !restart )
            condition.wait( &mutex );
        restart = false;
        mutex.unlock();
    }
}

class LatticeController::PrivateData {
public:
    PrivateData() :
        lattice( 0 ), stepsAtOnce( 1 ), stopped( true ), adaptationMode( false )
    {
    }
    typedef std::auto_ptr< LatticeInterface > LatticePtr;
    std::auto_ptr< LatticeInterface > lattice;
    PluginKernel TheKernel;
    LatticeScripter* latticeScripter;
    int stepsAtOnce;
    bool adaptationMode;
    bool stopped;
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
bool LatticeController::load(const QString& name, int sizeX, int sizeY, int latticeSizeX,
                             int latticeSizeY)
{
    closeLattice();
    qDebug() << "Trying Model " << name;
    std::string std_name = name.toStdString();
    int n = 0;
    bool modelExists = false;

    LatticeServer& LS = d_data->TheKernel.getLatticeServer();
    for (size_t i = 0; i < LS.getDriverCount(); ++i) {
        std::string s = LS.getDriver( i ).getName();
        std::cout << s.c_str() << std::endl;
        if ( LS.getDriver( i ).getName() == std_name ) {
            n = i;
            modelExists = true;
        }
    }

    d_data->lattice = d_data->TheKernel.getLatticeServer().getDriver( n ).createRenderer(
        sizeX, sizeY, latticeSizeX, latticeSizeY );
    thread.setLattice( lattice() );
    return modelExists;
}

/**
 * Zerstört die Lattice.
 */
void LatticeController::closeLattice()
{
    stopLoop();
    // Ist problematisch, da nicht das delete aus der Klasse verwendet wird. Muss noch irgendwann ins Interface.
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
    if ( ! loopRuns() ) {
        d_data->stopped = false;
        QTimer::singleShot( 0, this, SLOT(loop()) );
    }
}

/**
 * Stoppt das System.
 */
void LatticeController::stopLoop()
{
    if ( loopRuns() ) {
        d_data->stopped = true;
        emit changed();
    }
}

// Kann und sollte vielleicht in den Kernel ausgelagert werden.
QStringList LatticeController::getModelNames()
{
    LatticeServer& LS = d_data->TheKernel.getLatticeServer();
    std::list< std::string > names = LS.getModelNames();
    QStringList n;
    for(std::list< std::string >::const_iterator it = names.begin(); it != names.end(); ++it ) {
        n << QString::fromStdString( *it );
    }
    return n;

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

void LatticeController::loop()
{
    QTime t;
    t.start();
    while (t.elapsed() < 100) {
        if ( d_data->stopped ) {
            return;
        }
        if ( adaptationMode() ) {
                        lattice()->adaptParameters();
                        emit parametersChanged();
        }
        d_data->lattice->step( d_data->stepsAtOnce );
        emit processed( d_data->stepsAtOnce );

    }
    if ( d_data->stopped ) {
        emit stopped();
        return;
    }
    QTimer::singleShot( 0, this, SLOT(loop()) );
}
void LatticeController::start(int i)
{
    //thread.loop(i);
}

void LatticeController::stop()
{
    //thread.stop();
}

bool LatticeController::loopRuns() const
{
    return ! (d_data->stopped);
}

void LatticeController::setAdaptationMode(bool b)
{
    d_data->adaptationMode = b;
}

bool LatticeController::adaptationMode()
{
    return d_data->adaptationMode;
}
