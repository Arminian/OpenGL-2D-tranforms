import numpy as np

def generate_decagon_coordinates():
    # Number of vertices
    n_vertices = 10
    
    # Angle step in radians for each vertex in a regular decagon
    angle_step = 2 * np.pi / n_vertices
    
    # Generate x and y coordinates and divide each by 2
    coordinates = [
        (np.cos(i * angle_step) / 2, np.sin(i * angle_step) / 2)  # Divide each coordinate by 2
        for i in range(n_vertices)
    ]
    
    return coordinates

# Generate and print the coordinates
decagon_coordinates = generate_decagon_coordinates()
for i, (x, y) in enumerate(decagon_coordinates, start=1):
    print(f"Vertex {i}: x = {x:.3f}, y = {y:.3f}")