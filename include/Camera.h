#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Common.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/vec3.hpp"

class Camera
{
public:
    Camera();
    glm::mat4 modelViewMatrix;
    glm::mat4 projectionMatrix;
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 right;
    glm::vec3 up;
    double horizontalAngle;
    double verticalAngle;
    float near = 0.1f;

    void aim(double x, double y);
    void moveForward(double amount);
    void moveBackward(double amount);
    void moveLeft(double amount);
    void moveRight(double amount);
    void moveUpward(double amount);
    void moveDownward(double amount);
    void update();
};

#endif
