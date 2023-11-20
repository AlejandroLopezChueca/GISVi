#pragma once

#include <cstdint>
#include <memory>
#include <sys/stat.h>
#include <vector>

#include "API.h"
//#include "rendererAPI.h"

namespace GV
{
  enum class BufferTypeDraw
  {
    Static = 0, Dynamic, Stream
  };


  enum class ShaderDataType
  {
    None = 0, Float, Float2, Float3, Mat4
  };

  struct BufferElement
  {
    std::string name;
    ShaderDataType type;
    uint32_t size;
    uint32_t offset;
    bool normalized;
    bool instanced;

    BufferElement(ShaderDataType type_ ,const std::string name_, bool instanced_ = false, bool normalized_ = false);

    uint32_t getComponentCount() const;
  };

  class BufferLayout
  {
    public:

      BufferLayout() = default;

      BufferLayout(const std::initializer_list<BufferElement>& elements_);
      inline uint32_t getStride() const {return stride;}

      const std::vector<BufferElement>& getElements() const {return elements;}

      std::vector<BufferElement>::iterator begin() {return elements.begin();}
      std::vector<BufferElement>::iterator end() {return elements.end();}
      std::vector<BufferElement>::const_iterator begin() const {return elements.begin();}
      std::vector<BufferElement>::const_iterator end() const {return elements.end();}

    private:
      void calculateOffsetsAndStride();

      std::vector<BufferElement> elements;
      uint32_t stride = 0;
  };

  class VertexBuffer
  {
    public:
      virtual ~VertexBuffer() = default;

      virtual void bind() const = 0;
      virtual void unbind() const = 0;

      virtual void updateDynamicData() = 0;
      virtual const BufferLayout& getLayout() const = 0;
      virtual void setLayout(const BufferLayout& layout) = 0;
      virtual unsigned int getVertexBufferID() const = 0;

      static std::unique_ptr<VertexBuffer> create(GV::API api, void* vertices, int size, BufferTypeDraw bufferTypeDraw);
  };

  class IndexBuffer
  {
    public:
      virtual ~IndexBuffer() = default;
      virtual void bind() const = 0;
      virtual void unbind() const = 0;

      static std::unique_ptr<IndexBuffer> create(GV::API api, uint32_t* indices, uint32_t count);
  };

  class UniformBuffer
  {
    public:
      virtual ~UniformBuffer() = default;
      virtual void bind() const = 0;
      virtual void unbind() const = 0;
      virtual void setData(const void* data) const = 0;

      static std::unique_ptr<UniformBuffer> create(GV::API api, int size, uint32_t bindingPoint);
  };

}
