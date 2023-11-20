#include "ioUtils.h"

#include <cstddef>
#include <gdal/gdal_priv.h>
#include <gdal/gdalwarper.h>

void GV::IoUtils::clipRasterByExtension(GDALDataset* dataset)
{

    // Setup warp options.
    GDALWarpOptions *psWarpOptions = GDALCreateWarpOptions();


    // Initialize and execute the warp operation on region
    GDALWarpOperation oOperation;

    GDALDestroyWarpOptions(psWarpOptions);

}
