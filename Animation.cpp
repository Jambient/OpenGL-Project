#include "Animation.h"
#include "Commons.h"

// functions are from https://easings.net/
namespace EasingFunctions
{
    float Linear(float t)
    {
        return t;
    }

    float Sine(float t)
    {
        return 1 - cos((t * M_PI) / 2);
    }

    float Quad(float t)
    {
        return t * t;
    }

    float Cubic(float t)
    {
        return t * t * t;
    }

    float Quart(float t)
    {
        return t * t * t * t;
    }

    float Quint(float t)
    {
        return t * t * t * t * t;
    }

    float Exponential(float t)
    {
        return t == 0.0f ? 0.0f : std::pow(2, 10 * t - 10);
    }

    float Circular(float t)
    {
        return 1 - std::sqrt(1 - std::pow(t, 2));
    }

    float Back(float t)
    {
        const float c1 = 1.70158f;
        const float c3 = c1 + 1.0f;

        return c3 * t * t * t - c1 * t * t;
    }

    float Elastic(float t)
    {
        const float c4 = (2 * M_PI) / 3;
        return t == 0
            ? 0
            : t == 1
            ? 1
            : -std::pow(2, 10 * t - 10) * std::sin((t * 10 - 10.75) * c4);
    }

    float Bounce(float t)
    {
        t = 1.0f - t;
        const float n1 = 7.5625f;
        const float d1 = 2.75f;

        if (t < 1 / d1) {
            t = n1 * t * t;
        }
        else if (t < 2 / d1) {
            t = n1 * (t -= 1.5 / d1) * t + 0.75;
        }
        else if (t < 2.5 / d1) {
            t = n1 * (t -= 2.25 / d1) * t + 0.9375;
        }
        else {
            t = n1 * (t -= 2.625 / d1) * t + 0.984375;
        }

        return 1.0f - t;
    }
}

std::unordered_map<std::string, EasingStyle> Animation::stringToEasingStyle = {
    {"Linear", EasingStyle::Linear},
    {"Sine", EasingStyle::Sine},
    {"Quad", EasingStyle::Quad},
    {"Cubic", EasingStyle::Cubic},
    {"Quart", EasingStyle::Quart},
    {"Quint", EasingStyle::Quint},
    {"Exponential", EasingStyle::Exponential},
    {"Circular", EasingStyle::Circular},
    {"Back", EasingStyle::Back},
    {"Elastic", EasingStyle::Elastic},
    {"Bounce", EasingStyle::Bounce}
};

std::unordered_map<std::string, EasingDirection> Animation::stringToEasingDirection = {
    {"In", EasingDirection::In},
    {"Out", EasingDirection::Out},
    {"InOut", EasingDirection::InOut}
};

std::unordered_map<EasingStyle, EasingFunction> Animation::styleToFunction = {
    {EasingStyle::Linear, EasingFunctions::Linear},
    {EasingStyle::Sine, EasingFunctions::Sine},
    {EasingStyle::Quad, EasingFunctions::Quad},
    {EasingStyle::Cubic, EasingFunctions::Cubic},
    {EasingStyle::Quart, EasingFunctions::Quart},
    {EasingStyle::Quint, EasingFunctions::Quint},
    {EasingStyle::Exponential, EasingFunctions::Exponential},
    {EasingStyle::Circular, EasingFunctions::Circular},
    {EasingStyle::Back, EasingFunctions::Back},
    {EasingStyle::Elastic, EasingFunctions::Elastic},
    {EasingStyle::Bounce, EasingFunctions::Bounce}
};

Animation::Animation(SceneObject* target, std::vector<Keyframe> keyframes)
{
	m_target = target; 
	m_keyframes = keyframes;
    m_maxTime = keyframes.back().time;
}

void Animation::Update(float deltaTime)
{
    // update elapsed time
    m_elapsedTime += deltaTime * m_timeDirection;
    
    // check if time has passed animation bounds
    if (m_elapsedTime < 0 || m_elapsedTime > m_maxTime)
    {
        if (m_shouldReverse)
        {
            m_timeDirection *= -1;
            m_elapsedTime = std::min(std::max(m_elapsedTime, 0.0f), m_maxTime);
        }
        else
        {
            m_elapsedTime = 0.0f;
        }
    }

    // get current keyframe set
    Keyframe currentKeyframe, previousKeyframe;
    for (int i = 1; i < m_keyframes.size(); i++)
    {
        if (m_elapsedTime <= m_keyframes[i].time)
        {
            currentKeyframe = m_keyframes[i];
            previousKeyframe = m_keyframes[i - 1];
            break;
        }
    }

    // calculate t and modify using easing direction
    float t = (m_elapsedTime - previousKeyframe.time) / (currentKeyframe.time - previousKeyframe.time);

    if(m_easingDirection == EasingDirection::In)
    {
        t = styleToFunction[m_easingStyle](t);
    }
    else if (m_easingDirection == EasingDirection::Out)
    {
        t = 1.0f - styleToFunction[m_easingStyle](1.0f - t);
    }
    else if (m_easingDirection == EasingDirection::InOut)
    {
        if (t <= 0.5f)
            t = styleToFunction[m_easingStyle](t * 2) * 0.5f;
        else
            t = ( (1.0f - styleToFunction[m_easingStyle](1.0f - (t - 0.5f) * 2)) * 0.5f) + 0.5f;
    }

    // set new values for object
    m_target->SetPosition(glm::mix(previousKeyframe.position, currentKeyframe.position, t));
    m_target->SetRotation(glm::mix(previousKeyframe.rotation, currentKeyframe.rotation, t));
    m_target->SetScale(glm::mix(previousKeyframe.scale, currentKeyframe.scale, t));
}
