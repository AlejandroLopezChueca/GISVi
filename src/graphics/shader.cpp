#include <glad/glad.h>
#include <memory>

#include "API.h"
#include "OpenGL/OpenGLShader.h"
#include "shader.h"


std::unique_ptr<GV::Shader> GV::Shader::create(const std::string& filePath, GV::API api)
{
  switch (api) 
  {
    case GV::API::OPENGL:
      return std::make_unique<GV::OpenGLShader>(filePath);
    case GV::API::NONE:
      return nullptr; 
  };
}
std::unique_ptr<GV::Shader> GV::Shader::create(const std::string& vertexSrc, const std::string& fragmentSrc, GV::API api)
{
  switch (api) 
  {
    case GV::API::OPENGL:
      return std::make_unique<GV::OpenGLShader>(vertexSrc, fragmentSrc);
    case GV::API::NONE:
      return nullptr; 
  };
}

