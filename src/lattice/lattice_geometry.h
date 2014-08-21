/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/

/**
 * Wellenprogramm
 *
 * Rike-Benjamin Schuppner
 */

#ifndef LATTICE_GEOMETRY_H
#define LATTICE_GEOMETRY_H

#include <cmath>
#include <vector>
#include <string>
#include <sstream>
#include <cstdio>

/**
 * Symbolises a point on the internal lattice.
 */
struct LatticePoint {
    int x;
    int y;
    LatticePoint() :
        x( 0 ), y( 0 )
    {
    }
    LatticePoint(int x, int y) :
        x( x ), y( y )
    {
    }

};

/**
 * Symbolises a point on the ‘real’ surface of the system.
 */
struct SurfacePoint {
    double x;
    double y;

    //SurfacePoint() : x(0), y(0) {}
    //SurfacePoint( double x, double y ) : x(x), y(y) {}
    bool operator==(const SurfacePoint& p)
    {
        return (this->x == p.x && this->y == p.y);
    }

};

/**
 * Basic class that stores the dimensions of the system.
 *
 * Add: More Iterators
 */
class LatticeGeometry {
public:
    LatticeGeometry();
    LatticeGeometry(int sizeX, int sizeY, int latticeSizeX, int latticeSizeY);
    LatticeGeometry(const LatticeGeometry& geometry);
    long int indexPeriodic(int i, int j) const;
    long int index(int i, int j) const;
    int indexToX(int idx) const;
    int indexToY(int idx) const;
    double euklidianDistance(double ax, double ay, double bx, double by) const;
    double euklidianDistance(const SurfacePoint& a, const SurfacePoint& b) const;

    double latticeDistance(const LatticePoint& a, const LatticePoint& b) const;

    int sizeX() const;
    int sizeY() const;

    double scaleX() const;
    double scaleY() const;

    int boxesPerLengthX() const;
    int boxesPerLengthY() const;

    int latticeSize() const;
    int latticeSizeX() const;
    int latticeSizeY() const;

    LatticeGeometry geometry() const;

    SurfacePoint indexToSurfacePoint(int index) const;
    LatticePoint indexToLatticePoint(int index) const;

    int index(const LatticePoint& l) const;
    int index(const SurfacePoint& l) const;

    SurfacePoint latticeToSurfacePoint(const LatticePoint& l) const;
    LatticePoint surfaceToLatticePoint(const SurfacePoint& s) const;

    SurfacePoint centrePoint() const;

    bool hasValidGeometry() const;

    void initGeometry(int sizeX, int sizeY, int latticeSizeX, int latticeSizeY);
    void setGeometry(int sizeX, int sizeY, int latticeSizeX, int latticeSizeY);

    static LatticeGeometry sizeFromString(std::string size);
    static std::string stringFromSize(LatticeGeometry size);
    static std::string stringFromSize(int sizeX, int sizeY, int latticeSizeX, int latticeSizeY);

    bool operator==(const LatticeGeometry& other);
    bool operator!=(const LatticeGeometry& other);
private:
    int sizeX_, sizeY_;
    int latticeSizeX_, latticeSizeY_;
    long latticeSize_;
    double scaleX_, scaleY_;
    int boxesPerLengthX_, boxesPerLengthY_;

};

inline void LatticeGeometry::initGeometry(int sizeX, int sizeY, int latticeSizeX, int latticeSizeY)
{
    sizeX_ = sizeX;
    sizeY_ = sizeY;
    latticeSizeX_ = latticeSizeX;
    latticeSizeY_ = latticeSizeY;

    latticeSize_ = (latticeSizeX * latticeSizeY);
    scaleX_ = (static_cast< double > ( sizeX ) / static_cast< double > ( latticeSizeX ));
    scaleY_ = (static_cast< double > ( sizeY ) / static_cast< double > ( latticeSizeY ));
    boxesPerLengthX_ = (latticeSizeX / sizeX);
    boxesPerLengthY_ = (latticeSizeY / sizeY);
}

inline void LatticeGeometry::setGeometry(int sizeX, int sizeY, int latticeSizeX, int latticeSizeY)
{
    initGeometry(
        sizeX ? sizeX : sizeX_, sizeY ? sizeY : sizeY_,
        latticeSizeX ? latticeSizeX : latticeSizeX_, latticeSizeY ? latticeSizeY : latticeSizeY_ );
}

inline bool LatticeGeometry::hasValidGeometry() const
{
    return !(sizeX_ * sizeY_ * latticeSizeX_ * latticeSizeY_);
}

inline int LatticeGeometry::sizeX() const
{
    return sizeX_;
}

inline int LatticeGeometry::sizeY() const
{
    return sizeY_;
}

inline double LatticeGeometry::scaleX() const
{
    return scaleX_;
}

inline double LatticeGeometry::scaleY() const
{
    return scaleY_;
}

