/*
 * lattice_controller.h
 *
 *  Created on: 13.03.2009
 *      Author: rikebs
 */

#ifndef LATTICE_CONTROLLER_H_
#define LATTICE_CONTROLLER_H_

#include <QtCore>

class LatticeInterface;
class LatticeScripter;

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

    int sizeX() const;
    int sizeY() const;
    QSize size() const;

    int latticeSizeX() const;
    int latticeSizeY() const;
    QSize latticeSize() const;

    bool isValid();
    bool load(const std::string& name, int sizeX, int sizeY, int latticeSizeX, int latticeSizeY);
    void destroy();

    std::list<std::string> getModelNames();

    LatticeScripter* getLatticeScripter() const;
public slots:
    void stepNum(int n);
    void stepMany();
    void stepOnce();
    void stepUntil(double time);
    void startLoop();
    void stopLoop();
    void setToFixpoint(uint component, const QPointF& realPoint, uint size);
    void setComponentAt(uint component, const QPointF& realPoint, uint size, double value);
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

    class PrivateData;
    PrivateData* d_data;
};



#endif /* LATTICE_CONTROLLER_H_ */
