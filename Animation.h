#pragma once
#ifndef _ANIMATION_H
#define _ANIMATION_H
#include "SceneObject.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <functional>

using EasingFunction = std::function<double(double)>;

/**
 * @struct Keyframe
 * @brief A structure for storing data about a single keyframe of an animation.
 */
struct Keyframe
{
	float time = 0.0f;
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
};

/**
 * @enum EasingStyle
 * @brief An enum for choosing the easing style of the animation. This is what affects the interpolation time value.
 */
enum class EasingStyle
{
	Linear,
	Sine,
	Quad,
	Cubic,
	Quart,
	Quint,
	Exponential,
	Circular,
	Back,
	Elastic,
	Bounce,
};

/**
 * @enum EasingDirection
 * @brief An enum for choosing the easing direction of the animation. This is the direction the easing style is applied in.
 */
enum class EasingDirection
{
	In,
	Out,
	InOut
};

class SceneObject;

/**
 * @class Animation
 * @brief A class for managing an animation for a single scene object.
 */
class Animation
{
public:
	/**
	 * @brief Constructs an Animation object.
	 * @param target 
	 * @param keyframes 
	 */
	Animation(SceneObject* target, std::vector<Keyframe> keyframes);

	/**
	 * @brief Updates the animation.
	 * @param deltaTime The frame delta time.
	 */
	void Update(float deltaTime);

	/**
	 * @brief Sets the easing style of the animation.
	 * @param style The new easing style.
	 */
	void SetEasingStyle(EasingStyle style) { m_easingStyle = style; }
	void SetEasingStyle(std::string style) { m_easingStyle = s_stringToEasingStyle[style]; }

	/**
	 * @brief Sets the easing direction of the animation.
	 * @param direction The new easing direction.
	 */
	void SetEasingDirection(EasingDirection direction) { m_easingDirection = direction; }
	void SetEasingDirection(std::string direction) { m_easingDirection = s_stringToEasingDirection[direction]; }

	/**
	 * @brief Sets whether or not the animation should reverse once it reaches the end.
	 * @param shouldReverse True if the animation should reverse at the end, false otherwise.
	 */
	void ShouldReverse(bool shouldReverse) { m_shouldReverse = shouldReverse; }

private:
	static std::unordered_map<std::string, EasingStyle> s_stringToEasingStyle; /* Maps easing style identifiers to their enums */
	static std::unordered_map<std::string, EasingDirection> s_stringToEasingDirection; /* Maps easing direction identifiers to their enums */
	static std::unordered_map<EasingStyle, EasingFunction> s_styleToFunction; /* Maps each easing style to its easing function */

	SceneObject* m_target; /* The target scene object to run the animation on. */
	std::vector<Keyframe> m_keyframes; /* The keyframes of the animation. */
	EasingStyle m_easingStyle = EasingStyle::Linear; /* The current easing style. Default is Linear */
	EasingDirection m_easingDirection = EasingDirection::In; /* The current easing direction. Default is In */
	bool m_shouldReverse = false; /* Whether or not the animation should reverse at the end. Default is false */
	float m_elapsedTime = 0.0f; /* The elapsed time of the animation. */
	float m_maxTime = 0.0f; /* The max time of the keyframes. */
	int m_timeDirection = 1; /* The current direction the animation is playing in. 1 is forwards, -1 is backwards. */
};
#endif //_ANIMATION_H