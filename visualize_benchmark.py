"""
Visualization script for Active Particles 3D benchmark results.
Generates interactive comparison plots for Single CPU vs OpenMP vs GPU Metal performance.

Usage:
    uv run visualize_benchmark.py
"""

import pandas as pd
import plotly.graph_objects as go
from plotly.subplots import make_subplots
import sys
import logging

# Configure logging
logging.basicConfig(level=logging.INFO, format="%(levelname)s: %(message)s")


def load_benchmark_data(filename="benchmark_results.csv"):
    """Load benchmark results from CSV file."""
    df = pd.read_csv(filename)
    return df


def create_plots(df):
    """Generate comprehensive interactive benchmark visualization using Plotly."""

    # Color scheme
    colors = {
        "single": "#e74c3c",  # Red
        "openmp": "#3498db",  # Blue
        "gpu": "#2ecc71",  # Green
        "speedup": "#9b59b6",  # Purple
    }

    particles = df["Particles"]

    # Create subplots with 3 columns
    fig = make_subplots(
        rows=1,
        cols=3,
        subplot_titles=(
            "Execution Time vs Particle Count",
            "Speedup: Small Scale (≤500 particles)",
            "Speedup: Large Scale (≥1000 particles)",
        ),
        specs=[[{"type": "scatter"}, {"type": "bar"}, {"type": "bar"}]],
        horizontal_spacing=0.08,
        column_widths=[0.4, 0.3, 0.3],
    )

    # ============================================
    # Plot 1: Execution Time Comparison (Log-Log)
    # ============================================
    fig.add_trace(
        go.Scatter(
            x=particles,
            y=df["Single_CPU_Time"],
            mode="lines+markers",
            name="Single CPU",
            line=dict(color=colors["single"], width=4),
            marker=dict(size=12, symbol="circle"),
            hovertemplate="<b>Single CPU</b><br>Particles: %{x}<br>Time: %{y:.3f}s<extra></extra>",
        ),
        row=1,
        col=1,
    )
    fig.add_trace(
        go.Scatter(
            x=particles,
            y=df["OpenMP_6_Time"],
            mode="lines+markers",
            name="OpenMP (6 threads)",
            line=dict(color=colors["openmp"], width=4),
            marker=dict(size=12, symbol="square"),
            hovertemplate="<b>OpenMP (6 threads)</b><br>Particles: %{x}<br>Time: %{y:.3f}s<extra></extra>",
        ),
        row=1,
        col=1,
    )
    fig.add_trace(
        go.Scatter(
            x=particles,
            y=df["GPU_Time"],
            mode="lines+markers",
            name="GPU Metal",
            line=dict(color=colors["gpu"], width=4),
            marker=dict(size=12, symbol="triangle-up"),
            hovertemplate="<b>GPU Metal</b><br>Particles: %{x}<br>Time: %{y:.3f}s<extra></extra>",
        ),
        row=1,
        col=1,
    )

    # Add GPU threshold annotation on plot 1
    fig.add_vline(
        x=500,
        line_dash="dash",
        line_color="orange",
        line_width=3,
        annotation_text="GPU Threshold (500 particles)",
        annotation_position="top",
        annotation_font_color="orange",
        annotation_font_size=11,
        row=1,
        col=1,
    )

    # ============================================
    # Plot 2: Speedup Comparison - Small Scale (≤500 particles)
    # ============================================
    df_small = df[df["Particles"] <= 500]
    particles_small = df_small["Particles"]

    fig.add_trace(
        go.Bar(
            x=particles_small,
            y=df_small["OpenMP_vs_Single"],
            name="OpenMP vs Single CPU",
            marker_color=colors["openmp"],
            text=[f"{x:.2f}×" for x in df_small["OpenMP_vs_Single"]],
            textposition="outside",
            textfont=dict(size=10, color=colors["openmp"]),
            hovertemplate="<b>OpenMP vs Single</b><br>Particles: %{x}<br>Speedup: %{y:.2f}×<extra></extra>",
            showlegend=True,
        ),
        row=1,
        col=2,
    )
    fig.add_trace(
        go.Bar(
            x=particles_small,
            y=df_small["GPU_vs_OpenMP"],
            name="GPU vs OpenMP",
            marker_color=colors["speedup"],
            text=[f"{x:.2f}×" for x in df_small["GPU_vs_OpenMP"]],
            textposition="outside",
            textfont=dict(size=10, color=colors["speedup"]),
            hovertemplate="<b>GPU vs OpenMP</b><br>Particles: %{x}<br>Speedup: %{y:.2f}×<extra></extra>",
            showlegend=True,
        ),
        row=1,
        col=2,
    )
    fig.add_trace(
        go.Bar(
            x=particles_small,
            y=df_small["GPU_vs_Single"],
            name="GPU vs Single CPU",
            marker_color=colors["gpu"],
            text=[f"{x:.2f}×" for x in df_small["GPU_vs_Single"]],
            textposition="outside",
            textfont=dict(size=10, color=colors["gpu"]),
            hovertemplate="<b>GPU vs Single</b><br>Particles: %{x}<br>Speedup: %{y:.2f}×<extra></extra>",
            showlegend=True,
        ),
        row=1,
        col=2,
    )

    # ============================================
    # Plot 3: Speedup Comparison - Large Scale (≥1000 particles)
    # ============================================
    df_large = df[df["Particles"] >= 1000]
    particles_large = df_large["Particles"]

    fig.add_trace(
        go.Bar(
            x=particles_large,
            y=df_large["OpenMP_vs_Single"],
            name="OpenMP vs Single CPU",
            marker_color=colors["openmp"],
            text=[f"{x:.2f}×" for x in df_large["OpenMP_vs_Single"]],
            textposition="outside",
            textfont=dict(size=10, color=colors["openmp"]),
            hovertemplate="<b>OpenMP vs Single</b><br>Particles: %{x}<br>Speedup: %{y:.2f}×<extra></extra>",
            showlegend=False,
        ),
        row=1,
        col=3,
    )
    fig.add_trace(
        go.Bar(
            x=particles_large,
            y=df_large["GPU_vs_OpenMP"],
            name="GPU vs OpenMP",
            marker_color=colors["speedup"],
            text=[f"{x:.2f}×" for x in df_large["GPU_vs_OpenMP"]],
            textposition="outside",
            textfont=dict(size=10, color=colors["speedup"]),
            hovertemplate="<b>GPU vs OpenMP</b><br>Particles: %{x}<br>Speedup: %{y:.2f}×<extra></extra>",
            showlegend=False,
        ),
        row=1,
        col=3,
    )
    fig.add_trace(
        go.Bar(
            x=particles_large,
            y=df_large["GPU_vs_Single"],
            name="GPU vs Single CPU",
            marker_color=colors["gpu"],
            text=[f"{x:.2f}×" for x in df_large["GPU_vs_Single"]],
            textposition="outside",
            textfont=dict(size=10, color=colors["gpu"]),
            hovertemplate="<b>GPU vs Single</b><br>Particles: %{x}<br>Speedup: %{y:.2f}×<extra></extra>",
            showlegend=False,
        ),
        row=1,
        col=3,
    )

    # Add baseline reference lines at 1x speedup
    fig.add_hline(
        y=1,
        line_dash="dash",
        line_color="gray",
        line_width=2,
        annotation_text="1× (no speedup)",
        annotation_position="right",
        annotation_font_size=9,
        row=1,
        col=2,
    )
    fig.add_hline(
        y=1,
        line_dash="dash",
        line_color="gray",
        line_width=2,
        annotation_text="1× (no speedup)",
        annotation_position="right",
        annotation_font_size=9,
        row=1,
        col=3,
    )

    # Update axes
    fig.update_xaxes(
        title_text="Number of Particles",
        gridcolor="lightgray",
        showgrid=True,
        row=1,
        col=1,
    )
    fig.update_yaxes(
        title_text="Execution Time (seconds)",
        type="log",
        gridcolor="lightgray",
        showgrid=True,
        row=1,
        col=1,
    )

    fig.update_xaxes(
        title_text="Number of Particles",
        gridcolor="lightgray",
        showgrid=True,
        row=1,
        col=2,
    )
    fig.update_yaxes(
        title_text="Speedup Factor", gridcolor="lightgray", showgrid=True, row=1, col=2
    )

    fig.update_xaxes(
        title_text="Number of Particles",
        gridcolor="lightgray",
        showgrid=True,
        row=1,
        col=3,
    )
    fig.update_yaxes(
        title_text="Speedup Factor", gridcolor="lightgray", showgrid=True, row=1, col=3
    )

    # Overall layout
    fig.update_layout(
        title={
            "text": "Active Particles 3D: Performance Benchmark<br><sub>Single CPU vs OpenMP (6 threads) vs GPU Metal</sub>",
            "x": 0.5,
            "xanchor": "center",
            "font": {"size": 20},
        },
        height=550,
        width=2000,
        showlegend=True,
        legend=dict(
            x=0.5, y=-0.12, orientation="h", xanchor="center", font=dict(size=12)
        ),
        template="plotly_white",
        barmode="group",
        font=dict(size=11),
        hovermode="closest",
    )

    # Save as HTML (interactive)
    output_html = "benchmark_plots.html"
    fig.write_html(output_html)
    logging.info(f"Interactive plots saved to {output_html}")

    # Save as PNG
    output_png = "benchmark_plots.png"
    fig.write_image(output_png, width=2000, height=550, scale=2)
    logging.info(f"Static image saved to {output_png}")

    return fig


