#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <glm/gtc/type_ptr.hpp>

#include "OpenGLShader.h"
#include "glm/fwd.hpp"

namespace GV
{
  static GLenum shaderTypeFromString(const std::string& type)
  {
    if (type == "vertex") return GL_VERTEX_SHADER;
    if (type == "fragment" || type == "pixel") return GL_FRAGMENT_SHADER;
    //terminal -> printf("Unknown shader type : %s\n", type.c_str());
    return 0;
  }

  static const char* GLShaderStageToString(GLenum stage)
  {
    switch (stage)
      {
	case GL_VERTEX_SHADER:   return "GL_VERTEX_SHADER";
        case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
      }
      return nullptr;
  }

  static const char* getCacheDirectory()
  {
    return "assets/cache/shader/opengl";
  }

  static void createCacheDirectoryIfNeeded()
  {
    std::string cacheDirectory = GV::getCacheDirectory();
    if (!std::filesystem::exists(cacheDirectory))
    {
      std::filesystem::create_directories(cacheDirectory);
    }
  }
}

/////////////////////////// OpenGLShader /////////////////////////////////

GV::OpenGLShader::OpenGLShader(const std::string& filePath)
  : m_FilePath(filePath)
{
  GV::createCacheDirectoryIfNeeded();
  std::string source = readFile(filePath);
  auto shaderSources = preProcess(source);
  compile(shaderSources);
}

GV::OpenGLShader::OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc)
{
  std::unordered_map<GLenum, std::string> shaderSources;
  shaderSources[GL_VERTEX_SHADER] = vertexSrc;
  shaderSources[GL_FRAGMENT_SHADER] = vertexSrc;
  compile(shaderSources);
}

GV::OpenGLShader::~OpenGLShader()
{
  glDeleteProgram(m_RendererID);
}

void GV::OpenGLShader::bind() const
{
  glUseProgram(m_RendererID);
}

void GV::OpenGLShader::unbind() const
{
  glUseProgram(0);
}

std::string GV::OpenGLShader::readFile(const std::string& filePath)
{
  std::string result;
  // binary because it in not wanted any processing to the file, to prevent 
  // // to be read as a string by C++
  std::ifstream in(filePath, std::ios::in | std::ios::binary);
  if (in)
  {
    // Go to the end (file pointer) of the file to know the size of the file
    in.seekg(0, std::ios::end);
    size_t size = in.tellg();
    if (size != -1)
    {
      result.resize(size);
      in.seekg(0, std::ios::beg);
      in.read(&result[0], size);
      in.close();
    }
    else
    {
      throw std::invalid_argument("Could not open the file " + filePath);
    }
  }
  else 
  {
      throw std::invalid_argument("Could not open the file " + filePath);
  }
  return result;
}

std::unordered_map<GLenum, std::string> GV::OpenGLShader::preProcess(const std::string& source)
{

  std::unordered_map<GLenum, std::string> shaderSources;

  const char* typeToken = "#type";
  size_t typeTokenLength = strlen(typeToken);
  size_t pos = source.find(typeToken, 0); // Start of shader type declaratin line
  while (pos != std::string::npos)
  {
    size_t eol = source.find_first_of("\r\n", pos);
    if (eol == std::string::npos) throw std::invalid_argument("Sintax error in shader file");
    size_t begin = pos +typeTokenLength + 1;
    std::string type = source.substr(begin, eol - begin);
    if (!GV::shaderTypeFromString(type)) throw std::invalid_argument("Invalid shader type specified");

    size_t nextLinePos = source.find_first_not_of("\r\n", eol); // Start of shader code after shader type declaration line
    if (nextLinePos == std::string::npos) throw std::invalid_argument("Sintax error in shader file");
    pos = source.find(typeToken, nextLinePos);

    shaderSources[GV::shaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
  }
  return shaderSources;
}
 
void GV::OpenGLShader::compile(const std::unordered_map<GLenum, std::string>& shaderSources)
{
  // Creating of a program
  GLint program = glCreateProgram();
  if (shaderSources.size() > 3) throw std::invalid_argument("It is only supported 3 shaders");
  std::array<GLuint, 3> glShadersIDs;
  int glShaderIDIndex = 0;
  for (auto& kv : shaderSources)
  {

    GLenum type = kv.first;
    const std::string& source = kv.second;

    // Create an emty shader handle
    GLuint shader = glCreateShader(type);

    // Send the shader source code to GL
    // Note that std::string's .c_str is NULL character terminated
    const GLchar* sourceCstr = source.c_str();
    glShaderSource(shader, 1, &sourceCstr, 0);

    // Complie the Shader
    glCompileShader(shader);

    // Shader error handling. Check if compiling is a succes
    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
      GLint maxLength = 0;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

      // The maxLength includes the NULL character
      std::vector<GLchar> errorLog(maxLength);
      glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

      // Provide the infolog in whatever manor you deem best.
      // Exit with failure.
      //terminal -> printf("Shader compilation failure: %s\n", errorLog.data());
      glDeleteShader(shader); // Don't leak the shader.
      break;
    }
    // Attach shader to our program
    glAttachShader(program, shader);
    glShadersIDs[glShaderIDIndex++] = shader;
  }
  // Link our program
  glLinkProgram(program);

  // Note the different functions here: glGetProgram* instead of glGetShader*.
  GLint isLinked = 0;
  glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
  if (isLinked == GL_FALSE)
  {
    GLint maxLength = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

    // The maxLength includes the NULL character
    std::vector<GLchar> infoLog(maxLength);
    glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
    
    // We don't need the program anymore.
    glDeleteProgram(program);
    // Don't leak shaders either.
    for (auto id : glShadersIDs) glDeleteShader(id);
    // Use the infoLog as you see fit.
    //terminal -> printf("Shader link failure: %s\n", infoLog.data());
    return;
  }
  // Always detach shaders after a successful link.
  for (auto id : glShadersIDs) glDetachShader(program, id);
  // set when everthing is okay
  m_RendererID = program;
}

void GV::OpenGLShader::setInt(const std::string &name, int value)
{
  GLint location = glGetUniformLocation(m_RendererID, name.c_str());
  // value is the slot where the texture is bound
  glUniform1i(location, value);
}

void GV::OpenGLShader::setMat4(const std::string& name, const glm::mat4& matrix) const
{
  GLint location = glGetUniformLocation(m_RendererID, name.c_str());
  // false for column major matrix
  glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));

}

