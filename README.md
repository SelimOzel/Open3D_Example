## Intel Assignment, Selim Ozel
I solved the problem using depth search first (DFS). Two functions are implemented under the TriangleMesh class: ```IdenticallyColoredConnectedComponents()``` and ```DepthFirstSearch(int input_node, std::vector<int> &input_neighbor_nodes, std::vector<Eigen::Vector3d> &input_vertex_colors, Eigen::Vector3d input_search_color, std::vector<int> &return_vector)```.

The algorithm first computes each different color in the mesh. This requires O(N^2) complexity where N is the number of nodes. The find function can also search up to linear complexity within the color vector. It can be optimized further by using a sorted color list or a hash-table for storing new colors.

Second part of the algorithm implements a DFS for each different color existing on the graph. Within the color loop it searches through each node in the graph, if that node is of the same color as the current search color. DFS algorithm looks up to adjacency matrix to find connected nodes. It moves to neighbour nodes one-by-one if they are the same color as the search color. It also marks previously visited nodes with ```termination_color``` to avoid cycling. This increases space complexity of the algorithm since I keep track of ```vertex_colors_```. The runtime complexity of the second part is O(CxDxB) where C is number of different colors in the mesh, D is depth of the graph and B is its breadth.

I added two ```*.ply``` files to the project. They are located under TestData folder to be compatible with the design of the repository. ```test_mesh.ply``` is the one given with the assignment. ```test_mesh_Small``` contains the mesh described in the assignment document.

The solutions for both python and Cpp can be found under examples/python/basic and examples/Cpp folders. See [commit d72448](https://github.com/SelimOzel/Open3D_Example/commit/d72448914a05e6409597f9094b1a5a8bd76226f3) for python solution file and its bindings. See [commit fa1575](https://github.com/SelimOzel/Open3D_Example/commit/fa1575985867ef6179cca89f2177e8692179c09c) for Cpp solution. Note that file locations are updated in a [later commit](https://github.com/SelimOzel/Open3D_Example/commit/bb7f2e1bb1910a206617e95973aa7e7398fc2f4d).

Both solutions read input data using ReadTriangleMesh implemented inside the Open3D library. They compute the adjacency matrix, which is needed for node traversal. Once adjacency matrix is computed, IdenticallyColoredConnectedComponents is called and it outputs a 2D integer matrix. Each row in this matrix contains id values of identically-colored-connected nodes. It is sorted ascendingly by the smallest element. Within each row id values are also sorted ascendingly. Finally, both python and Cpp solutions output 2D matrix contents into a results.txt file via standard file access libraries.

I turned on the BUILD_UNIT_TESTS flag in the cmake file. I added the unit test for IdenticallyColoredConnectedComponents function under UnitTes/Geometry/TriangleMesh.cpp file. Unit testing implementation can be found in [commit 022bf4](https://github.com/SelimOzel/Open3D_Example/commit/022bf4ade4161887af89e14b64503cd0ba575712).

Instructions to build and observe my results are as follows. Note that I developed and tested them on a Macbook Pro (Early 2015), macOS Mojave v10.14.6. 
## To Build Open3D
```shell
git clone --recursive https://github.com/SelimOzel/Open3D_Example
cd Open3D_Example
util/scripts/install-deps-osx.sh
python3 -m venv env
source env/bin/activate
cmake -DCMAKE_INSTALL_PREFIX=/Users/selimozel/Desktop/intel-project/Open3D_Example/build -DPYTHON_EXECUTABLE=/usr/local/bin/python3
make -j$(sysctl -n hw.physicalcpu)
make install-pip-package 
make install 
```

## Solution Instructions - Cpp:
```shell
cd <Open3D_Example>/Examples/Cpp
make
cd <Open3D_Example>/bin/examples
./solution
```

## Solution Instructions - Python:
```shell
cd <Open3D_Example>/Examples/Python/Basic
python3 solution.py
```

## Solution Instructions - Unit Tests:
```shell
cd <Open3D_Example>/src/UnitTes
make
cd <Open3D_Example>/bin
./unitTests
```

<p align="center">
<img src="docs/_static/open3d_logo_horizontal.png" width="320" />
</p>

# Open3D: A Modern Library for 3D Data Processing

<h4>
    <a href="http://www.open3d.org">open3d.org</a> |
    <a href="http://www.open3d.org/docs">Documentation</a> |
    <a href="http://www.open3d.org/docs/release/getting_started.html">Quick Start</a> |
    <a href="http://www.open3d.org/docs/release/compilation.html">Build from Source</a> |
    <a href="http://www.open3d.org/docs/release/index.html#python-api-index">Python API</a> |
    <a href="http://www.open3d.org/docs/release/cpp_api/index.html">C++ API</a> |
    <a href="http://www.open3d.org/docs/release/contribute.html">Contribute</a> |
    <a href="https://www.youtube.com/watch?v=I3UjXlA4IsU">Demo</a> |
    <a href="https://forum.open3d.org">Forum</a>
</h4>

Open3D is an open-source library that supports rapid development of software that deals with 3D data. The Open3D frontend exposes a set of carefully selected data structures and algorithms in both C++ and Python. The backend is highly optimized and is set up for parallelization. We welcome contributions from the open-source community.

## Core features

* 3D data structures
* 3D data processing algorithms
* Scene reconstruction
* Surface alignment
* 3D visualization
* Python binding

## Supported OSes and compilers

* Ubuntu 16.04 or newer: GCC 5.x or newer [![Build Status](https://travis-ci.org/intel-isl/Open3D.svg?branch=master)](https://travis-ci.org/intel-isl/Open3D)
* macOS: XCode 8.0 or newer [![Build Status](https://travis-ci.org/intel-isl/Open3D.svg?branch=master)](https://travis-ci.org/intel-isl/Open3D)
* Windows: Visual Studio 2017 or newer [![Build status](https://ci.appveyor.com/api/projects/status/3hasjo041lv6srsi/branch/master?svg=true)](https://ci.appveyor.com/project/yxlao/open3d/branch/master)

## Citation
Please cite [our work](https://arxiv.org/abs/1801.09847) if you use Open3D.

```
@article{Zhou2018,
	author    = {Qian-Yi Zhou and Jaesik Park and Vladlen Koltun},
	title     = {{Open3D}: {A} Modern Library for {3D} Data Processing},
	journal   = {arXiv:1801.09847},
	year      = {2018},
}
```
