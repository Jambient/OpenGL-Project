#pragma once
#include "SceneObject.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <functional>

struct Keyframe
{
	float time = 0.0f;
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};

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

enum class EasingDirection
{
	In,
	Out,
	InOut
};

using EasingFunction = std::function<double(double)>;

class SceneObject;

class Animation
{
public:
	Animation(SceneObject* target, std::vector<Keyframe> keyframes);
	void Update(float deltaTime);

	void SetEasingStyle(EasingStyle style) { m_easingStyle = style; }
	void SetEasingStyle(std::string style) { m_easingStyle = stringToEasingStyle[style]; }
	void SetEasingDirection(EasingDirection direction) { m_easingDirection = direction; }
	void SetEasingDirection(std::string direction) { m_easingDirection = stringToEasingDirection[direction]; }
	void ShouldReverse(bool shouldReverse) { m_shouldReverse = shouldReverse; }

private:
	SceneObject* m_target;
	std::vector<Keyframe> m_keyframes;
	EasingStyle m_easingStyle = EasingStyle::Linear;
	EasingDirection m_easingDirection = EasingDirection::In;
	bool m_shouldReverse = false;
	float m_elapsedTime = 0.0f;
	float m_maxTime = 0.0f;
	int m_timeDirection = 1;

	static std::unordered_map<std::string, EasingStyle> stringToEasingStyle;
	static std::unordered_map<std::string, EasingDirection> stringToEasingDirection;
	static std::unordered_map<EasingStyle, EasingFunction> styleToFunction;
};