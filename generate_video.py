"""
Video generation script for Active Particles 3D simulation data.
Generates two animated 3D visualizations from simulation output:
1. Standard: All particles colored by z-position
2. Tracking: 10% of particles with unique individual colors

Usage:
    uv run generate_video.py
"""

import numpy as np
import pandas as pd
import plotly.graph_objects as go
import logging
from pathlib import Path

# Configure logging
logging.basicConfig(level=logging.INFO, format="%(levelname)s: %(message)s")

# Configuration
INPUT_FILE = "data/simulation.bin"
PARAMETER_FILE = "parameter.txt"
OUTPUT_STANDARD = "figures/particles_standard"
OUTPUT_TRACKED = "figures/particles_tracked"
TRACK_PERCENTAGE = 0.01  # 1% of particles
FRAME_DURATION = 50  # milliseconds


def read_parameters(parameter_file=PARAMETER_FILE):
    """Read simulation parameters from parameter.txt file."""
    try:
        with open(parameter_file, "r") as f:
            line = f.readline().strip()
            params = line.split("\t")
            return {
                "Wall": float(params[6]),
                "height": float(params[7]),
            }
    except Exception as e:
        logging.warning(f"Error reading parameters: {e}. Using data-based dimensions.")
        return None


def read_binary_simulation(filename):
    """Read simulation data from binary file."""
    # Check if file exists, try .bin or .csv
    input_path = Path(filename)
    if not input_path.exists():
        csv_path = input_path.with_suffix(".csv")
        if csv_path.exists():
            logging.info(f"Binary file not found, using CSV: {csv_path}")
            return read_csv_simulation(str(csv_path))
        else:
            raise FileNotFoundError(f"Neither {input_path} nor {csv_path} found")

    with open(filename, "rb") as f:
        num_particles = np.fromfile(f, dtype=np.int32, count=1)[0]
        num_frames = np.fromfile(f, dtype=np.int32, count=1)[0]

        logging.info(f"Reading binary: {num_particles} particles, {num_frames} frames")

        data = []
        for frame_idx in range(num_frames):
            timestep = np.fromfile(f, dtype=np.int32, count=1)[0]
            x = np.fromfile(f, dtype=np.float64, count=num_particles)
            y = np.fromfile(f, dtype=np.float64, count=num_particles)
            z = np.fromfile(f, dtype=np.float64, count=num_particles)
            ex = np.fromfile(f, dtype=np.float64, count=num_particles)
            ey = np.fromfile(f, dtype=np.float64, count=num_particles)
            ez = np.fromfile(f, dtype=np.float64, count=num_particles)

            for particle_id in range(num_particles):
                data.append(
                    {
                        "Particles": particle_id,
                        "x-position": x[particle_id],
                        "y-position": y[particle_id],
                        "z-position": z[particle_id],
                        "ex-orientation": ex[particle_id],
                        "ey-orientation": ey[particle_id],
                        "ez-orientation": ez[particle_id],
                        "time": timestep,
                    }
                )

        return pd.DataFrame(data)


def read_csv_simulation(filename):
    """Read simulation data from CSV file."""
    logging.info(f"Reading CSV file: {filename}")
    df = pd.read_csv(filename)

    if df["Particles"].dtype == object:
        df["Particles"] = df["Particles"].str.extract(r"(\d+)").astype(int)

    return df


def create_cylinder_surface(Wall, height_min, height_max):
    """Create cylinder surface mesh."""
    theta = np.linspace(0, 2 * np.pi, 50)
    z_cyl = np.linspace(height_min, height_max, 30)
    Theta, Z = np.meshgrid(theta, z_cyl)
    X_cyl = Wall * np.cos(Theta)
    Y_cyl = Wall * np.sin(Theta)
    return X_cyl, Y_cyl, Z


def save_to_mp4(fig, output_file):
    """
    Save Plotly figure as HTML.
    Note: MP4 export not supported for animated Plotly figures.
    """
    html_file = f"{output_file}.html"
    fig.write_html(html_file)
    logging.info(f"Saved: {html_file}")