inline int LatticeGeometry::boxesPerLengthX() const
{
    return boxesPerLengthX_;
}

inline int LatticeGeometry::boxesPerLengthY() const
{
    return boxesPerLengthY_;
}

inline int LatticeGeometry::latticeSize() const
{
    return latticeSize_;
}

inline int LatticeGeometry::latticeSizeX() const
{
    return latticeSizeX_;
}

inline int LatticeGeometry::latticeSizeY() const
{
    return latticeSizeY_;
}

inline SurfacePoint LatticeGeometry::indexToSurfacePoint(int index) const
{
    return latticeToSurfacePoint( indexToLatticePoint( index ) );
}
inline LatticePoint LatticeGeometry::indexToLatticePoint(int index) const
{
    return LatticePoint( indexToX( index ), indexToY( index ) );
}

inline int LatticeGeometry::index(const LatticePoint& l) const
{
    return index( l.x, l.y );
}
inline int LatticeGeometry::index(const SurfacePoint& l) const
{
    return index( surfaceToLatticePoint( l ) );
}

inline SurfacePoint LatticeGeometry::latticeToSurfacePoint(const LatticePoint& l) const
{
    SurfacePoint s;
    s.x = l.x * scaleX_;
    s.y = l.y * scaleY_;
    return s;
}

inline LatticePoint LatticeGeometry::surfaceToLatticePoint(const SurfacePoint& s) const
{
    return LatticePoint(
        static_cast< int > ( floor( s.x / scaleX_ + 0.5 ) ), static_cast< int > ( floor( s.y
            / scaleY_ + 0.5 ) ) );
}

inline SurfacePoint LatticeGeometry::centrePoint() const
{
    SurfacePoint sp = { sizeX() / 2.0, sizeY() / 2.0 };
    return sp;
}

/**
 * Returns the geometry of a Lattice.
 */
inline LatticeGeometry LatticeGeometry::geometry() const
{
    LatticeGeometry g( *this );
    return g;
}

inline LatticeGeometry::LatticeGeometry() :
    sizeX_( 0 ), sizeY_( 0 ), latticeSizeX_( 0 ), latticeSizeY_( 0 ), latticeSize_( 0 ),
        scaleX_( 0 ), scaleY_( 0 ), boxesPerLengthX_( 0 ), boxesPerLengthY_( 0 )
{
}

/**
 * Constructs a geometry from given sizes.
 */
inline LatticeGeometry::LatticeGeometry(int sizeX, int sizeY, int latticeSizeX, int latticeSizeY) :
    sizeX_( sizeX ), sizeY_( sizeY ), latticeSizeX_( latticeSizeX ), latticeSizeY_( latticeSizeY ),
        latticeSize_( latticeSizeX * latticeSizeY ), scaleX_( static_cast< double > ( sizeX )
            / static_cast< double > ( latticeSizeX ) ), scaleY_( static_cast< double > ( sizeY )
            / static_cast< double > ( latticeSizeY ) ), boxesPerLengthX_( latticeSizeX / sizeX ),
        boxesPerLengthY_( latticeSizeY / sizeY )
{
}

/**
 * Copys a geometry from another system
 */
inline LatticeGeometry::LatticeGeometry(const LatticeGeometry& geometry) :
    sizeX_( geometry.sizeX_ ), sizeY_( geometry.sizeY_ ), latticeSizeX_( geometry.latticeSizeX_ ),
        latticeSizeY_( geometry.latticeSizeY_ ), latticeSize_( geometry.latticeSizeX_
            * geometry.latticeSizeY_ ), scaleX_( static_cast< double > ( geometry.sizeX_ )
            / static_cast< double > ( geometry.latticeSizeX_ ) ), scaleY_(
            static_cast< double > ( geometry.sizeY_ )
                / static_cast< double > ( geometry.latticeSizeY_ ) ), boxesPerLengthX_(
            geometry.latticeSizeX_ / geometry.sizeX_ ), boxesPerLengthY_( geometry.latticeSizeY_
            / geometry.sizeY_ )
{
}

inline long int LatticeGeometry::index(int i, int j) const
{

    int l = j + i * (latticeSizeY_); // i: y direction, j: x direction
    return l;
}

inline long int LatticeGeometry::indexPeriodic(int i, int j) const
/* Index mit periodischen Randbedg. */
{
    long int k = index( i, j );
    return (k >= latticeSize_ || k < 0) ? (((k % latticeSize_) + latticeSize_) % latticeSize_) : k;
}

inline int LatticeGeometry::indexToX(int idx) const
{
    return idx / (latticeSizeY_);
}

inline int LatticeGeometry::indexToY(int idx) const
{
    return idx % (latticeSizeY_);
}

/**
 * Returns the distance between two points on the surface with coordinates (@a ax, @a ay) and (@a bx, @a by), respectively.
 *
 * Assumes periodic boundary conditions.
 */
