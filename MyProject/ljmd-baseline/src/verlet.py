def calculate_velocity(mass, dt, fx_prev, fx_curr):
    # Calculate the velocity using the Verlet algorithm
    velocity = (fx_curr - fx_prev) * (dt / (2 * mass))
    return velocity

# Example usage
mass = 3.0  # Mass of the particle
dt = 0.1  # Time step
fx_prev = 1.0  # Force at previous time step
fx_curr = 2.0  # Force at current time step

velocity = calculate_velocity(mass, dt, fx_prev, fx_curr)
print("Velocity:", velocity)