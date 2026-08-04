#include <iostream>
#include <vector>

#include "CGL/vector2D.h"

#include "mass.h"
#include "rope.h"
#include "spring.h"

#define DAMPING_FACTOR 0.00005
#define EULER_DAMPING 0.01

namespace CGL {

    Rope::Rope(Vector2D start, Vector2D end, int num_nodes, float node_mass, float k, vector<int> pinned_nodes)
    {
        // TODO (Part 1): Create a rope starting at `start`, ending at `end`, and containing `num_nodes` nodes.
        Vector2D length = (end - start) / (num_nodes - 1);

        for (int i = 0; i < num_nodes; i++) {
            Vector2D pos = start + i * length;
            this->masses.push_back(new Mass(pos, node_mass, false));
            if (i > 0) {
                this->springs.push_back(new Spring(this->masses[i-1], this->masses[i], k));
            }
        }
         for (auto &i : pinned_nodes) {
            masses[i]->pinned = true;
        }
    }

    void Rope::simulateEuler(float delta_t, Vector2D gravity)
    {
        for (auto &s : springs)
        {
            // TODO (Part 2): Use Hooke's law to calculate the force on a node
            Vector2D dir = (s->m1->position - s->m2->position).unit();
            float length = (s->m1->position - s->m2->position).norm();
            Vector2D force = -s->k * (length - s->rest_length) * dir;
            s->m1->forces += force;
            s->m2->forces -= force;
        }

        for (auto &m : masses)
        {
            if (!m->pinned)
            {
                // TODO (Part 2): Add the force due to gravity, then compute the new velocity and position
                m->forces += m->mass * gravity;
                m->velocity += m->forces / m->mass * delta_t;
                // TODO (Part 2): Add global damping
                m->velocity *= (1.0f - EULER_DAMPING * delta_t);
                m->position += m->velocity * delta_t;
            }

            // Reset all forces on each mass
            m->forces = Vector2D(0, 0);
        }
    }

    void Rope::simulateVerlet(float delta_t, Vector2D gravity)
    {
        for (auto &m : masses)
        {
            if (!m->pinned)
            {
                Vector2D temp_position = m->position;
                // TODO (Part 3.1): Set the new position of the rope mass
                m->position = m->position + (1 - DAMPING_FACTOR) * (m->position - m->last_position) + gravity * delta_t * delta_t;
                m->last_position = temp_position;
                // TODO (Part 4): Add global Verlet damping
            }
        }

        for (auto &s : springs)
        {
            // TODO (Part 3): Simulate one timestep of the rope using explicit Verlet （solving constraints)
            Vector2D dir = (s->m1->position - s->m2->position).unit();
            float length = (s->m1->position - s->m2->position).norm();
            float shift = length - s->rest_length;
            if (!s->m1->pinned) {
                s->m1->position -= shift / 2 * dir;
            }
            if (!s->m2->pinned) {
                s->m2->position += shift / 2 * dir;
            }
        }
    }
}
