// ----------------------------------------------------------------------------
// -                        Open3D: www.open3d.org                            -
// ----------------------------------------------------------------------------
// The MIT License (MIT)
//
// Copyright (c) 2018 www.open3d.org
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------------------------------------------------------

#include "Open3D/Geometry/TriangleMesh.h"
#include "Open3D/Geometry/Image.h"
#include "Open3D/Geometry/PointCloud.h"

#include "open3d_pybind/docstring.h"
#include "open3d_pybind/geometry/geometry.h"
#include "open3d_pybind/geometry/geometry_trampoline.h"

using namespace open3d;

void pybind_trianglemesh(py::module &m) {
    py::class_<geometry::TriangleMesh, PyGeometry3D<geometry::TriangleMesh>,
               std::shared_ptr<geometry::TriangleMesh>, geometry::MeshBase>
            trianglemesh(m, "TriangleMesh",
                         "TriangleMesh class. Triangle mesh contains vertices "
                         "and triangles represented by the indices to the "
                         "vertices. Optionally, the mesh may also contain "
                         "triangle normals, vertex normals and vertex colors.");
    py::detail::bind_default_constructor<geometry::TriangleMesh>(trianglemesh);
    py::detail::bind_copy_functions<geometry::TriangleMesh>(trianglemesh);
    trianglemesh
            .def(py::init<const std::vector<Eigen::Vector3d> &,
                          const std::vector<Eigen::Vector3i> &>(),
                 "Create a triangle mesh from vertices and triangle indices",
                 "vertices"_a, "triangles"_a)
            .def("__repr__",
                 [](const geometry::TriangleMesh &mesh) {
                     std::string info = fmt::format(
                             "geometry::TriangleMesh with {} points and {} "
                             "triangles",
                             mesh.vertices_.size(), mesh.triangles_.size());

                     if (mesh.HasTexture()) {
                         info += fmt::format(", and ({}, {}) texture.",
                                             mesh.texture_.width_,
                                             mesh.texture_.height_);
                     } else {
                         info += ".";
                     }
                     return info;
                 })
            .def(py::self + py::self)
            .def(py::self += py::self)
            .def("compute_triangle_normals",
                 &geometry::TriangleMesh::ComputeTriangleNormals,
                 "Function to compute triangle normals, usually called before "
                 "rendering",
                 "normalized"_a = true)
            .def("compute_vertex_normals",
                 &geometry::TriangleMesh::ComputeVertexNormals,
                 "Function to compute vertex normals, usually called before "
                 "rendering",
                 "normalized"_a = true)
            .def("compute_adjacency_list",
                 &geometry::TriangleMesh::ComputeAdjacencyList,
                 "Function to compute adjacency list, call before adjacency "
                 "list is needed")
            .def("remove_duplicated_vertices",
                 &geometry::TriangleMesh::RemoveDuplicatedVertices,
                 "Function that removes duplicated verties, i.e., vertices "
                 "that have identical coordinates.")
            .def("remove_duplicated_triangles",
                 &geometry::TriangleMesh::RemoveDuplicatedTriangles,
                 "Function that removes duplicated triangles, i.e., removes "
                 "triangles that reference the same three vertices, "
                 "independent of their order.")
            .def("remove_unreferenced_vertices",
                 &geometry::TriangleMesh::RemoveUnreferencedVertices,
                 "This function removes vertices from the triangle mesh that "
                 "are not referenced in any triangle of the mesh.")
            .def("remove_degenerate_triangles",
                 &geometry::TriangleMesh::RemoveDegenerateTriangles,
                 "Function that removes degenerate triangles, i.e., triangles "
                 "that references a single vertex multiple times in a single "
                 "triangle. They are usually the product of removing "
                 "duplicated vertices.")
            .def("remove_non_manifold_edges",
                 &geometry::TriangleMesh::RemoveNonManifoldEdges,
                 "Function that removes all non-manifold edges, by "
                 "successively deleting  triangles with the smallest surface "
                 "area adjacent to the non-manifold edge until the number of "
                 "adjacent triangles to the edge is `<= 2`.")
            .def("merge_close_vertices",
                 &geometry::TriangleMesh::MergeCloseVertices,
                 "Function that will merge close by vertices to a single one. "
                 "The vertex position, "
                 "normal and color will be the average of the vertices. The "
                 "parameter eps "
                 "defines the maximum distance of close by vertices.  This "
                 "function might help to "
                 "close triangle soups.",
                 "eps"_a)
            .def("filter_sharpen", &geometry::TriangleMesh::FilterSharpen,
                 "Function to sharpen triangle mesh. The output value "
                 "(:math:`v_o`) is the input value (:math:`v_i`) plus strength "
                 "times the input value minus he sum of he adjacent values. "
                 ":math:`v_o = v_i x strength (v_i * |N| - \\sum_{n \\in N} "
                 "v_n)`",
                 "number_of_iterations"_a = 1, "strength"_a = 1,
                 "filter_scope"_a = geometry::MeshBase::FilterScope::All)
            .def("filter_smooth_simple",
                 &geometry::TriangleMesh::FilterSmoothSimple,
                 "Function to smooth triangle mesh with simple neighbour "
                 "average. :math:`v_o = \\frac{v_i + \\sum_{n \\in N} "
                 "v_n)}{|N| + 1}`, with :math:`v_i` being the input value, "
                 ":math:`v_o` the output value, and :math:`N` is the set of "
                 "adjacent neighbours.",
                 "number_of_iterations"_a = 1,
                 "filter_scope"_a = geometry::MeshBase::FilterScope::All)
            .def("filter_smooth_laplacian",
                 &geometry::TriangleMesh::FilterSmoothLaplacian,
                 "Function to smooth triangle mesh using Laplacian. :math:`v_o "
                 "= v_i \\cdot \\lambda (sum_{n \\in N} w_n v_n - v_i)`, with "
                 ":math:`v_i` being the input value, :math:`v_o` the output "
                 "value, :math:`N` is the  set of adjacent neighbours, "
                 ":math:`w_n` is the weighting of the neighbour based on the "
                 "inverse distance (closer neighbours have higher weight), and "
                 "lambda is the smoothing parameter.",
                 "number_of_iterations"_a = 1, "lambda"_a = 0.5,
                 "filter_scope"_a = geometry::MeshBase::FilterScope::All)
            .def("filter_smooth_taubin",
                 &geometry::TriangleMesh::FilterSmoothTaubin,
                 "Function to smooth triangle mesh using method of Taubin, "
                 "\"Curve and Surface Smoothing Without Shrinkage\", 1995. "
                 "Applies in each iteration two times filter_smooth_laplacian, "
                 "first with filter parameter lambda and second with filter "
                 "parameter mu as smoothing parameter. This method avoids "
                 "shrinkage of the triangle mesh.",
                 "number_of_iterations"_a = 1, "lambda"_a = 0.5, "mu"_a = -0.53,
                 "filter_scope"_a = geometry::MeshBase::FilterScope::All)
            .def("has_vertices", &geometry::TriangleMesh::HasVertices,
                 "Returns ``True`` if the mesh contains vertices.")
            .def("has_triangles", &geometry::TriangleMesh::HasTriangles,
                 "Returns ``True`` if the mesh contains triangles.")
            .def("has_vertex_normals",
                 &geometry::TriangleMesh::HasVertexNormals,
                 "Returns ``True`` if the mesh contains vertex normals.")
            .def("has_vertex_colors", &geometry::TriangleMesh::HasVertexColors,
                 "Returns ``True`` if the mesh contains vertex colors.")
            .def("has_triangle_normals",
                 &geometry::TriangleMesh::HasTriangleNormals,
                 "Returns ``True`` if the mesh contains triangle normals.")
            .def("has_adjacency_list",
                 &geometry::TriangleMesh::HasAdjacencyList,
                 "Returns ``True`` if the mesh contains adjacency normals.")
            .def("has_triangle_uvs", &geometry::TriangleMesh::HasTriangleUvs,
                 "Returns ``True`` if the mesh contains uv coordinates.")
            .def("has_texture", &geometry::TriangleMesh::HasTexture,
                 "Returns ``True`` if the mesh contains a texture image.")
            .def("normalize_normals", &geometry::TriangleMesh::NormalizeNormals,
                 "Normalize both triangle normals and vertex normals to legnth "
                 "1.")
            .def("paint_uniform_color",
                 &geometry::TriangleMesh::PaintUniformColor,
                 "Assigns each vertex in the TriangleMesh the same color.")
            .def("euler_poincare_characteristic",
                 &geometry::TriangleMesh::EulerPoincareCharacteristic,
                 "Function that computes the Euler-Poincaré characteristic, "
                 "i.e., V + F - E, where V is the number of vertices, F is the "
                 "number of triangles, and E is the number of edges.")
            .def("get_non_manifold_edges",
                 &geometry::TriangleMesh::GetNonManifoldEdges,
                 "Get list of non-manifold edges.",
                 "allow_boundary_edges"_a = true)
            .def("is_edge_manifold", &geometry::TriangleMesh::IsEdgeManifold,
                 "Tests if the triangle mesh is edge manifold.",
                 "allow_boundary_edges"_a = true)
            .def("get_non_manifold_vertices",
                 &geometry::TriangleMesh::GetNonManifoldVertices,
                 "Returns a list of indices to non-manifold vertices.")
            .def("is_vertex_manifold",
                 &geometry::TriangleMesh::IsVertexManifold,
                 "Tests if all vertices of the triangle mesh are manifold.")
            .def("is_self_intersecting",
                 &geometry::TriangleMesh::IsSelfIntersecting,
                 "Tests if the triangle mesh is self-intersecting.")
            .def("get_self_intersecting_triangles",
                 &geometry::TriangleMesh::GetSelfIntersectingTriangles,
                 "Returns a list of indices to triangles that intersect the "
                 "mesh.")
            .def("is_intersecting", &geometry::TriangleMesh::IsIntersecting,
                 "Tests if the triangle mesh is intersecting the other "
                 "triangle mesh.")
            .def("is_orientable", &geometry::TriangleMesh::IsOrientable,
                 "Tests if the triangle mesh is orientable.")
            .def("is_watertight", &geometry::TriangleMesh::IsWatertight,
                 "Tests if the triangle mesh is watertight.")
            .def("orient_triangles", &geometry::TriangleMesh::OrientTriangles,
                 "If the mesh is orientable this function orients all "
                 "triangles such that all normals point towards the same "
                 "direction.")
            .def("select_down_sample",
                 &geometry::TriangleMesh::SelectDownSample,
                 "Function to select mesh from input triangle mesh into output "
                 "triangle mesh. ``input``: The input triangle mesh. "
                 "``indices``: "
                 "Indices of vertices to be selected.",
                 "indices"_a)
            .def("crop",
                 (std::shared_ptr<geometry::TriangleMesh>(
                         geometry::TriangleMesh::*)(
                         const geometry::AxisAlignedBoundingBox &) const) &
                         geometry::TriangleMesh::Crop,
                 "Function to crop input TriangleMesh into output TriangleMesh",
                 "bounding_box"_a)
            .def("crop",
                 (std::shared_ptr<geometry::TriangleMesh>(
                         geometry::TriangleMesh::*)(
                         const geometry::OrientedBoundingBox &) const) &
                         geometry::TriangleMesh::Crop,
                 "Function to crop input TriangleMesh into output TriangleMesh",
                 "bounding_box"_a)
            .def("sample_points_uniformly",
                 &geometry::TriangleMesh::SamplePointsUniformly,
                 "Function to uniformly sample points from the mesh.",
                 "number_of_points"_a = 100)
            .def("sample_points_poisson_disk",
                 &geometry::TriangleMesh::SamplePointsPoissonDisk,
                 "Function to sample points from the mesh, where each point "
                 "has "
                 "approximately the same distance to the neighbouring points "
                 "(blue "
                 "noise). Method is based on Yuksel, \"Sample Elimination for "
                 "Generating Poisson Disk Sample Sets\", EUROGRAPHICS, 2015.",
                 "number_of_points"_a, "init_factor"_a = 5, "pcl"_a = nullptr)
            .def("subdivide_midpoint",
                 &geometry::TriangleMesh::SubdivideMidpoint,
                 "Function subdivide mesh using midpoint algorithm.",
                 "number_of_iterations"_a = 1)
            .def("subdivide_loop", &geometry::TriangleMesh::SubdivideLoop,
                 "Function subdivide mesh using Loop's algorithm. Loop, "
                 "\"Smooth "
                 "subdivision surfaces based on triangles\", 1987.",
                 "number_of_iterations"_a = 1)
            .def("simplify_vertex_clustering",
                 &geometry::TriangleMesh::SimplifyVertexClustering,
                 "Function to simplify mesh using vertex clustering.",
                 "voxel_size"_a,
                 "contraction"_a =
                         geometry::MeshBase::SimplificationContraction::Average)
            .def("simplify_quadric_decimation",
                 &geometry::TriangleMesh::SimplifyQuadricDecimation,
                 "Function to simplify mesh using Quadric Error Metric "
                 "Decimation by "
                 "Garland and Heckbert",
                 "target_number_of_triangles"_a)
            .def("compute_convex_hull",
                 &geometry::TriangleMesh::ComputeConvexHull,
                 "Computes the convex hull of the triangle mesh.")
            .def("cluster_connected_triangles",
                 &geometry::TriangleMesh::ClusterConnectedTriangles,
                 "Function that clusters connected triangles, i.e., triangles "
                 "that are connected via edges are assigned the same cluster "
                 "index.  This function retuns an array that contains the "
                 "cluster index per triangle, a second array contains the "
                 "number of triangles per cluster, and a third vector contains "
                 "the surface area per cluster.")
            .def("remove_triangles_by_index",
                 &geometry::TriangleMesh::RemoveTrianglesByIndex,
                 "This function removes the triangles with index in "
                 "triangle_indices.  Call remove_unreferenced_vertices to "
                 "clean up vertices afterwards.",
                 "triangle_indices"_a)
            .def("remove_triangles_by_mask",
                 &geometry::TriangleMesh::RemoveTrianglesByMask,
                 "This function removes the triangles where triangle_mask is "
                 "set to true.  Call remove_unreferenced_vertices to clean up "
                 "vertices afterwards.",
                 "triangle_mask"_a)
            .def("remove_vertices_by_index",
                 &geometry::TriangleMesh::RemoveVerticesByIndex,
                 "This function removes the vertices with index in "
                 "vertex_indices. Note that also all triangles associated with "
                 "the vertices are removed.",
                 "vertex_indices"_a)
            .def("remove_vertices_by_mask",
                 &geometry::TriangleMesh::RemoveVerticesByMask,
                 "This function removes the vertices that are masked in "
                 "vertex_mask. Note that also all triangles associated with "
                 "the vertices are removed.",
                 "vertex_mask"_a)
            .def("identically_colored_connected_components",
                 &geometry::TriangleMesh::IdenticallyColoredConnectedComponents,
                 "Computes identically colored connected components and returns a 2D matrix containing node numbers.")            
            .def("deform_as_rigid_as_possible",
                 &geometry::TriangleMesh::DeformAsRigidAsPossible,
                 "This function deforms the mesh using the method by Sorkine "
                 "and Alexa, "
                 "'As-Rigid-As-Possible Surface Modeling', 2007",
                 "constraint_vertex_indices"_a, "constraint_vertex_positions"_a,
                 "max_iter"_a)
            .def_static("create_from_point_cloud_alpha_shape",
                        &geometry::TriangleMesh::CreateFromPointCloudAlphaShape,
                        "Alpha shapes are a generalization of the convex hull. "
                        "With decreasing alpha value the shape schrinks and "
                        "creates cavities. See Edelsbrunner and Muecke, "
                        "\"Three-Dimensional Alpha Shapes\", 1994.",
                        "pcd"_a, "alpha"_a, "tetra_mesh"_a, "pt_map"_a)
            .def_static(
                    "create_from_point_cloud_ball_pivoting",
                    &geometry::TriangleMesh::CreateFromPointCloudBallPivoting,
                    "Function that computes a triangle mesh from a oriented "
                    "PointCloud. This implements the Ball Pivoting algorithm "
                    "proposed in F. Bernardini et al., \"The ball-pivoting "
                    "algorithm for surface reconstruction\", 1999. The "
                    "implementation is also based on the algorithms outlined "
                    "in Digne, \"An Analysis and Implementation of a Parallel "
                    "Ball Pivoting Algorithm\", 2014. The surface "
                    "reconstruction is done by rolling a ball with a given "
                    "radius over the point cloud, whenever the ball touches "
                    "three points a triangle is created.",
                    "pcd"_a, "radii"_a)
            .def_static("create_from_point_cloud_poisson",
                        &geometry::TriangleMesh::CreateFromPointCloudPoisson,
                        "Function that computes a triangle mesh from a "
                        "oriented PointCloud pcd. This implements the Screened "
                        "Poisson Reconstruction proposed in Kazhdan and Hoppe, "
                        "\"Screened Poisson Surface Reconstruction\", 2013. "
                        "This function uses the original implementation by "
                        "Kazhdan. See https://github.com/mkazhdan/PoissonRecon",
                        "pcd"_a, "depth"_a = 8, "width"_a = 0, "scale"_a = 1.1,
                        "linear_fit"_a = false)
            .def_static("create_box", &geometry::TriangleMesh::CreateBox,
                        "Factory function to create a box. The left bottom "
                        "corner on the "
                        "front will be placed at (0, 0, 0).",
                        "width"_a = 1.0, "height"_a = 1.0, "depth"_a = 1.0)
            .def_static("create_tetrahedron",
                        &geometry::TriangleMesh::CreateTetrahedron,
                        "Factory function to create a tetrahedron. The "
                        "centroid of the mesh "
                        "will be placed at (0, 0, 0) and the vertices have a "
                        "distance of "
                        "radius to the center.",
                        "radius"_a = 1.0)
            .def_static("create_octahedron",
                        &geometry::TriangleMesh::CreateOctahedron,
                        "Factory function to create a octahedron. The centroid "
                        "of the mesh "
                        "will be placed at (0, 0, 0) and the vertices have a "
                        "distance of "
                        "radius to the center.",
                        "radius"_a = 1.0)
            .def_static("create_icosahedron",
                        &geometry::TriangleMesh::CreateIcosahedron,
                        "Factory function to create a icosahedron. The "
                        "centroid of the mesh "
                        "will be placed at (0, 0, 0) and the vertices have a "
                        "distance of "
                        "radius to the center.",
                        "radius"_a = 1.0)
            .def_static("create_sphere", &geometry::TriangleMesh::CreateSphere,
                        "Factory function to create a sphere mesh centered at "
                        "(0, 0, 0).",
                        "radius"_a = 1.0, "resolution"_a = 20)
            .def_static("create_cylinder",
                        &geometry::TriangleMesh::CreateCylinder,
                        "Factory function to create a cylinder mesh.",
                        "radius"_a = 1.0, "height"_a = 2.0, "resolution"_a = 20,
                        "split"_a = 4)
            .def_static("create_cone", &geometry::TriangleMesh::CreateCone,
                        "Factory function to create a cone mesh.",
                        "radius"_a = 1.0, "height"_a = 2.0, "resolution"_a = 20,
                        "split"_a = 1)
            .def_static("create_torus", &geometry::TriangleMesh::CreateTorus,
                        "Factory function to create a torus mesh.",
                        "torus_radius"_a = 1.0, "tube_radius"_a = 0.5,
                        "radial_resolution"_a = 30, "tubular_resolution"_a = 20)
            .def_static("create_arrow", &geometry::TriangleMesh::CreateArrow,
                        "Factory function to create an arrow mesh",
                        "cylinder_radius"_a = 1.0, "cone_radius"_a = 1.5,
                        "cylinder_height"_a = 5.0, "cone_height"_a = 4.0,
                        "resolution"_a = 20, "cylinder_split"_a = 4,
                        "cone_split"_a = 1)
            .def_static("create_coordinate_frame",
                        &geometry::TriangleMesh::CreateCoordinateFrame,
                        "Factory function to create a coordinate frame mesh. "
                        "The coordinate "
                        "frame will be centered at ``origin``. The x, y, z "
                        "axis will be "
                        "rendered as red, green, and blue arrows respectively.",
                        "size"_a = 1.0,
                        "origin"_a = Eigen::Vector3d(0.0, 0.0, 0.0))
            .def_static("create_moebius",
                        &geometry::TriangleMesh::CreateMoebius,
                        "Factory function to create a Moebius strip.",
                        "length_split"_a = 70, "width_split"_a = 15,
                        "twists"_a = 1, "raidus"_a = 1, "flatness"_a = 1,
                        "width"_a = 1, "scale"_a = 1)
            .def_readwrite("vertices", &geometry::TriangleMesh::vertices_,
                           "``float64`` array of shape ``(num_vertices, 3)``, "
                           "use ``numpy.asarray()`` to access data: Vertex "
                           "coordinates.")
            .def_readwrite("vertex_normals",
                           &geometry::TriangleMesh::vertex_normals_,
                           "``float64`` array of shape ``(num_vertices, 3)``, "
                           "use ``numpy.asarray()`` to access data: Vertex "
                           "normals.")
            .def_readwrite(
                    "vertex_colors", &geometry::TriangleMesh::vertex_colors_,
                    "``float64`` array of shape ``(num_vertices, 3)``, "
                    "range ``[0, 1]`` , use ``numpy.asarray()`` to access "
                    "data: RGB colors of vertices.")
            .def_readwrite("triangles", &geometry::TriangleMesh::triangles_,
                           "``int`` array of shape ``(num_triangles, 3)``, use "
                           "``numpy.asarray()`` to access data: List of "
                           "triangles denoted by the index of points forming "
                           "the triangle.")
            .def_readwrite("triangle_normals",
                           &geometry::TriangleMesh::triangle_normals_,
                           "``float64`` array of shape ``(num_triangles, 3)``, "
                           "use ``numpy.asarray()`` to access data: Triangle "
                           "normals.")
            .def_readwrite(
                    "adjacency_list", &geometry::TriangleMesh::adjacency_list_,
                    "List of Sets: The set ``adjacency_list[i]`` contains the "
                    "indices of adjacent vertices of vertex i.")
            .def_readwrite("triangle_uvs",
                           &geometry::TriangleMesh::triangle_uvs_,
                           "``float64`` array of shape ``(3 * num_triangles, "
                           "2)``, use "
                           "``numpy.asarray()`` to access data: List of "
                           "uvs denoted by the index of points forming "
                           "the triangle.")
            .def_readwrite("texture", &geometry::TriangleMesh::texture_,
                           "open3d.geometry.Image: The texture image.");
    docstring::ClassMethodDocInject(m, "TriangleMesh",
                                    "compute_adjacency_list");
    docstring::ClassMethodDocInject(m, "TriangleMesh",
                                    "compute_triangle_normals");
    docstring::ClassMethodDocInject(m, "TriangleMesh",
                                    "compute_vertex_normals");
    docstring::ClassMethodDocInject(m, "TriangleMesh", "has_adjacency_list");
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "has_triangle_normals",
            {{"normalized",
              "Set to ``True`` to normalize the normal to length 1."}});
    docstring::ClassMethodDocInject(m, "TriangleMesh", "has_triangles");
    docstring::ClassMethodDocInject(m, "TriangleMesh", "has_triangle_uvs");
    docstring::ClassMethodDocInject(m, "TriangleMesh", "has_texture");
    docstring::ClassMethodDocInject(m, "TriangleMesh", "has_vertex_colors");
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "has_vertex_normals",
            {{"normalized",
              "Set to ``True`` to normalize the normal to length 1."}});
    docstring::ClassMethodDocInject(m, "TriangleMesh", "has_vertices");
    docstring::ClassMethodDocInject(m, "TriangleMesh", "normalize_normals");
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "paint_uniform_color",
            {{"color", "RGB color for the PointCloud."}});
    docstring::ClassMethodDocInject(m, "TriangleMesh",
                                    "euler_poincare_characteristic");
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "get_non_manifold_edges",
            {{"allow_boundary_edges",
              "If true, than non-manifold edges are defined as edges with more "
              "than two adjacent triangles, otherwise each edge that is not "
              "adjacent to two triangles is defined as non-manifold."}});
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "is_edge_manifold",
            {{"allow_boundary_edges",
              "If true, than non-manifold edges are defined as edges with more "
              "than two adjacent triangles, otherwise each edge that is not "
              "adjacent to two triangles is defined as non-manifold."}});
    docstring::ClassMethodDocInject(m, "TriangleMesh", "is_vertex_manifold");
    docstring::ClassMethodDocInject(m, "TriangleMesh",
                                    "get_non_manifold_vertices");
    docstring::ClassMethodDocInject(m, "TriangleMesh", "is_self_intersecting");
    docstring::ClassMethodDocInject(m, "TriangleMesh",
                                    "get_self_intersecting_triangles");
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "is_intersecting",
            {{"other", "Other triangle mesh to test intersection with."}});
    docstring::ClassMethodDocInject(m, "TriangleMesh", "is_orientable");
    docstring::ClassMethodDocInject(m, "TriangleMesh", "is_watertight");
    docstring::ClassMethodDocInject(m, "TriangleMesh", "orient_triangles");
    docstring::ClassMethodDocInject(m, "TriangleMesh",
                                    "remove_duplicated_vertices");
    docstring::ClassMethodDocInject(m, "TriangleMesh",
                                    "remove_duplicated_triangles");
    docstring::ClassMethodDocInject(m, "TriangleMesh",
                                    "remove_unreferenced_vertices");
    docstring::ClassMethodDocInject(m, "TriangleMesh",
                                    "remove_degenerate_triangles");
    docstring::ClassMethodDocInject(m, "TriangleMesh",
                                    "remove_non_manifold_edges");
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "merge_close_vertices",
            {{"eps",
              "Parameter that defines the distance between close vertices."}});
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "filter_sharpen",
            {{"number_of_iterations",
              " Number of repetitions of this operation"},
             {"strengh", "Filter parameter."},
             {"scope", "Mesh property that should be filtered."}});
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "filter_smooth_simple",
            {{"number_of_iterations",
              " Number of repetitions of this operation"},
             {"scope", "Mesh property that should be filtered."}});
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "filter_smooth_laplacian",
            {{"number_of_iterations",
              " Number of repetitions of this operation"},
             {"lambda", "Filter parameter."},
             {"scope", "Mesh property that should be filtered."}});
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "filter_smooth_taubin",
            {{"number_of_iterations",
              " Number of repetitions of this operation"},
             {"lambda", "Filter parameter."},
             {"mu", "Filter parameter."},
             {"scope", "Mesh property that should be filtered."}});
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "select_down_sample",
            {{"indices", "Indices of vertices to be selected."}});
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "crop",
            {{"bounding_box", "AxisAlignedBoundingBox to crop points"}});
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "sample_points_uniformly",
            {{"number_of_points",
              "Number of points that should be uniformly sampled."}});
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "sample_points_poisson_disk",
            {{"number_of_points", "Number of points that should be sampled."},
             {"init_factor",
              "Factor for the initial uniformly sampled PointCloud. This init "
              "PointCloud is used for sample elimination."},
             {"pcl",
              "Initial PointCloud that is used for sample elimination. If this "
              "parameter is provided the init_factor is ignored."}});
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "subdivide_midpoint",
            {{"number_of_iterations",
              "Number of iterations. A single iteration splits each triangle "
              "into four triangles that cover the same surface."}});
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "subdivide_loop",
            {{"number_of_iterations",
              "Number of iterations. A single iteration splits each triangle "
              "into four triangles."}});
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "simplify_vertex_clustering",
            {{"voxel_size",
              "The size of the voxel within vertices are pooled."},
             {"contraction",
              "Method to aggregate vertex information. Average computes a "
              "simple average, Quadric minimizes the distance to the adjacent "
              "planes."}});
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "simplify_quadric_decimation",
            {{"target_number_of_triangles",
              "The number of triangles that the simplified mesh should have. "
              "It is not guranteed that this number will be reached."}});
    docstring::ClassMethodDocInject(m, "TriangleMesh", "compute_convex_hull");
    docstring::ClassMethodDocInject(m, "TriangleMesh",
                                    "cluster_connected_triangles");
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "remove_triangles_by_index",
            {{"triangle_indices",
              "1D array of triangle indices that should be removed from the "
              "TriangleMesh."}});
    docstring::ClassMethodDocInject(m, "TriangleMesh",
                                    "remove_triangles_by_mask",
                                    {{"triangle_mask",
                                      "1D bool array, True values indicate "
                                      "triangles that should be removed."}});
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "remove_vertices_by_index",
            {{"vertex_indices",
              "1D array of vertex indices that should be removed from the "
              "TriangleMesh."}});
    docstring::ClassMethodDocInject(m, "TriangleMesh",
                                    "remove_vertices_by_mask",
                                    {{"vertex_mask",
                                      "1D bool array, True values indicate "
                                      "vertices that should be removed."}});
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "deform_as_rigid_as_possible",
            {{"constraint_vertex_indices",
              "Indices of the triangle vertices that should be constrained by "
              "the vertex positions "
              "in constraint_vertex_positions."},
             {"constraint_vertex_positions",
              "Vertex positions used for the constraints."},
             {"max_iter",
              "Maximum number of iterations to minimize energy functional."}});
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "create_from_point_cloud_alpha_shape",
            {{"pcd",
              "PointCloud from whicht the TriangleMesh surface is "
              "reconstructed."},
             {"alpha",
              "Parameter to controll the shape. A very big value will give a "
              "shape close to the convex hull."},
             {"tetra_mesh",
              "If not None, than uses this to construct the alpha shape. "
              "Otherwise, TetraMesh is computed from pcd."},
             {"pt_map",
              "Optional map from tetra_mesh vertex indices to pcd points."}});
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "create_from_point_cloud_ball_pivoting",
            {{"pcd",
              "PointCloud from whicht the TriangleMesh surface is "
              "reconstructed. Has to contain normals."},
             {"radii",
              "The radii of the ball that are used for the surface "
              "reconstruction."}});
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "create_from_point_cloud_poisson",
            {{"pcd",
              "PointCloud from which the TriangleMesh surface is "
              "reconstructed. Has to contain normals."},
             {"depth",
              "Maximum depth of the tree that will be used for surface "
              "reconstruction. Running at depth d corresponds to solving on a "
              "grid whose resolution is no larger than 2^d x 2^d x 2^d. Note "
              "that since the reconstructor adapts the octree to the sampling "
              "density, the specified reconstruction depth is only an upper "
              "bound."},
             {"width",
              "Specifies the target width of the finest level octree cells. "
              "This parameter is ignored if depth is specified"},
             {"scale",
              "Specifies the ratio between the diameter of the cube used for "
              "reconstruction and the diameter of the samples' bounding cube."},
             {"linear_fit",
              "If true, the reconstructor use linear interpolation to estimate "
              "the positions of iso-vertices."}});
    docstring::ClassMethodDocInject(m, "TriangleMesh", "create_box",
                                    {{"width", "x-directional length."},
                                     {"height", "y-directional length."},
                                     {"depth", "z-directional length."}});
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "create_tetrahedron",
            {{"radius", "Distance from centroid to mesh vetices."}});
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "create_octahedron",
            {{"radius", "Distance from centroid to mesh vetices."}});
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "create_icosahedron",
            {{"radius", "Distance from centroid to mesh vetices."}});
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "create_sphere",
            {{"radius", "The radius of the sphere."},
             {"resolution",
              "The resolution of the sphere. The longitues will be split into "
              "``resolution`` segments (i.e. there are ``resolution + 1`` "
              "latitude lines including the north and south pole). The "
              "latitudes will be split into ```2 * resolution`` segments (i.e. "
              "there are ``2 * resolution`` longitude lines.)"}});
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "create_cylinder",
            {{"radius", "The radius of the cylinder."},
             {"height",
              "The height of the cylinder. The axis of the cylinder will be "
              "from (0, 0, -height/2) to (0, 0, height/2)."},
             {"resolution",
              " The circle will be split into ``resolution`` segments"},
             {"split",
              "The ``height`` will be split into ``split`` segments."}});
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "create_cone",
            {{"radius", "The radius of the cone."},
             {"height",
              "The height of the cone. The axis of the cone will be from (0, "
              "0, 0) to (0, 0, height)."},
             {"resolution",
              "The circle will be split into ``resolution`` segments"},
             {"split",
              "The ``height`` will be split into ``split`` segments."}});
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "create_torus",
            {{"torus_radius",
              "The radius from the center of the torus to the center of the "
              "tube."},
             {"tube_radius", "The radius of the torus tube."},
             {"radial_resolution",
              "The number of segments along the radial direction."},
             {"tubular_resolution",
              "The number of segments along the tubular direction."}});
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "create_arrow",
            {{"cylinder_radius", "The radius of the cylinder."},
             {"cone_radius", "The radius of the cone."},
             {"cylinder_height",
              "The height of the cylinder. The cylinder is from (0, 0, 0) to "
              "(0, 0, cylinder_height)"},
             {"cone_height",
              "The height of the cone. The axis of the cone will be from (0, "
              "0, cylinder_height) to (0, 0, cylinder_height + cone_height)"},
             {"resolution",
              "The cone will be split into ``resolution`` segments."},
             {"cylinder_split",
              "The ``cylinder_height`` will be split into ``cylinder_split`` "
              "segments."},
             {"cone_split",
              "The ``cone_height`` will be split into ``cone_split`` "
              "segments."}});
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "create_coordinate_frame",
            {{"size", "The size of the coordinate frame."},
             {"origin", "The origin of the cooridnate frame."}});
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "create_moebius",
            {{"length_split",
              "The number of segments along the Moebius strip."},
             {"width_split",
              "The number of segments along the width of the Moebius strip."},
             {"twists", "Number of twists of the Moebius strip."},
             {"radius", "The radius of the Moebius strip."},
             {"flatness", "Controls the flatness/height of the Moebius strip."},
             {"width", "Width of the Moebius strip."},
             {"scale", "Scale the complete Moebius strip."}});
}

void pybind_trianglemesh_methods(py::module &m) {}
