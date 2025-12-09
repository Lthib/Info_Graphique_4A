#include "./../../include/dynamics/EulerExplicitSolver.hpp"

EulerExplicitSolver::EulerExplicitSolver()
{

}

EulerExplicitSolver::~EulerExplicitSolver()
{

}

void EulerExplicitSolver::do_solve(const float& dt, std::vector<ParticlePtr>& particles)
{
    for(ParticlePtr p : particles)
    {
        if(!p->isFixed())
        {
            //TODO: Implement explicit euler solver
            //Functions to use:
            //Particle::getPosition(), Particle::getVelocity(), Particle::getMass(), Particle::getForce()
            //Particle::setPosition(), Particle::setVelocity()
            glm::vec3 pos = p->getPosition();
            glm::vec3 vel = p->getVelocity();
            glm::vec3 force = p->getForce();
            float masse = p->getMass();

            p->setVelocity(vel+(1/masse)*dt*force);
            p->setPosition(pos+dt*(vel+(1/masse)*dt*force));
        }
    }
}
