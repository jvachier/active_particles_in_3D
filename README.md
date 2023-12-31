# Active particles confined 3D

The aim of this project is to build simulations describings the motion of active interacting particles under confinement in 3D. These simulations are based on Langevin equations and used the Euler-Mayurama algorithm. Two types of geometries are used: either squared or circular.

Active interactive particles evolve in different geometries, such as circular or squared. The dynamics is given by two Langevins equations, one for the position $\mathbf{\tilde{r}}(\tilde{x},\tilde{y},\tilde{z})$ of the particles and one for its orientation $\mathbf{e}$

$$
\begin{align}
\frac{d}{d\tilde{t}}\mathbf{\tilde{r}} &= \tilde{v_s}\mathbf{e} - \tilde{\nabla}_{\tilde{R}}(\tilde{LP}) + \sqrt{2\tilde{D}_t}\tilde{\mathbf{\xi}_t}\,\\
\frac{d}{d\tilde{t}}\mathbf{e} &= \sqrt{2\tilde{D}_e}\mathbf{e}\times\tilde{\mathbf{\xi}_e}\,
\end{align}
$$

where $\mathbf{e} = (e_x,e_y,e_z)^{T}$ is the orientational unit vector, $\tilde{v_s}$ is the self-propulsion, $\tilde{D_{t}}$ and $\tilde{D_{e}}$ are the translational and rotational diffusivities, respectively. Moreover, $\langle \tilde{\xi_{t_i}}(\tilde{t}')\tilde{\xi_{t_j}}(\tilde{t}) \rangle = \delta_{ij}\delta(\tilde{t}'-\tilde{t})$ and $\langle \tilde{\xi_{e_i}}(\tilde{t}')\tilde{\xi_{e_j}}(\tilde{t}) \rangle = \delta_{ij}\delta(\tilde{t}'-\tilde{t})$ are two Gaussian white noises. Moreover, the interactions between the particles is represented by using the Lennard-Jones  potential

$$
\tilde{LP} = 4\tilde{\epsilon}[(\frac{\tilde{\sigma}}{\tilde{R}})^{12} - (\frac{\tilde{\sigma}}{\tilde{R}})^{6}]\,
$$

where $\tilde{\epsilon}$ is the depth of the potential well, $\tilde{R}$ is the distance between two interacting particles. In this project, only the repulsive part of the potential is considered.

# Visualizations

## Video
https://github.com/jvachier/active_particles_in_3D/assets/89128100/e0ea3d4e-58a5-4565-8e57-4705057479df



## Seventeen different particles
![plot](./src/figures/particles.png)

## Particles trajectories
![plot](./src/figures/particles_time.png)