def create_standard_visualization(df, Wall, height_min, height_max):
    """Create standard visualization with all particles colored by z-position."""
    timesteps = sorted(df["time"].unique())
    num_frames = len(timesteps)
    logging.info(f"Creating standard visualization ({num_frames} frames)...")

    X_cyl, Y_cyl, Z = create_cylinder_surface(Wall, height_min, height_max)

    frames = []
    for idx, timestep in enumerate(timesteps):
        frame_data = df[df["time"] == timestep]

        frames.append(
            go.Frame(
                data=[
                    go.Surface(
                        x=X_cyl,
                        y=Y_cyl,
                        z=Z,
                        colorscale="Greys",
                        showscale=False,
                        opacity=0.1,
                        name="Cylinder",
                    ),
                    go.Scatter3d(
                        x=frame_data["x-position"],
                        y=frame_data["y-position"],
                        z=frame_data["z-position"],
                        mode="markers",
                        marker=dict(
                            size=5,
                            color=frame_data["z-position"],
                            colorscale="Viridis",
                            showscale=True,
                            colorbar=dict(title="Z Position"),
                            line=dict(width=0.5, color="darkblue"),
                        ),
                        name="Particles",
                        hovertemplate="<b>Particle %{customdata}</b><br>X: %{x:.2f}<br>Y: %{y:.2f}<br>Z: %{z:.2f}<br><extra></extra>",
                        customdata=frame_data["Particles"],
                    ),
                ],
                name=f"frame{idx}",
            )
        )

    initial_data = df[df["time"] == timesteps[0]]

    fig = go.Figure(
        data=[
            go.Surface(
                x=X_cyl,
                y=Y_cyl,
                z=Z,
                colorscale="Greys",
                showscale=False,
                opacity=0.1,
            ),
            go.Scatter3d(
                x=initial_data["x-position"],
                y=initial_data["y-position"],
                z=initial_data["z-position"],
                mode="markers",
                marker=dict(
                    size=5,
                    color=initial_data["z-position"],
                    colorscale="Viridis",
                    showscale=True,
                    colorbar=dict(title="Z Position"),
                ),
                customdata=initial_data["Particles"],
            ),
        ],
        frames=frames,
    )

    fig.update_layout(
        title={
            "text": f"Active Brownian Particles - Standard View<br><sub>{len(initial_data)} particles</sub>",
            "x": 0.5,
            "xanchor": "center",
        },
        scene=dict(
            xaxis=dict(title="X", range=[-Wall * 1.1, Wall * 1.1]),
            yaxis=dict(title="Y", range=[-Wall * 1.1, Wall * 1.1]),
            zaxis=dict(title="Z", range=[height_min - 1, height_max + 1]),
            aspectmode="cube",
        ),
        updatemenus=[
            {
                "type": "buttons",
                "buttons": [
                    {
                        "label": "Play",
                        "method": "animate",
                        "args": [
                            None,
                            {
                                "frame": {"duration": FRAME_DURATION, "redraw": True},
                                "fromcurrent": True,
                            },
                        ],
                    },
                    {
                        "label": "Pause",
                        "method": "animate",
                        "args": [
                            [None],
                            {
                                "frame": {"duration": 0, "redraw": False},
                                "mode": "immediate",
                            },
                        ],
                    },
                ],
            }
        ],
        sliders=[
            {
                "active": 0,
                "steps": [
                    {
                        "args": [
                            [f"frame{i}"],
                            {
                                "frame": {"duration": 0, "redraw": True},
                                "mode": "immediate",
                            },
                        ],
                        "label": str(timesteps[i]),
                        "method": "animate",
                    }
                    for i in range(num_frames)
                ],
            }
        ],
        height=800,
    )

    return fig


