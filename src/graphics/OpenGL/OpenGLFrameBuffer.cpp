#include <cstdint>
#include <glad/glad.h>
#include <stdexcept>
#include <vector>

#include "OpenGLFrameBuffer.h"
#include "graphics/frameBuffer.h"

GV::OpenGLFrameBuffer::OpenGLFrameBuffer(const GV::FrameBufferSpecifications& specs)
  : m_Specifications(specs)
{
  resize();
}

GV::OpenGLFrameBuffer::~OpenGLFrameBuffer()
{
  glDeleteFramebuffers(1, &m_RendererID);
  glDeleteTextures(1, &m_ColorAttachment);
}

void GV::OpenGLFrameBuffer::resize()
{
  if (m_RendererID)
  {
    glDeleteFramebuffers(1, &m_RendererID);
    glDeleteTextures(1, &m_ColorAttachment);
    glDeleteTextures(1, &m_DepthAttachment);
 	
    m_ColorAttachment = 0;
    m_DepthAttachment = 0;
  } 
  glCreateFramebuffers(1, &m_RendererID);
  glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

  glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachment);
  glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);

  std::vector<uint8_t> da(m_Specifications.width * m_Specifications.height *4, 170);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Specifications.width, m_Specifications.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)da.data());
  //glTexStorage2D(GL_TEXTURE_2D, 1,GL_RGBA8, m_Specifications.width, m_Specifications.height);
  //glBindImageTexture(1, m_ColorAttachment, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);
 
  //depthBuffer
  if (m_Specifications.is3D)
  {
    glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);
    glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);

    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_Specifications.width, m_Specifications.height);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Specifications.width, m_Specifications.height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);
  }

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
      throw std::runtime_error("FrameBuffer is incomplete!");
  }

  //glBindTexture(GL_TEXTURE_2D, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GV::OpenGLFrameBuffer::bind()
{
  glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
  //glViewport(0, 0, m_Specifications.width, m_Specifications.height);
}

void GV::OpenGLFrameBuffer::bindTexture()
{
  //slot 0
  //glBindTextureUnit(1, m_ColorAttachment);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
}

void GV::OpenGLFrameBuffer::unbind()
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
