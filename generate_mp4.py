"""
MP4 video generation script for Active Particles 3D simulation data.
Renders individual frames from simulation data and stitches them into MP4 using ffmpeg.

This script reuses the visualization logic from generate_video.py but renders
static frames instead of interactive HTML animations.

Requirements:
    - plotly
    - kaleido (for static image export)
    - ffmpeg (system install)

Install:
    uv add kaleido
    brew install ffmpeg

Usage:
    uv run generate_mp4.py
"""

import numpy as np
import pandas as pd
import plotly.graph_objects as go
import logging
import subprocess
import shutil
from pathlib import Path

# Configure logging
logging.basicConfig(level=logging.INFO, format="%(levelname)s: %(message)s")

# Configuration
INPUT_FILE = "data/simulation.bin"
PARAMETER_FILE = "parameter.txt"
OUTPUT_DIR = "figures"
TEMP_FRAMES_DIR = "temp_frames"
TRACK_PERCENTAGE = 0.01  # 1% of particles
FPS = 30  # Frames per second
WIDTH = 1920
HEIGHT = 1080


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


def check_dependencies():
    """Check if required dependencies are installed."""
    try:
        import kaleido
    except ImportError:
        logging.error("kaleido not installed. Install with: uv add kaleido")
        return False

    if not shutil.which("ffmpeg"):
        logging.error("ffmpeg not found. Install with: brew install ffmpeg")
        return False

    return True


def render_standard_frame(df, timestep, Wall, height_min, height_max, output_path):
    """Render a single frame for standard visualization."""
    frame_data = df[df["time"] == timestep]
    X_cyl, Y_cyl, Z = create_cylinder_surface(Wall, height_min, height_max)

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
            ),
        ]
    )

    fig.update_layout(
        title=f"Active Brownian Particles - Frame {timestep}",
        scene=dict(
            xaxis=dict(title="X", range=[-Wall * 1.1, Wall * 1.1]),
            yaxis=dict(title="Y", range=[-Wall * 1.1, Wall * 1.1]),
            zaxis=dict(title="Z", range=[height_min - 1, height_max + 1]),
            aspectmode="cube",
        ),
        width=WIDTH,
        height=HEIGHT,
    )

    fig.write_image(output_path)


def render_tracking_frame(
    df,
    timestep,
    tracked_particles,
    particle_colors,
    Wall,
    height_min,
    height_max,
    output_path,
):
    """Render a single frame for tracking visualization."""
    frame_data = df[
        (df["time"] == timestep) & (df["Particles"].isin(tracked_particles))
    ]
    particle_color_list = [particle_colors[p] for p in frame_data["Particles"]]
    X_cyl, Y_cyl, Z = create_cylinder_surface(Wall, height_min, height_max)

    fig = go.Figure(
        data=[
            go.Surface(
                x=X_cyl, y=Y_cyl, z=Z, colorscale="Greys", showscale=False, opacity=0.1
            ),
            go.Scatter3d(
                x=frame_data["x-position"],
                y=frame_data["y-position"],
                z=frame_data["z-position"],
                mode="markers",
                marker=dict(
                    size=8, color=particle_color_list, line=dict(width=1, color="white")
                ),
            ),
        ]
    )

    num_tracked = len(tracked_particles)
    fig.update_layout(
        title=f"Particle Tracking - {num_tracked} Particles - Frame {timestep}",
        scene=dict(
            xaxis=dict(title="X", range=[-Wall * 1.1, Wall * 1.1]),
            yaxis=dict(title="Y", range=[-Wall * 1.1, Wall * 1.1]),
            zaxis=dict(title="Z", range=[height_min - 1, height_max + 1]),
            aspectmode="cube",
        ),
        width=WIDTH,
        height=HEIGHT,
    )

    fig.write_image(output_path)


