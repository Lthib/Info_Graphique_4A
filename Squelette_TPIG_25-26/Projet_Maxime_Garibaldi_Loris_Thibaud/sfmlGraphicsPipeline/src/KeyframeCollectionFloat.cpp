//Authors : Maxime Garibaldi/Loris Thibaud

#include "../include/KeyframeCollectionFloat.hpp"
#include <glm/glm.hpp>
#include <cmath>

void KeyframeCollectionFloat::add(float value, float time)
{
    m_keyframes.insert(std::make_pair(time, value));//rajouter une pair temps/fov
}

bool KeyframeCollectionFloat::empty() const
{
    return m_keyframes.empty();
}

std::array<KeyframeCollectionFloat::Keyframe, 2>
KeyframeCollectionFloat::getBoundingKeyframes(float time) const
{
    std::array<Keyframe, 2> result{
        std::make_pair(0.0f, 0.0f),
        std::make_pair(0.0f, 0.0f)
    };//On initialise notre array avec des Keyframe.

    std::map<float, float>::const_iterator upper = m_keyframes.upper_bound(time);//on récupère la première keyframe donc le temps est sup à time.
    std::map<float, float>::const_iterator lower = std::prev(upper);//on récupère la keyframe juste avant ou à time => borne inférieure
    std::map<float, float>::const_iterator end = m_keyframes.end();

    if (upper != end && lower != end)//on vérifie que tout est valide.
    {
        result[0] = *lower;
        result[1] = *upper;
    }

    return result;
}

float KeyframeCollectionFloat::interpolate(float time) const
{
    if (!m_keyframes.empty())
    {
        std::map<float, float>::const_iterator itFirst = m_keyframes.begin();
        std::map<float, float>::const_reverse_iterator itLast = m_keyframes.rbegin(); 

        float effective_time = std::fmod(time, itLast->first);//modulation pour "boucler" entre itLast et itFirst

        std::array<Keyframe, 2> k = getBoundingKeyframes(effective_time);//les keyframes entre effective_time

        float t0 = k[0].first;
        float t1 = k[1].first;
        float v0 = k[0].second;
        float v1 = k[1].second;

        if (t0 == t1) return v0; // eviter la division par 0

        float factor = (effective_time - t0) / (t1 - t0);

        return glm::mix(v0, v1, factor); //la valeur interpolée selon le temps effectif
    }
    else
    {
        return 0.0f;//pas de keyframe
    }
}