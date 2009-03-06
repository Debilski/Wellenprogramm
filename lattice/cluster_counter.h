#ifndef CLUSTER_COUNTER_H
#define CLUSTER_COUNTER_H

#include "lattice_geometry.h"
#include "lattice_interface.h"
#include <queue>


class ClusterCounter : public LatticeGeometry {
public:

    const LatticeInterface* lattice_;
    ClusterCounter(int sizeX, int sizeY, int latticeSizeX, int latticeSizeY, const LatticeInterface* lattice = 0);
    ~ClusterCounter();

    void countClusters();
    void printClusters();

    std::vector< Cluster > getClusters();

    //map<long int, double> getClusterSizes();
    //map<long int, SurfacePoint> getClusterPositions();
    void setClusterThreshold(double threshold);
    double clusterThreshold();


    int numberOfClusters();

    blitz::Array<int, 2> clusterField;
private:

    long int nextClusterId_;
    unsigned int numClusters_;
    int distanceThreshold_;
    double sizeThreshold_;
    double clusterThreshold_;


    int clusterWalker(long int position, int setTo);

    std::queue< long int > clusterQueue;
    /*
     map<long int, double> clusterIdSizes;
     map<long int, double> previousClusterIdSizes;
     map<long int, SurfacePoint> clusterIdPositions;
     map<long int, SurfacePoint> previousClusterIdPositions;

     map<long int, double>::const_iterator clusterIdSizesIter;
     map<long int, SurfacePoint>::const_iterator clusterIdPositionsIter;
     */
    std::vector< double > clusterSizes;
    std::vector< SurfacePoint > clusterPositions;

    std::vector< Cluster > clusterVector;
    std::vector< Cluster > previousClusterVector;

    long int makeNewClusterId();
};

#endif

