/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/

#include "cluster_counter.h"

#include <blitz/array.h>
#include <iostream>
#include <algorithm>
#include <vector>

// --- Cluster Stuff ---


ClusterCounter::ClusterCounter(const LatticeInterface* lattice) : lattice_( lattice ), nextClusterId_( 0 ), numClusters_( 0 ), distanceThreshold_(
        20 ), sizeThreshold_( 4.0 ), clusterThreshold_( 0 )
{
}

ClusterCounter::~ClusterCounter()
{
}

void ClusterCounter::init(int sizeX, int sizeY, int latticeSizeX, int latticeSizeY)
{
    setGeometry( sizeX, sizeY, latticeSizeX, latticeSizeY);
    clusterField.resize( latticeSizeX, latticeSizeY );
}

void ClusterCounter::setClusterThreshold(double threshold)
{
    clusterThreshold_ = threshold;
}

double ClusterCounter::clusterThreshold()
{
    return clusterThreshold_;
}

int ClusterCounter::numberOfClusters()
{
    return numClusters_;
}

std::vector< Cluster > ClusterCounter::getClusters()
{
    return clusterVector;
}

void ClusterCounter::printClusters()
{
    for (uint i = 0; i < clusterVector.size(); ++i) {
        std::cout << clusterVector[ i ].size << " ";
    }
}

