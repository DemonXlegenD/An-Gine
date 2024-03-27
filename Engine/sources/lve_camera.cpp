#include "lve_camera.h"

//std
#include <cassert>
#include <limits>

namespace lve {
    void LveCamera::SetOrthographicProjection(
        float _left, float _right, float _top, float _bottom, float _near, float _far) {
        projectionMatrix = glm::mat4{ 1.0f };
        projectionMatrix[0][0] = 2.f / (_right - _left);
        projectionMatrix[1][1] = 2.f / (_bottom - _top);
        projectionMatrix[2][2] = 1.f / (_far - _near);
        projectionMatrix[3][0] = -(_right + _left) / (_right - _left);
        projectionMatrix[3][1] = -(_bottom + _top) / (_bottom - _top);
        projectionMatrix[3][2] = -_near / (_far - _near);
    }

    void LveCamera::SetPerspectiveProjection(float _fovy, float _aspect, float _near, float _far) {
        assert(glm::abs(_aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
        const float tanHalfFovy = tan(_fovy / 2.f);
        projectionMatrix = glm::mat4{ 0.0f };
        projectionMatrix[0][0] = 1.f / (_aspect * tanHalfFovy);
        projectionMatrix[1][1] = 1.f / (tanHalfFovy);
        projectionMatrix[2][2] = _far / (_far - _near);
        projectionMatrix[2][3] = 1.f;
        projectionMatrix[3][2] = -(_far * _near) / (_far - _near);
    }

    void LveCamera::SetViewDirection(glm::vec3 _position, glm::vec3 _direction, glm::vec3 _up) {
        const glm::vec3 w{ glm::normalize(_direction) };
        const glm::vec3 u{ glm::normalize(glm::cross(w, _up)) };
        const glm::vec3 v{ glm::cross(w, u) };

        viewMatrix = glm::mat4{ 1.f };
        viewMatrix[0][0] = u.x;
        viewMatrix[1][0] = u.y;
        viewMatrix[2][0] = u.z;
        viewMatrix[0][1] = v.x;
        viewMatrix[1][1] = v.y;
        viewMatrix[2][1] = v.z;
        viewMatrix[0][2] = w.x;
        viewMatrix[1][2] = w.y;
        viewMatrix[2][2] = w.z;
        viewMatrix[3][0] = -glm::dot(u, _position);
        viewMatrix[3][1] = -glm::dot(v, _position);
        viewMatrix[3][2] = -glm::dot(w, _position);
    }

    void LveCamera::SetViewTarget(glm::vec3 _position, glm::vec3 _target, glm::vec3 _up) {
        SetViewDirection(_position, _target - _position, _up);
    }

    void LveCamera::SetViewYXZ(glm::vec3 _position, glm::vec3 _rotation) {
        const float c3 = glm::cos(_rotation.z);
        const float s3 = glm::sin(_rotation.z);
        const float c2 = glm::cos(_rotation.x);
        const float s2 = glm::sin(_rotation.x);
        const float c1 = glm::cos(_rotation.y);
        const float s1 = glm::sin(_rotation.y);
        const glm::vec3 u{ (c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1) };
        const glm::vec3 v{ (c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3) };
        const glm::vec3 w{ (c2 * s1), (-s2), (c1 * c2) };
        viewMatrix = glm::mat4{ 1.f };
        viewMatrix[0][0] = u.x;
        viewMatrix[1][0] = u.y;
        viewMatrix[2][0] = u.z;
        viewMatrix[0][1] = v.x;
        viewMatrix[1][1] = v.y;
        viewMatrix[2][1] = v.z;
        viewMatrix[0][2] = w.x;
        viewMatrix[1][2] = w.y;
        viewMatrix[2][2] = w.z;
        viewMatrix[3][0] = -glm::dot(u, _position);
        viewMatrix[3][1] = -glm::dot(v, _position);
        viewMatrix[3][2] = -glm::dot(w, _position);
    }
}