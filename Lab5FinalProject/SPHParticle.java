import java.util.ArrayList;
import processing.core.*;

public class SPHParticle {
    // SPH Fluid Prameters
    float timeStep = 0.01f;
    float h = 16.0f;
    float h_square = h * h;
    float stiffness = 0.09f;   // or bulk modulus
    float restDensity = 0.05f; // rho_0
    float particleMass = 4.0f;
    float maxSpeed = 50.0f;
    float wallStiffness = 10.0f;
    float viscosity = 25.0f;

    int particle_color;

    // Set a waiting time so the particles can stay at spawning position for a while
    int lag_count = 800;

    // W_poly6 coefficient in 2D K_poly6 = 4/(PI*h^8)
    float densityCoef = particleMass * 4 / (PApplet.PI * PApplet.pow(h, 8));
    // W_spiky coefficient in 2D K_spicky = -30/(PI*h^5)
    float pressureCoef = particleMass * -30 / (PApplet.PI * PApplet.pow(h, 5));
    // W_viscosity coefficient in 2D K_viscosity = 20/(3*PI*h^5)
    float viscosityCoef = particleMass * 20 / (3 * PApplet.PI * PApplet.pow(h, 5));

    // Particle Variables
    PVector gravity;
    PVector position;
    PVector velocity;
    PVector acceleration;
    float density;
    float pressure;

    SPHParticle(PVector pos, float _visc, float _mass, int _c) {
        gravity = new PVector(0, 9.8f);
        position = pos;
        velocity = new PVector(0, 0, 0);
        acceleration = new PVector(0, 0, 0);
        density = PApplet.EPSILON;
        pressure = PApplet.EPSILON;

        viscosity = _visc;
        particleMass = _mass;
        particle_color = _c;
    }

    void Step(ArrayList<SPHParticle> particles) {
        if (lag_count <= 0) {
            ComputeDensity(particles);
            ComputePressure();
            ComputeForces(particles);
        }

        lag_count--;
    }

    void ComputeDensity(ArrayList<SPHParticle> particles) {

        float _density = PApplet.EPSILON;
        for (int i = 0; i < particles.size(); i++) {
            PVector other = particles.get(i).position;

            PVector diff = PVector.sub(other, position);

            if (diff.mag() <= PApplet.EPSILON)
                continue;

            float diff_square = PVector.dot(diff, diff);

            if (diff_square < h_square) {
                // Calculate the density from the summation of all nearby particels's density
                // via Smoothed Kernel
                _density += densityCoef * PApplet.pow((h_square - diff_square), 3);
            }
        }
        density = _density;
    }

    void ComputePressure() {
        // Calculate the pressure from the summation of all nearby particles's pressure
        // via Smoothed Kernel
        float _pressure = stiffness * PApplet.max(PApplet.pow(density / restDensity, 7) - 1, PApplet.EPSILON);
        pressure = _pressure;
    }

    void ComputeForces(ArrayList<SPHParticle> particles) {

        PVector _pressure = new PVector(0, 0);
        PVector _viscosity = new PVector(0, 0);

        for (int i = 0; i < particles.size(); i++) {
            SPHParticle other = particles.get(i);

            PVector pos_diff = PVector.sub(other.position, position);
            if (pos_diff.mag() <= PApplet.EPSILON)
                continue;

            float pos_diff_square = PVector.dot(pos_diff, pos_diff);

            if (pos_diff_square < h_square) {
                float diff_len = PApplet.sqrt(pos_diff_square);

                // Pressure with Spiky Kernel Implementation
                // mass * (pressure_i - pressure_j) / (2 * density_j) ) * Grad(W_spiky)
                // Grad(W_spiky) = -30 / (PI*h^5) * (h-r)^2
                float avg_pressure = 0.5f * (pressure + other.pressure);
                PVector pressure_per_particle = PVector.mult(pos_diff,
                        pressureCoef * avg_pressure / other.density * PApplet.pow(this.h - diff_len, 2) / diff_len);
                _pressure.add(pressure_per_particle);

                // Viscosity with Viscosity Kernel Implementation
                PVector vel_diff = PVector.sub(other.velocity, velocity);
                PVector viscosity_per_particle = PVector.mult(vel_diff,
                        viscosityCoef / other.density * (this.h - diff_len));
                _viscosity.add(viscosity_per_particle);
            }
        }
        PVector total_force = PVector.add(_pressure, PVector.mult(_viscosity, this.viscosity));
        this.acceleration = total_force.div(this.density);

    }

    void Intergrate(int width, int height, int right_wall) {
        if (lag_count <= 0) {
            int margin = 0;

            // Repel the particles away from the walls
            if (position.y > height - margin)
                velocity.y -= wallStiffness * PApplet.sq(position.y - height + margin);
            else if (position.y < margin)
                velocity.y += wallStiffness * PApplet.sq(position.y);
            if (position.x > right_wall - margin)
                velocity.x -= wallStiffness * PApplet.sq(position.x - right_wall + margin);
            else if (position.x < margin)
                velocity.x += wallStiffness * PApplet.sq(position.x);

            // Limit the particle speed
            velocity.limit(maxSpeed);

            // Intergrate the acc, vel, pos
            acceleration = PVector.add(acceleration, gravity);
            velocity = PVector.add(velocity, PVector.mult(acceleration, timeStep));
            position = PVector.add(position, PVector.mult(velocity, timeStep));

        }
    }

    void Show(PApplet p) {
        // println(velocity.mag());
        // color c = color(map(viscosity, 0, 100, 150, 0), 255, 255);
        p.fill(particle_color);
        p.noStroke();
        p.ellipse(position.x, position.y, 0.3f * h, 0.3f * h);
    }
}