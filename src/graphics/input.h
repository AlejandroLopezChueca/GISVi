#pragma once

#include <glm/glm.hpp>

#include "keyCodes.h"
#include "mouseCodes.h"

namespace GV
{
  namespace Input
  {
    bool isKeyPressed(GV::Key::keyCode key);
    bool isMouseButtonPressed(GV::Mouse::mouseCode button);
    glm::vec2 getMousePosition();
    float getMouseX();
    float getMouseY();


  }

}
