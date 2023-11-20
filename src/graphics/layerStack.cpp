#include "layerStack.h"
#include <iostream>

GV::LayerStack::~LayerStack()
{
  for (Layer* layer : m_Layers)
  {
    layer -> onDetach();
    delete layer;
  }
}

void GV::LayerStack::pushLayer(Layer *layer)
{
  m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex,layer);
  m_LayerInsertIndex++;

}

void GV::LayerStack::pushOverlay(Layer *layer)
{
  m_Layers.emplace_back(layer);

}

void GV::LayerStack::popLayer(Layer *layer)
{

}

void GV::LayerStack::popOverlay(Layer *layer)
{

}
