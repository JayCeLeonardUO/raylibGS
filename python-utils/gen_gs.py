import numpy as np
from plyfile import PlyData, PlyElement

def gen_gs()->np.ndarray:
    # Define the parameters of the Gaussian function
    mu = [10, 10, 10]
    cov = [
        [0.8084480166435242, -0.376901775598526, -0.947654664516449],
        [-0.376901775598526, 2.381552219390869, 1.5799134969711304],
        [-0.947654664516449, 1.5799134969711304, 1.899906873703003],
    ]
    # Generate the data
    X = np.random.multivariate_normal(mu, cov, 10000)
    return X

def save_point_cloud_ply(points: np.ndarray, filename: str = 'output.ply'):
    """
    Convert numpy array of 3D points into a PLY file
    
    Args:
        points: np.ndarray of shape (N, 3) containing 3D points
        filename: output PLY filename
    """
    # Convert points to structured array
    vertices = np.zeros(len(points), dtype=[('x', 'f4'), ('y', 'f4'), ('z', 'f4')])
    vertices['x'] = points[:, 0]
    vertices['y'] = points[:, 1]
    vertices['z'] = points[:, 2]
    
    # Create PLY element
    vertex_element = PlyElement.describe(vertices, 'vertex')
    
    # Write PLY file
    PlyData([vertex_element], text=True).write(filename)

# Usage
if __name__ == "__main__":
    # Generate points
    points = gen_gs()
    
    # Save to PLY
    save_point_cloud_ply(points)