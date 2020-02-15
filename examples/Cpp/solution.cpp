// Input/Output
#include <iostream>

// Files
#include <fstream>

// Open3D
#include "Open3D/Open3D.h"

// Maths
#include <Eigen/Core>

// Namespaces
using namespace open3d;
using namespace std;

int main()
{
    // Read triangle mesh "test_mesh.ply"
    geometry::TriangleMesh mesh;

    // Use the ReadTrianbleMesh function defined under TriangleMeshIO.h
    //io::ReadTriangleMesh("../../examples/TestData/test_mesh_Small.ply", mesh);
    io::ReadTriangleMesh("../../examples/TestData/test_mesh.ply", mesh);

    // Intro messages
    cout << "\nIntel Solution for Open3D by Selim Ozel: \n";
    cout << "Test mesh has " << mesh.vertices_.size() << " vertices. \n";
    cout << "Test mesh has " << mesh.triangles_.size() << " triangles. \n";

    // Compute adjacency list
    mesh.ComputeAdjacencyList();
    if (mesh.HasAdjacencyList())
    {
        cout << "Adjacency mesh exists and computed. \n";
    }  

    // Compute all identically colored connected components
    vector<vector<int>> result = mesh.IdenticallyColoredConnectedComponents();

    // Print results to file
    ofstream outputFileHandle;
    outputFileHandle.open("results.txt");

    for (int i = 0; i<result.size() ; i++)
    {
        for (int j = 0; j<result[i].size(); j++)
        {
            outputFileHandle << result[i][j] << " ";
        }
        outputFileHandle << "\n";
    }
    outputFileHandle.close();

    cout << "Results written to \"results.txt\" \n";

    return 0;   
}