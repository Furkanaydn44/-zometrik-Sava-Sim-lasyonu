#include "isoEngine.h"
#include <stdio.h>
#include <math.h>

unsigned int TILESIZE;

void InitIsoEngine(isoEngineT *isoEngine,int tileSizeInPixels)
{

     if(tileSizeInPixels <= 0)
     {
          TILESIZE=32;
     }
     else{
         TILESIZE=tileSizeInPixels;
     }
     isoEngine->mapHeight = 0;
     isoEngine->mapWidth = 0;
     isoEngine->scrollX = 0;
     isoEngine->scrollY = 0;
}
void IsoEngineSetMapSize(isoEngineT *isoEngine,int width,int height)
{
    if(isoEngine == NULL)
    {
       return;
    }
     isoEngine->mapHeight = height;
        isoEngine->mapWidth = width;
}
void Convert2dToIso(point2DT *point)
{
    int tmpX = (point->y - point->x);
    int tmpY = (point->x + point->y)*0.5;
    point->x = tmpX;
    point->y = tmpY;
}
void GetTileCoordinates(point2DT *point,point2DT *point2DCoord)
{
    float tempX = (float)point->x / (float)TILESIZE;
    float tempY = (float)point->y / (float)TILESIZE;

    point2DCoord->x = (int) tempX;
    point2DCoord->y = (int) tempY;
}
