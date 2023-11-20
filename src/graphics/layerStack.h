#pragma once

#include "layer.h"
#include <vector>

namespace GV
{
  class LayerStack
  {
    public:
      LayerStack() = default;
      ~LayerStack();

      void pushLayer(Layer* layer);
      void pushOverlay(Layer* layer);
      void popLayer(Layer* layer);
      void popOverlay(Layer* layer);

      std::vector<Layer*>::iterator begin() {return m_Layers.begin();}
      std::vector<Layer*>::iterator end() {return m_Layers.end();}

    private:
      std::vector<Layer*> m_Layers;
      unsigned int m_LayerInsertIndex = 0;
  };
}