void ClusterCounter::countClusters()
{
    // Ordne die Cluster zu und generiere neuen Vektor aus Clustergrößen
    int tmp_numClusters = 0;
    clusterSizes = std::vector< double >();
    clusterSizes.reserve( 10 );

    int clusterIgnoreCounter = 0;
    double clusterIgnoreSize = 0;

    for (int i = 0; i < latticeSize(); ++i) {
        if ( clusterField.data()[ i ] == -1 ) {
            clusterQueue = std::queue< long int >();
            clusterQueue.push( i );
            double cluster_size = 0;
            while (!clusterQueue.empty()) {
                long pos = clusterQueue.front();
                clusterQueue.pop();
                cluster_size += clusterWalker( pos, tmp_numClusters + 1 );
            }
            if ( cluster_size >= 1 ) {
                if ( cluster_size >= sizeThreshold_ ) {
                    tmp_numClusters++;
                    clusterSizes.push_back( cluster_size * scaleX() * scaleY() );
                } else {
                    tmp_numClusters++;
                    clusterSizes.push_back( -1 );
                    ++clusterIgnoreCounter;
                    clusterIgnoreSize += cluster_size * scaleX() * scaleY();
                }
                //        cout << cluster_size << ", ";
            }
        }
    }
#ifdef DEBUG
    if ( clusterIgnoreCounter != 0 )
        std::cout << "# " << clusterIgnoreCounter << " cluster(s) with a total size of "
            << clusterIgnoreSize << " were each smaller than " << sizeThreshold_
            << " lattice points and are going to be ignored.\n";
#endif
    // Ordne den Centroid jedes Clusters zu
    // Für den Rest cv. Czink et al
    //    int* marginalised_mass_x = new int[ latticeSizeX() ];
    //    int* marginalised_mass_y = new int[ latticeSizeY() ];

    blitz::Array< int, 1 > marginalised_mass_x( latticeSizeX() );
    blitz::Array< int, 1 > marginalised_mass_y( latticeSizeX() );

    clusterPositions = std::vector< SurfacePoint >();
    clusterPositions.reserve( tmp_numClusters );
    for (int k = 0; k < tmp_numClusters; ++k) {
        double mass = 0;
        double mx = 0;
        double my = 0;

        //fill( marginalised_mass_x, marginalised_mass_x + latticeSizeX(), 0 );
        //fill( marginalised_mass_y, marginalised_mass_y + latticeSizeY(), 0 );
        marginalised_mass_x = 0;
        marginalised_mass_y = 0;

        //blitz::firstIndex i;
        //blitz::secondIndex j;
        //mass = blitz::count( clusterField(i,j) == k+1 );
        //marginalised_mass_x = blitz::count( clusterField(i,j) == k+1, j );
        //marginalised_mass_y = blitz::count( clusterField(j,i) == k+1, j );
        for (int i = 0; i < latticeSizeX(); ++i) {
            for (int j = 0; j < latticeSizeY(); ++j) {
                if ( clusterField( i, j ) == k + 1 ) {
                    mass++;
                    marginalised_mass_x( i )++;
                    marginalised_mass_y( j )++;
                }
            }
        }

        if ( marginalised_mass_x( 0 ) != 0 && marginalised_mass_x( latticeSizeX() - 1 ) != 0 ) {
            bool reverse = false;
            for (int i = 0; i < latticeSizeX(); ++i) {
                if ( marginalised_mass_x( i ) == 0 )
                    reverse = true;
                if ( reverse == true ) {
                    mx += (i - latticeSizeX()) * marginalised_mass_x( i );
                } else {
                    mx += (i) * marginalised_mass_x( i );
                }
            }
        } else {
            for (int i = 0; i < latticeSizeX(); ++i) {
                mx += i * marginalised_mass_x( i );
            }
        }
        mx = mx * scaleX() / mass;
        if ( mx < 0 )
            mx += sizeX();

        if ( marginalised_mass_y( 0 ) != 0 && marginalised_mass_y( latticeSizeY() - 1 ) != 0 ) {
            bool reverse = false;
            for (int i = 0; i < latticeSizeY(); ++i) {
                if ( marginalised_mass_y( i ) == 0 )
                    reverse = true;
                if ( reverse == true ) {
                    my += (i - latticeSizeY()) * marginalised_mass_y( i );
                } else {
                    my += (i) * marginalised_mass_y( i );
                }
            }
        } else {
            for (int i = 0; i < latticeSizeY(); ++i) {
                my += i * marginalised_mass_y( i );
            }
        }
        my = my * scaleY() / mass;
        if ( my < 0 )
            my += sizeY();

        SurfacePoint s = { mx, my };
        //std::cout << "coE (" << mx/mass << "; " << my/mass << ") " << std::endl <<std::flush;
        clusterPositions.push_back( s );
    }

    //    delete[] marginalised_mass_x;
    //    delete[] marginalised_mass_y;

    previousClusterVector = clusterVector;

    // Lösche Cluster, die zu klein sind:
    SurfacePoint nullPosition = { -1, -1 };
    for (uint i = 0; i < clusterSizes.size(); ++i) {
        if ( clusterSizes[ i ] == -1 ) {
            //std::cout << "deleted";
            clusterPositions[ i ] = nullPosition;
        }
    }

    clusterSizes.erase(
        std::remove( clusterSizes.begin(), clusterSizes.end(), -1 ), clusterSizes.end() );
    clusterPositions.erase( std::remove(
        clusterPositions.begin(), clusterPositions.end(), nullPosition ), clusterPositions.end() );

    // Fülle DistanceMatrix
    int prevSize = previousClusterVector.size();
    int newSize = clusterSizes.size();
    clusterVector.clear();
    clusterVector.reserve( newSize );
    blitz::Array< double, 2 > distanceMatrix( prevSize, newSize );

    for (int i = 0; i < prevSize; ++i) {
        for (int j = 0; j < newSize; ++j) {
            distanceMatrix( i, j ) = euklidianDistance(
                previousClusterVector[ i ].position.x, previousClusterVector[ i ].position.y,
                clusterPositions[ j ].x, clusterPositions[ j ].y );
        }
    }

    //  std::cout << distanceMatrix;
    // Finde nächsten alten Cluster
    for (int j = 0; j < newSize; ++j) {

        double smallestDist = -1;
        int pos = -1;
        for (int i = 0; i < prevSize; ++i) {
            if ( distanceMatrix( i, j ) < smallestDist || smallestDist < 0 ) {
                pos = i;
                smallestDist = distanceMatrix( i, j );
            }
        }

        //blitz::Array<int, 1> minI(newSize);
        //minI = blitz::minIndex( distanceMatrix(blitz::tensor::j, blitz::tensor::i), blitz::tensor::j );

        // if (pos == minI(j)) cout << "OK"; else { cout << endl << "Not OK " << pos << " " << minI(j) << endl; }

        // Wenn es keinen gibt: Neue Id.
        Cluster c;
        c.clusterId = (pos < 0 || smallestDist > distanceThreshold_) ? makeNewClusterId() : -1;
        c.position = clusterPositions[ j ];
        c.size = clusterSizes[ j ];
        clusterVector.push_back( c );
    }

    for (int i = 0; i < prevSize; ++i) {
        int numElem_smaller_thresh = 0;
        for (int j = 0; j < newSize; ++j) {
            if ( distanceMatrix( i, j ) < distanceThreshold_ )
                numElem_smaller_thresh++;
        }
        if ( numElem_smaller_thresh == 1 ) {
            for (int j = 0; j < newSize; ++j) {
                if ( distanceMatrix( i, j ) < distanceThreshold_ ) {
                    // Prüfe vorher, ob es nicht doch einen näheren alten Cluster gab, der schon zugewiesen wurde
                    if ( clusterVector[ j ].clusterId != -1 ) {
                        long int tempId = clusterVector[ j ].clusterId;
                        for (int ii = 0; ii < i; ++ii) {
                            if ( previousClusterVector[ ii ].clusterId == tempId ) {
                                if ( distanceMatrix( i, j ) < distanceMatrix( ii, j ) ) {
                                    clusterVector[ j ].clusterId
                                        = previousClusterVector[ i ].clusterId;
                                }
                            }
                        }
                    } else {
                        clusterVector[ j ].clusterId = previousClusterVector[ i ].clusterId;
                    }
                }
            }
        }
        if ( numElem_smaller_thresh > 1 ) {
            double smallestDist = -1;
            int pos = -1;
            for (int j = 0; j < newSize; ++j) {
                if ( distanceMatrix( i, j ) < smallestDist || smallestDist < 0 ) {
                    pos = j;
                    smallestDist = distanceMatrix( i, j );
                }
            }
            if ( pos >= 0 ) {
                // Prüfe vorher, ob es nicht doch einen näheren alten Cluster gab, der schon zugewiesen wurde
                if ( clusterVector[ pos ].clusterId != -1 ) {
                    long int tempId = clusterVector[ pos ].clusterId;
                    for (int ii = 0; ii < i; ++ii) {
                        if ( previousClusterVector[ ii ].clusterId == tempId ) {
                            if ( distanceMatrix( i, pos ) < distanceMatrix( ii, pos ) ) {
                                clusterVector[ pos ].clusterId
                                    = previousClusterVector[ i ].clusterId;
                            }
                        }
                    }
                } else {
                    clusterVector[ pos ].clusterId = previousClusterVector[ i ].clusterId;
                }
            }
        }
    }
    // Jetzt alle zuweisen, die noch keine Id haben.
    for (int j = 0; j < newSize; ++j) {
        if ( clusterVector[ j ].clusterId == -1 )
            clusterVector[ j ].clusterId = makeNewClusterId();
        // Noch falsch?
    }

    /*
     clusterIdSizes.clear();
     clusterIdPositions.clear();
     for(int i=0; i<clusterVector.size(); ++i) {
     long int clusterId = clusterVector[i].clusterId;
     clusterIdSizes[ clusterId ] = clusterVector[i].size ;

     clusterIdPositions[ clusterId ] = clusterVector[i].position ;
     }

     for(clusterIdSizesIter = clusterIdSizes.begin(); clusterIdSizesIter != clusterIdSizes.end(); ++clusterIdSizesIter) {
     SurfacePoint pos = clusterIdPositions[clusterIdSizesIter->first];
     cout << clusterIdSizesIter->first << ":" << clusterIdSizesIter->second << " (" << pos.x << ";" << pos.y << ") ";
     }
     //copy(clusterSizes.begin(), clusterSizes.end(), ostream_iterator<double>(cout, " "));
     cout << endl << flush;
     */

    numClusters_ = clusterVector.size();
}

