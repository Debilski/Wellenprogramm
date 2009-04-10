/*
 * lattice_controller.h
 *
 *  Created on: 13.03.2009
 *      Author: rikebs
 */

#ifndef LATTICE_CONTROLLER_H_
#define LATTICE_CONTROLLER_H_

#include <qobject.h>
#include <qthread.h>

#include <qmutex.h>
#include <qwaitcondition.h>

class LatticeInterface;
class LatticeScripter;

class LoopThread : public QThread {
Q_OBJECT
public:
    LoopThread(QObject *parent = 0);
    ~LoopThread();
    void setLattice(LatticeInterface* lattice);
    void loop(int stepsAtOnce);
    void stop();
protected:
    void run();
private:

    QMutex mutex;
    QWaitCondition condition;
    LatticeInterface* lattice;
    bool abort;
    bool restart;
    bool stopped;
    int stepsAtOnce;
};

#include <qsize.h>
class QPointF;

/**
 * Introspektionsfähige Abstraktion des Lattice-Objekts.
 *
 * Über dieses Objekt soll über Slots und Signale die Steuerung
 * der Ui verbessert werden und außerdem das jetzige Haupt-Objekt
 * entlastet werden.
 *
 * Einige der hier verfügbar gemachten Funktionen sind direkt aus
 * LatticeInterface übernommen und bilden dies nur ab.
 * Andere Funktionen haben darüber hinaus gehende Fähigkeiten, die
 * später nach Lattice<> exportiert werden könnten.
 *
 *
 */
class LatticeController : public QObject {
Q_OBJECT
Q_PROPERTY(int sizeX READ sizeX)
Q_PROPERTY(int sizeY READ sizeY)
Q_PROPERTY(int latticeSizeX READ latticeSizeX)
Q_PROPERTY(int latticeSizeY READ latticeSizeY)
Q_PROPERTY(int adaptationMode READ adaptationMode WRITE setAdaptationMode)

public:
    LatticeController(QObject* parent = 0);
    ~LatticeController();
    const LatticeInterface* lattice() const;
    LatticeInterface* lattice();

    int sizeX() const;
    int sizeY() const;
    QSize size() const;

    int latticeSizeX() const;
    int latticeSizeY() const;
    QSize latticeSize() const;

    bool isValid();
    bool load(const QString& name, int sizeX, int sizeY, int latticeSizeX, int latticeSizeY);
    void closeLattice();

    QStringList getModelNames();

    LatticeScripter* getLatticeScripter() const;

    bool loopRuns() const;
public slots:
    void start(int i);
    void stop();

    void stepNum(int n);
    void stepMany();
    void stepOnce();
    void stepUntil(double time);
    void startLoop();
    void stopLoop();
    void setToFixpoint(uint component, const QPointF& realPoint, uint size);
    void setComponentAt(uint component, const QPointF& realPoint, uint size, double value);

    bool adaptationMode();
    void setAdaptationMode(bool b);
private slots:
    void loop();
signals:
    /**
     * Wird aufgerufen, wenn das Modell sich verändert hat.
     */
    void modelChanged();
    /**
     * Wird aufgerufen, wenn das System sich verändert hat.
     * Ist möglich, dies noch in mehrere Signale aufzuteilen.
     */
    void changed();
    void stopped();
    void processed(int);
    void parametersChanged();
private:
    LoopThread thread;

    class PrivateData;
    PrivateData* d_data;
};

#endif /* LATTICE_CONTROLLER_H_ */
