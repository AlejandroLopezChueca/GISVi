#include "OpenGLUtils.h"

#include <glad/glad.h>

int GV::OpenGLUtils::getMaxNumberClipDistance()
{
  int countClipDistance = 0;
  glGetIntegerv(GL_MAX_CLIP_DISTANCES, &countClipDistance);
  return countClipDistance;
}

void GV::OpenGLUtils::enableClipDistance(int idx)
{
  glEnable(GL_CLIP_DISTANCE0 + idx);
}


