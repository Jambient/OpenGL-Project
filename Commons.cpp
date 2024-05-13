#include "Commons.h"

Ray::Ray(const glm::vec3& _orig, const glm::vec3& _dir)
{
    orig = _orig;
    dir = _dir;
    invdir = glm::vec3(1 / dir.x, 1 / dir.y, 1 / dir.z);
}

bool AABBox::intersect(const Ray& r, float& t)
{
    float tmin, tmax;

    glm::vec3 t1 = (bounds[0] - r.orig) * r.invdir;
    glm::vec3 t2 = (bounds[1] - r.orig) * r.invdir;

    tmin = glm::max(glm::min(t1.x, t2.x), glm::max(glm::min(t1.y, t2.y), glm::min(t1.z, t2.z)));
    tmax = glm::min(glm::max(t1.x, t2.x), glm::min(glm::max(t1.y, t2.y), glm::max(t1.z, t2.z)));

    if (tmax < 0 || tmin > tmax)
        return false;

    t = tmin > 0 ? tmin : tmax;

    return true;
}

glm::vec3 RotateVector(const glm::vec3& vector, const glm::vec3& rotation)
{
    glm::quat quatRotation = glm::quat(glm::radians(rotation));
    glm::vec3 rotatedVector = glm::rotate(quatRotation, vector);
    return rotatedVector;
}