inline double LatticeGeometry::euklidianDistance(double ax, double ay, double bx, double by) const
{
    double xdist = (ax - bx < sizeX_ / 2) ? (ax - bx) : (sizeX_ - (ax - bx));
    double ydist = (ay - by < sizeY_ / 2) ? (ay - by) : (sizeY_ - (ay - by));
    double dist = sqrt( (xdist) * (xdist) + (ydist) * (ydist) );
    return dist;
}

/**
 * Returns the distance between two SurfacePoints @a a and @a b.
 *
 * Assumes periodic boundary conditions.
 */
inline double LatticeGeometry::euklidianDistance(const SurfacePoint& a, const SurfacePoint& b) const
{
    return euklidianDistance( a.x, a.y, b.x, b.y );
}

inline double LatticeGeometry::latticeDistance(const LatticePoint& a, const LatticePoint& b) const
{
    return sqrt( (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) );
}

inline LatticeGeometry LatticeGeometry::sizeFromString(std::string size)
{
    int sizeX, sizeY, latticeSizeX, latticeSizeY;
    if (sscanf( size.c_str(), "%dx%d / %dx%d", &sizeX, &sizeY, &latticeSizeX, &latticeSizeY ) == 4)
        return LatticeGeometry( sizeX, sizeY, latticeSizeX, latticeSizeY );
    return LatticeGeometry();
}

inline std::string LatticeGeometry::stringFromSize(LatticeGeometry size)
{
    return stringFromSize( size.sizeX(), size.sizeY(), size.latticeSizeX(), size.latticeSizeY() );
}

inline std::string LatticeGeometry::stringFromSize(int sizeX, int sizeY, int latticeSizeX,
                                                   int latticeSizeY)
{
    std::stringstream s;
    s << sizeX << "x" << sizeY << " / " << latticeSizeX << "x" << latticeSizeY;
    std::string res = s.str();
    return res;
}

inline bool LatticeGeometry::operator==(const LatticeGeometry& other)
{
    return sizeX_ == other.sizeX_ && sizeY_ == other.sizeY_ && latticeSizeX_ == other.latticeSizeX_
        && latticeSizeY_ == other.latticeSizeY_;
}

inline bool LatticeGeometry::operator!=(const LatticeGeometry& other)
{
    return !(operator==( other ));
}

class LatticeIterator {
    const LatticeGeometry* latticeGeometry_;
    int currentIndex_;
    std::vector< LatticePoint > polygon_;
    typedef std::vector< LatticePoint >::const_iterator T_iter;
public:
    LatticeIterator(const LatticeGeometry* latticeGeometry, std::vector< SurfacePoint > polygon) :
        latticeGeometry_( latticeGeometry ), currentIndex_( 0 )
    {
        for (std::vector< SurfacePoint >::const_iterator it = polygon.begin(); it != polygon.end(); ++it)
        {
            LatticePoint p;
            p = latticeGeometry_->surfaceToLatticePoint( *it );
            polygon_.push_back( p );
        }
    }
    int index()
    {
        return currentIndex_;
    }
    void operator++()
    {
        do {
            currentIndex_++;

        } while (!isInsidePolygon( latticeGeometry_->indexToLatticePoint( currentIndex_ ) )
            && !isDone());
    }
    bool isDone()
    {
        return currentIndex_ >= latticeGeometry_->latticeSize();
    }
private:
    bool isInsidePolygon(LatticePoint p)
    {
        unsigned int xnew, ynew;
        unsigned int xold, yold;
        unsigned int x1, y1;
        unsigned int x2, y2;
        bool inside = false;

        unsigned int npoints = polygon_.size();
        if ( npoints < 3 ) {
            return false;
        }
        xold = polygon_[ npoints - 1 ].x;
        yold = polygon_[ npoints - 1 ].y;
        for (unsigned int i = 0; i < npoints; i++) {
            xnew = polygon_[ i ].x;
            ynew = polygon_[ i ].y;
            if ( xnew > xold ) {
                x1 = xold;
                x2 = xnew;
                y1 = yold;
                y2 = ynew;
            } else {
                x1 = xnew;
                x2 = xold;
                y1 = ynew;
                y2 = yold;
            }
            if ( (static_cast< long > ( xnew ) < static_cast< long > ( p.x ))
                == (static_cast< long > ( p.x ) <= static_cast< long > ( xold )) /* edge "open" at one end */
            && (static_cast< long > ( p.y ) - static_cast< long > ( y1 ))
                * static_cast< long > ( x2 - x1 ) < (static_cast< long > ( y2 )
                - static_cast< long > ( y1 )) * static_cast< long > ( p.x - x1 ) )
            {
                inside = !inside;
            }
            xold = xnew;
            yold = ynew;
        }
        return (inside);

    }
};
#endif

