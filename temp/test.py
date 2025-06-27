import os
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Polygon
from PIL import Image

def cubic_bezier(p0, p1, p2, p3, t):
    return (1 - t)**3 * np.array(p0) + 3 * (1 - t)**2 * t * np.array(p1) + \
           3 * (1 - t) * t**2 * np.array(p2) + t**3 * np.array(p3)

# Create output directory
output_dir = "bezier_frames"
os.makedirs(output_dir, exist_ok=True)

# Generate frames
a_values = np.linspace(0, 1, 11)
t_vals = np.linspace(0, 1, 100)

for i, a in enumerate(a_values):
    p0 = [0, 0]
    p1 = [a, 1 - a]
    p2 = [a, 1 - a]
    p3 = [1, 1]
    
    bezier_points = np.array([cubic_bezier(p0, p1, p2, p3, t) for t in t_vals])
    x_line = np.linspace(0, 1, 100)

    fig, ax = plt.subplots(figsize=(5, 5))
    ax.plot(bezier_points[:, 0], bezier_points[:, 1], color='blue')
    ax.plot(x_line, -x_line + 1, color='orange', linestyle='--')
    ax.plot(*zip(p0, p1, p2, p3), 'o', color='gray')

    control_points = [p0, p1, p2, p3]
    for j in range(3):
        ax.plot(
            [control_points[j][0], control_points[j + 1][0]],
            [control_points[j][1], control_points[j + 1][1]],
            linestyle='--', color='gray', linewidth=1
        )

    ax.set_xticks([0, 1])
    ax.set_yticks([0, 1])
    ax.set_xlim(0, 1)
    ax.set_ylim(0, 1)
    ax.set_aspect('equal')
    ax.set_xlabel("Time (t)")
    ax.set_ylabel("Progression")
    ax.set_title("Easing Function with Bézier Curve")
    ax.spines['top'].set_visible(False)
    ax.spines['right'].set_visible(False)
    plt.grid(True, linestyle=':', linewidth=0.5)
    plt.tight_layout()
    
    frame_path = os.path.join(output_dir, f"frame_{i:02d}.png")
    plt.savefig(frame_path)
    plt.close()

# Optional: Create a GIF
# Optional: Create a slower GIF that loops forth and back
make_gif = True
if make_gif:
    frame_paths = [os.path.join(output_dir, f"frame_{i:02d}.png") for i in range(len(a_values))]
    
    # Load and double frames to animate forward and back
    images = [Image.open(fp) for fp in frame_paths]
    images_back = list(reversed(images[1:-1]))  # avoid repeating first and last
    full_sequence = images + images_back

    # Save the GIF
    full_sequence[0].save(
        "bezier_animation.gif",
        save_all=True,
        append_images=full_sequence[1:],
        duration=500,  # 500ms per frame = slower
        loop=0
    )
    print("GIF saved as 'bezier_animation.gif'")

