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

    virtual void addVertexBuffer(GV::VertexBuffer* const vertexBuffer) = 0;
    virtual const std::vector<GV::VertexBuffer*>& getVertexBuffers() const = 0; 

    static std::unique_ptr<VertexArray> create(GV::API api);
  };


}