def create_tracking_visualization(df, Wall, height_min, height_max):
    """Create tracking visualization with individual particle colors."""
    timesteps = sorted(df["time"].unique())
    num_frames = len(timesteps)

    all_particles = df["Particles"].unique()
    num_tracked = max(1, int(len(all_particles) * TRACK_PERCENTAGE))
    tracked_particles = np.random.choice(all_particles, size=num_tracked, replace=False)

    logging.info(
        f"Creating tracking visualization ({num_tracked} particles, {TRACK_PERCENTAGE * 100:.0f}%)..."
    )

    colors = [
        f"hsl({int(360 * i / num_tracked)}, 70%, 50%)" for i in range(num_tracked)
    ]
    particle_colors = dict(zip(tracked_particles, colors))

    X_cyl, Y_cyl, Z = create_cylinder_surface(Wall, height_min, height_max)

    frames = []
    for idx, timestep in enumerate(timesteps):
        frame_data = df[
            (df["time"] == timestep) & (df["Particles"].isin(tracked_particles))
        ]
        particle_color_list = [particle_colors[p] for p in frame_data["Particles"]]

        frames.append(
            go.Frame(
                data=[
                    go.Surface(
                        x=X_cyl,
                        y=Y_cyl,
                        z=Z,
                        colorscale="Greys",
                        showscale=False,
                        opacity=0.1,
                    ),
                    go.Scatter3d(
                        x=frame_data["x-position"],
                        y=frame_data["y-position"],
                        z=frame_data["z-position"],
                        mode="markers",
                        marker=dict(
                            size=8,
                            color=particle_color_list,
                            line=dict(width=1, color="white"),
                        ),
                        hovertemplate="<b>Particle %{customdata}</b><br>X: %{x:.2f}<br>Y: %{y:.2f}<br>Z: %{z:.2f}<br><extra></extra>",
                        customdata=frame_data["Particles"],
                    ),
                ],
                name=f"frame{idx}",
            )
        )

    initial_data = df[
        (df["time"] == timesteps[0]) & (df["Particles"].isin(tracked_particles))
    ]
    initial_colors = [particle_colors[p] for p in initial_data["Particles"]]

    fig = go.Figure(
        data=[
            go.Surface(
                x=X_cyl, y=Y_cyl, z=Z, colorscale="Greys", showscale=False, opacity=0.1
            ),
            go.Scatter3d(
                x=initial_data["x-position"],
                y=initial_data["y-position"],
                z=initial_data["z-position"],
                mode="markers",
                marker=dict(
                    size=8, color=initial_colors, line=dict(width=1, color="white")
                ),
                customdata=initial_data["Particles"],
            ),
        ],
        frames=frames,
    )

    fig.update_layout(
        title={
            "text": f"Particle Tracking - {num_tracked} Particles ({TRACK_PERCENTAGE * 100:.0f}%)",
            "x": 0.5,
            "xanchor": "center",
        },
        scene=dict(
            xaxis=dict(title="X", range=[-Wall * 1.1, Wall * 1.1]),
            yaxis=dict(title="Y", range=[-Wall * 1.1, Wall * 1.1]),
            zaxis=dict(title="Z", range=[height_min - 1, height_max + 1]),
            aspectmode="cube",
        ),
        updatemenus=[
            {
                "type": "buttons",
                "buttons": [
                    {
                        "label": "Play",
                        "method": "animate",
                        "args": [
                            None,
                            {
                                "frame": {"duration": FRAME_DURATION, "redraw": True},
                                "fromcurrent": True,
                            },
                        ],
                    },
                    {
                        "label": "Pause",
                        "method": "animate",
                        "args": [
                            [None],
                            {
                                "frame": {"duration": 0, "redraw": False},
                                "mode": "immediate",
                            },
                        ],
                    },
                ],
            }
        ],
        sliders=[
            {
                "active": 0,
                "steps": [
                    {
                        "args": [
                            [f"frame{i}"],
                            {
                                "frame": {"duration": 0, "redraw": True},
                                "mode": "immediate",
                            },
                        ],
                        "label": str(timesteps[i]),
                        "method": "animate",
                    }
                    for i in range(num_frames)
                ],
            }
        ],
        height=800,
    )

    return fig


def main():
    """Main execution."""
    logging.info("=" * 60)
    logging.info("Active Particles 3D - Video Generation")
    logging.info("=" * 60)

    # Load simulation data
    logging.info(f"Loading data from: {INPUT_FILE}")
    df = read_binary_simulation(INPUT_FILE)

    # Get cylinder dimensions
    params = read_parameters(PARAMETER_FILE)
    if params:
        Wall = params["Wall"]
        height = params["height"]
        height_min = -height
        height_max = height
        logging.info(f"Cylinder from {PARAMETER_FILE}: Wall={Wall}, height={height}")
    else:
        max_r = np.sqrt((df["x-position"] ** 2 + df["y-position"] ** 2).max())
        max_z = df["z-position"].max()
        min_z = df["z-position"].min()
        Wall = max_r * 1.1
        height_min = min_z
        height_max = max_z
        logging.info(
            f"Cylinder from data: Wall={Wall:.2f}, height={height_max - height_min:.2f}"
        )

    # Create output directory
    Path("figures").mkdir(exist_ok=True)

    # Create standard visualization
    logging.info("-" * 60)
    fig_standard = create_standard_visualization(df, Wall, height_min, height_max)
    save_to_mp4(fig_standard, OUTPUT_STANDARD)

    # Create tracking visualization
    logging.info("-" * 60)
    fig_tracked = create_tracking_visualization(df, Wall, height_min, height_max)
    save_to_mp4(fig_tracked, OUTPUT_TRACKED)

    logging.info("=" * 60)
    logging.info("✓ Video generation complete!")
    logging.info(f"  Standard: {OUTPUT_STANDARD}.html")
    logging.info(f"  Tracking: {OUTPUT_TRACKED}.html")
    logging.info("=" * 60)


if __name__ == "__main__":
    main()
