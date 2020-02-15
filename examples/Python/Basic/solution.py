import open3d

print("Intel Solution for Open3D by Selim Ozel:")

# Read triangle mesh "test_mesh.ply"
mesh = open3d.io.read_triangle_mesh("test_mesh.ply");
print("Mesh read.")

# Compute adjacency list
mesh.compute_adjacency_list();
print("Adjacency mesh computed.")

# Then get connected components
connected_components = mesh.identically_colored_connected_components()
print("Identically colored components computed.")

# Print connected_components in the specified format
f = open("results.txt", "w")
for row in connected_components:
	rowString = ""
	for node in row:
		rowString += str(node) + " "
	f.write(rowString + "\n")

f.close()

print("Output written to results.txt .")