def generate_standard_mp4(df, Wall, height_min, height_max):
    """Generate standard MP4 video."""
    timesteps = sorted(df["time"].unique())
    num_frames = len(timesteps)
    logging.info(f"Generating standard visualization ({num_frames} frames)...")

    # Create temp directory
    temp_dir = Path(TEMP_FRAMES_DIR) / "standard"
    temp_dir.mkdir(parents=True, exist_ok=True)

    # Render frames
    for idx, timestep in enumerate(timesteps):
        frame_path = temp_dir / f"frame_{idx:04d}.png"
        logging.info(f"  Rendering frame {idx + 1}/{num_frames} (timestep {timestep})")
        render_standard_frame(df, timestep, Wall, height_min, height_max, frame_path)

    # Create MP4 using ffmpeg
    output_mp4 = Path(OUTPUT_DIR) / "particles_standard.mp4"
    logging.info(f"Creating MP4: {output_mp4}")

    cmd = [
        "ffmpeg",
        "-y",  # Overwrite output file
        "-framerate",
        str(FPS),
        "-i",
        str(temp_dir / "frame_%04d.png"),
        "-c:v",
        "libx264",
        "-pix_fmt",
        "yuv420p",
        "-crf",
        "23",  # Quality (lower = better, 18-28 is good range)
        str(output_mp4),
    ]

    subprocess.run(cmd, check=True, capture_output=True)
    logging.info(f"✓ Created: {output_mp4}")

    # Cleanup
    shutil.rmtree(temp_dir)


def generate_tracking_mp4(df, Wall, height_min, height_max):
    """Generate tracking MP4 video."""
    timesteps = sorted(df["time"].unique())
    num_frames = len(timesteps)

    all_particles = df["Particles"].unique()
    num_tracked = max(1, int(len(all_particles) * TRACK_PERCENTAGE))
    tracked_particles = np.random.choice(all_particles, size=num_tracked, replace=False)

    logging.info(
        f"Generating tracking visualization ({num_tracked} particles, {TRACK_PERCENTAGE * 100:.0f}%)..."
    )

    colors = [
        f"hsl({int(360 * i / num_tracked)}, 70%, 50%)" for i in range(num_tracked)
    ]
    particle_colors = dict(zip(tracked_particles, colors))

    # Create temp directory
    temp_dir = Path(TEMP_FRAMES_DIR) / "tracking"
    temp_dir.mkdir(parents=True, exist_ok=True)

    # Render frames
    for idx, timestep in enumerate(timesteps):
        frame_path = temp_dir / f"frame_{idx:04d}.png"
        logging.info(f"  Rendering frame {idx + 1}/{num_frames} (timestep {timestep})")
        render_tracking_frame(
            df,
            timestep,
            tracked_particles,
            particle_colors,
            Wall,
            height_min,
            height_max,
            frame_path,
        )

    # Create MP4 using ffmpeg
    output_mp4 = Path(OUTPUT_DIR) / "particles_tracked.mp4"
    logging.info(f"Creating MP4: {output_mp4}")

    cmd = [
        "ffmpeg",
        "-y",
        "-framerate",
        str(FPS),
        "-i",
        str(temp_dir / "frame_%04d.png"),
        "-c:v",
        "libx264",
        "-pix_fmt",
        "yuv420p",
        "-crf",
        "23",
        str(output_mp4),
    ]

    subprocess.run(cmd, check=True, capture_output=True)
    logging.info(f"✓ Created: {output_mp4}")

    # Cleanup
    shutil.rmtree(temp_dir)


def main():
    """Main execution function."""
    logging.info("Active Particles 3D - MP4 Video Generator")
    logging.info("=" * 60)

    # Check dependencies
    if not check_dependencies():
        return 1

    # Read simulation data
    logging.info("Loading simulation data...")
    df = read_binary_simulation(INPUT_FILE)

    # Read parameters
    params = read_parameters()
    if params:
        Wall = params["Wall"]
        height = params["height"]
    else:
        # Fallback: estimate from data
        Wall = max(df["x-position"].abs().max(), df["y-position"].abs().max()) * 1.1
        height = df["z-position"].max() - df["z-position"].min()

    height_min = df["z-position"].min()
    height_max = df["z-position"].max()

    logging.info(f"Cylinder: radius={Wall:.1f}, height={height:.1f}")
    logging.info(f"Z-range: [{height_min:.1f}, {height_max:.1f}]")
    logging.info("")

    # Generate videos
    try:
        generate_standard_mp4(df, Wall, height_min, height_max)
        logging.info("")
        generate_tracking_mp4(df, Wall, height_min, height_max)
    except subprocess.CalledProcessError as e:
        logging.error(f"ffmpeg failed: {e}")
        logging.error(e.stderr.decode() if e.stderr else "")
        return 1

    logging.info("")
    logging.info("=" * 60)
    logging.info("MP4 generation complete!")
    logging.info(f"Output files in: {OUTPUT_DIR}/")
    logging.info("=" * 60)

    return 0


if __name__ == "__main__":
    exit(main())
