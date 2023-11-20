#pragma once

#include <memory>
#include <vector>

//#include "rendererAPI.h"
#include "API.h"
#include "buffer.h"

namespace GV 
{
  class VertexArray
  {

  public:
    virtual ~VertexArray() = default;

    virtual void bind() const = 0;
    virtual void unbind() const = 0;

    virtual void addVertexBuffer(const GV::VertexBuffer* vertexBuffer) = 0;
    virtual void setIndexBuffer(const GV::IndexBuffer* indexBuffer) = 0;

    virtual const std::vector<const GV::VertexBuffer*>& getVertexBuffers() const = 0; 
      
    virtual void updateMemberBufferData(const uint32_t idxBuffer, const uint32_t offset, const uint32_t size, const void* data) = 0;

    static std::unique_ptr<VertexArray> create(GV::API api);
  };


}
