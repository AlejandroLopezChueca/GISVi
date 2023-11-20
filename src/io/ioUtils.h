#pragma once

#include <gdal/gdal.h>
#include <gdal/gdal_priv.h>

namespace GV
{
  namespace IoUtils
  {
    void clipRasterByExtension(GDALDataset* dataset);
  }

}
