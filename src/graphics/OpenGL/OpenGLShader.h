#pragma once

#include "graphics/shader.h"
#include <glad/glad.h>
#include <cstdint>
#include <unordered_map>

namespace GV
{
  class OpenGLShader: public Shader
  {
    public:
      OpenGLShader(const std::string& filePath);
      /*
       * @brief Create a shader object in OpenGL
       *
       * @param[in] vertexSrc    Source code for vertex shader
       * @param[in] fragmentSrc  Source code for fragment shader
       */
      OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
      ~OpenGLShader();


      void bind() const override;
      void unbind() const override;
      
      void setInt(const std::string& name, int value) override;
      void setMat4(const std::string& name, const glm::mat4& matrix) const override;

    private:
      std::string readFile(const std::string& filePath);
      std::unordered_map<GLenum, std::string> preProcess(const std::string& source);

      void compile(const std::unordered_map<GLenum, std::string>& shaderSources);

      uint32_t m_RendererID;
      std::string m_FilePath;

  };
}
