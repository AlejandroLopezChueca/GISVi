#pragma once

#include "graphics/texture.h"
#include "graphics/API.h"

#include <filesystem>
#include <memory>

namespace GV
{
  class ContentBrowserPanel
  {
    public:
      ContentBrowserPanel(GV::API api);


      void onImGuiRender();

      private:
	GV::API m_Api;
	std::filesystem::path m_CurrentDirectory;
	std::filesystem::path m_BaseDirectory;

	std::unique_ptr<GV::Texture2D> m_DirectoryIcon;
	std::unique_ptr<GV::Texture2D> m_FileIcon;


  };
}
