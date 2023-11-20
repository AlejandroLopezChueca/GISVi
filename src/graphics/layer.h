#pragma once


namespace GV
{
  class Layer
  {
  public:
    virtual ~Layer() = default;

    virtual void onAttach() = 0;
    virtual void onDetach() = 0;
    virtual void onUpdate() {};
    virtual void onImGuiRender() {};
    virtual void onEvent() {};

  };

}
