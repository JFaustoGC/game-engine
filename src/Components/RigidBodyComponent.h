//
// Created by fausto on 7/01/25.
//

#ifndef RIGIDBODYCOMPONENT_H
#define RIGIDBODYCOMPONENT_H

#include <glm/glm.hpp>



struct RigidBodyComponent {
    glm::vec2 velocity;

    RigidBodyComponent(glm::vec2 velocity = glm::vec2(0.0f, 0.0f))
    {
        this->velocity = velocity;
    }
};

#endif //RIGIDBODYCOMPONENT_H
