#ifndef KEYFRAMECOLLECTIONFLOAT_HPP
#define KEYFRAMECOLLECTIONFLOAT_HPP

#include <map>
#include <array>

class KeyframeCollectionFloat
{
public:
    typedef std::pair<float, float> Keyframe; //la pair entre le temps et la valeur de ma focale

    KeyframeCollectionFloat() = default;

    void add(float value, float time); // Ajouter une clé : valeur + temps

    float interpolate(float time) const;//ce qu'on va utiliser dans do_animate de la caméra.

    bool empty() const;

private:
    std::map<float, float> m_keyframes; //toutes les keyframes dont la clé sont leur temps.

    // Récupère deux keyframes limites supérieures et inférieures à un temps donné pour le calcul
    std::array<Keyframe, 2> getBoundingKeyframes(float time) const;
};

#endif