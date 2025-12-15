#include "./../../include/dynamics/SpringForceField.hpp"

SpringForceField::SpringForceField(const ParticlePtr p1, const ParticlePtr p2, float stiffness, float equilibriumLength, float damping) :
    m_p1(p1),
    m_p2(p2),
    m_stiffness(stiffness),
    m_equilibriumLength(equilibriumLength),
    m_damping(damping)
{}

void SpringForceField::do_addForce()
{
    //TODO: Implement a damped spring
    //Functions to use:
    //glm::length( vec3 ): Return the length of a vector
    //glm::normalize( vec3 ): Return the normalization of a vector
    //Particle::getVelocity(), Particle::getForce(), Particle::setForce()
    //Nb:   Compute force ONLY IF the displacement length is above std::numeric_limits<float>::epsilon()
    //      Otherwise the computation is useless

    //Compute displacement vector
    glm::vec3 dep= m_p1->getPosition()-m_p2->getPosition();

    //Compute displacement length
    float pos_norm = length(dep);

    //Compute spring force corresponding to the displacement 
    //If the displacement is measurable by the computer (otherwise no force)
    glm::vec3 forcek=-m_stiffness*(pos_norm-m_equilibriumLength)*(dep/pos_norm);
    glm::vec3 v1 = m_p1->getVelocity()-m_p2->getVelocity();
    glm::vec3 forcekc=-m_damping*(glm::dot(v1,(dep/pos_norm)))*(dep/pos_norm);

    glm::vec3 forcetot=forcek+forcekc;
    m_p1->setForce(m_p1->getForce()+forcetot);
    m_p2->setForce(m_p2->getForce()-forcetot);
}

ParticlePtr SpringForceField::getParticle1() const
{
    return m_p1;
}

ParticlePtr SpringForceField::getParticle2() const
{
    return m_p2;
}
