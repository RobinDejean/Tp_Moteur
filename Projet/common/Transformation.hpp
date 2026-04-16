#pragma once

// GLEW
#include <GL/glew.h>

// GLM
#include <glm/glm.hpp>
#include <glm/ext.hpp>

// GLFW
#include <GLFW/glfw3.h>

// USUAL INCLUDES
#include <math.h>

#define M_PI_SAFE float(M_PI - 0.001)
#define M_PI_2_SAFE float(M_PI_2 - 0.001)
#define M_PI_4_SAFE float(M_PI_4 - 0.001)

#define VEC_ZERO glm::vec3(0.f, 0.f, 0.f)
#define VEC_UP glm::vec3(0.f, 1.f, 0.f)
#define VEC_FRONT glm::vec3(0.f, 0.f, 1.f)
#define VEC_RIGHT glm::vec3(1.f, 0.f, 0.f)

class Transformation {
    glm::vec3 translation;
    glm::vec3 scale;
    glm::vec3 euler_angles;

    // HELPERS
    static float clipAnglePI(float _angle) {
        while (_angle < -M_PI)
            _angle += 2. * M_PI;
        while (_angle > M_PI)
            _angle -= 2. * M_PI;
        return _angle;
    }

    static glm::vec3 EulerToEuclidian(const glm::vec2 &_angles) {
        float sinPhi = cosf(_angles.x);
        float x = sinPhi * sinf(_angles.y);
        float y = sinf(_angles.x);
        float z = sinPhi * cosf(_angles.y);

        return glm::vec3(x, y, z);
    }

    static glm::vec2 EuclidianToEuler(const glm::vec3 &xyz) {
        float angles_x = asin(xyz[1] / glm::length(xyz)); // polar angle from +y axis, 0..π

        float angles_y = atan2(xyz[0], xyz[2]); // azimuth around y-axis, 0..2π
        if (angles_y < 0.0f)
            angles_y += 2.0f * M_PI;

        return glm::vec2(angles_x, angles_y);
    }

public:
    Transformation() : translation(0.f), scale(1.f), euler_angles(0.f) {}
    Transformation(glm::vec3 _translation, glm::vec3 _scale, glm::vec3 _euler_angles) : translation(_translation), scale(_scale), euler_angles(_euler_angles) { updateRotation(); }

    // GETTERS
    inline const glm::vec3 getTranslation() const { return translation; }
    inline const glm::vec3 getEulerAngles() const { return euler_angles; }
    inline glm::vec3 getScale() const { return scale; }
    //inline glm::vec3 getFrontVector() const { return Transformation::EulerToEuclidian(euler_angles); }

    // SETTERS
    inline void setTranslation(const glm::vec3 &t) { translation = t; }
    inline void setEulerAngles(const glm::vec3 &r) { euler_angles = r; }
    inline void addEulerAngles(const glm::vec3 &r) { euler_angles += r; }
    inline void setEulerAnglesFromFront(const glm::vec3 &_front) { euler_angles = glm::vec3(Transformation::EuclidianToEuler(_front), 0.f); }
    inline void setScale(glm::vec3 s) { scale = s; }
    inline void setScale(float s) { scale = glm::vec3(s); }
    inline void setScaleX(float sx) { scale.x = sx; }
    inline void setScaleY(float sy) { scale.y = sy; }
    inline void setScaleZ(float sz) { scale.z = sz; }
    inline void setScaleXY(float s) { scale.x = scale.y = s; }
    inline void setScaleXZ(float s) { scale.x = scale.z = s; }
    inline void setScaleYZ(float s) { scale.y = scale.z = s; }

    // UPDATES
    inline void updateRotation() {
        euler_angles = glm::vec3(
            glm::clamp(euler_angles.x, -M_PI_2_SAFE, M_PI_2_SAFE), // Pitch clamp
            Transformation::clipAnglePI(euler_angles.y),           // Yaw clip
            euler_angles.z);
    }

    inline glm::mat4 computeTransformationMatrix() const {
        glm::mat4 translation_matrix = glm::translate(glm::mat4(1.), translation);
        glm::mat4 rotation_matrix = glm::rotate(glm::mat4(1.), euler_angles.x, VEC_RIGHT);
        rotation_matrix = glm::rotate(rotation_matrix, euler_angles.y, VEC_UP);
        rotation_matrix = glm::rotate(rotation_matrix, euler_angles.z, VEC_FRONT);
        glm::mat4 scale_matrix = glm::scale(glm::mat4(1.), scale);
        return translation_matrix * rotation_matrix * scale_matrix;
    }
};

inline glm::vec3 applyTransformation(const glm::vec3 &vec, float w, const glm::mat4 &transfo) {
    glm::vec4 temp = transfo * glm::vec4(vec.x, vec.y, vec.z, w);
    return temp.w == 0. ? glm::vec3(temp.x, temp.y, temp.z) : glm::vec3(temp.x, temp.y, temp.z) / temp.w;
}