def print_summary(df):
    """Print summary statistics."""
    logging.info("=" * 60)
    logging.info("BENCHMARK SUMMARY")
    logging.info("=" * 60)

    max_particles = df["Particles"].max()
    max_idx = df["Particles"].idxmax()

    logging.info(f"At maximum particle count ({max_particles}):")
    logging.info(f"  Single CPU:          {df['Single_CPU_Time'].iloc[max_idx]:.3f}s")
    logging.info(f"  OpenMP (6 threads):  {df['OpenMP_6_Time'].iloc[max_idx]:.3f}s")
    logging.info(f"  GPU Metal:           {df['GPU_Time'].iloc[max_idx]:.3f}s")
    logging.info("")
    logging.info("Speedup factors:")
    logging.info(f"  OpenMP vs Single:    {df['OpenMP_vs_Single'].iloc[max_idx]:.2f}×")
    logging.info(f"  GPU vs OpenMP:       {df['GPU_vs_OpenMP'].iloc[max_idx]:.2f}×")
    logging.info(f"  GPU vs Single:       {df['GPU_vs_Single'].iloc[max_idx]:.2f}×")

    # Find optimal configuration for different particle counts
    logging.info("")
    logging.info("Recommendations:")
    for _, row in df.iterrows():
        n = int(row["Particles"])
        if (
            row["GPU_Time"] < row["OpenMP_6_Time"]
            and row["GPU_Time"] < row["Single_CPU_Time"]
        ):
            config = "GPU Metal"
        elif row["OpenMP_6_Time"] < row["Single_CPU_Time"]:
            config = "OpenMP (6 threads)"
        else:
            config = "Single CPU"
        logging.info(f"  {n:5d} particles → {config}")

    logging.info("=" * 60)


def main():
    """Main execution function."""
    logging.info("Loading benchmark data...")
    df = load_benchmark_data()

    logging.info(f"Found {len(df)} test cases")
    logging.info("Generating plots...")

    create_plots(df)
    print_summary(df)
    logging.info("Visualization complete!")
    return 0


if __name__ == "__main__":
    sys.exit(main())
