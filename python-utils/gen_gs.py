import numpy as np

def generate_ply(vertices, faces, filename):
    """
    Generates a .ply file for Blender.

    Parameters:
    vertices (list of tuple): List of vertex coordinates (x, y, z).
    faces (list of tuple): List of faces, each face is a tuple of vertex indices.
    filename (str): The name of the output .ply file.
    """
    with open(filename, 'w') as ply_file:
        # Write the header
        ply_file.write("ply\n")
        ply_file.write("format ascii 1.0\n")
        ply_file.write(f"element vertex {len(vertices)}\n")
        ply_file.write("property float x\n")
        ply_file.write("property float y\n")
        ply_file.write("property float z\n")
        ply_file.write(f"element face {len(faces)}\n")
        ply_file.write("property list uchar int vertex_indices\n")
        ply_file.write("end_header\n")

        # Write the vertex data
        for vertex in vertices:
            ply_file.write(f"{vertex[0]} {vertex[1]} {vertex[2]}\n")

        # Write the face data
        for face in faces:
            ply_file.write(f"{len(face)} {' '.join(map(str, face))}\n")

if __name__ == "__main__":
    # Example usage
    vertices = [(0, 0, 0), (1, 0, 0), (1, 1, 0), (0, 1, 0), (0, 0, 1), (1, 0, 1), (1, 1, 1), (0, 1, 1)]
    faces = [(0, 1, 2, 3), (4, 5, 6, 7), (0, 1, 5, 4), (2, 3, 7, 6), (0, 3, 7, 4), (1, 2, 6, 5)]
    generate_ply(vertices, faces, "output.ply")