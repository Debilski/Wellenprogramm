/*
 * lattice_controller.h
 *
 *  Created on: 13.03.2009
 *      Author: rikebs
 */

#ifndef LATTICE_CONTROLLER_H_
#define LATTICE_CONTROLLER_H_

#include <QtCore>
#include <memory>

#include "lattice_interface.h"
#include "lattice_scripter.h"
#include "plugin_kernel.h"
#include "configuration.h"


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

public:
    LatticeController(QObject* parent = 0);
    ~LatticeController();
    const LatticeInterface* lattice() const;
    LatticeInterface* lattice();

    int sizeX() { return lattice_->sizeX(); }
    int sizeY() { return lattice_->sizeY(); }
    int latticeSizeX() { return lattice_->latticeSizeX(); }
    int latticeSizeY() { return lattice_->latticeSizeY(); }

    bool isValid();
    bool load(const std::string& name, int sizeX, int sizeY, int latticeSizeX, int latticeSizeY);
    void destroy();

    std::list<std::string> getModelNames();

    LatticeScripter* const getLatticeScripter() const;
public slots:
    void stepNum(int n);
    void stepMany();
    void stepOnce();
    void stepUntil(double time);
    void startLoop();
    void stopLoop();
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
private:
    void loop();
    typedef std::auto_ptr<LatticeInterface> LatticePtr;
    std::auto_ptr<LatticeInterface> lattice_;
    PluginKernel TheKernel;
    LatticeScripter* latticeScripter_;
    int stepsAtOnce_;
};

#endif /* LATTICE_CONTROLLER_H_ */