/**
 * Setzt ClusterField an \a position auf \a setTo, sowie die Nachbarpunkte, falls das Feld
 * an diesen Punkten noch keinen zugeordneten Cluster hat.
 */
int ClusterCounter::clusterWalker(long int position, int setTo)
{
    int sum = 0;
    if ( clusterField.data()[ position ] == -1 ) {
        ++sum;
        //cout << position << " " << flush;
        clusterField.data()[ position ] = setTo;
    }
    int xpos = indexToX( position );
    int ypos = indexToY( position );

    if ( clusterField.data()[ indexPeriodic( xpos + 1, ypos ) ] == -1 ) {
        clusterQueue.push( indexPeriodic( xpos + 1, ypos ) );
        clusterField.data()[ indexPeriodic( xpos + 1, ypos ) ] = setTo;
        ++sum;
    }
    if ( clusterField.data()[ indexPeriodic( xpos - 1, ypos ) ] == -1 ) {
        clusterQueue.push( indexPeriodic( xpos - 1, ypos ) );
        clusterField.data()[ indexPeriodic( xpos - 1, ypos ) ] = setTo;
        ++sum;
    }
    if ( clusterField.data()[ indexPeriodic( xpos, ypos + 1 ) ] == -1 ) {
        clusterQueue.push( indexPeriodic( xpos, ypos + 1 ) );
        clusterField.data()[ indexPeriodic( xpos, ypos + 1 ) ] = setTo;
        ++sum;
    }
    if ( clusterField.data()[ indexPeriodic( xpos, ypos - 1 ) ] == -1 ) {
        clusterQueue.push( indexPeriodic( xpos, ypos - 1 ) );
        clusterField.data()[ indexPeriodic( xpos, ypos - 1 ) ] = setTo;
        ++sum;
    }
    return sum;
}

long int ClusterCounter::makeNewClusterId()
{
    return nextClusterId_++;
}
