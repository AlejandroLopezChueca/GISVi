#include "ui/app.h"

int main()
{
  if (GV::App::s_appCreated) return 0;
  GV::App app;
  if (app.getInitApp())
  {
    GV::App::s_appCreated = true;
    app.startApp();
    return 0;
  }
}
