#include <math.h>
#include "camera.h"

Camera Camera_new()
{
    Camera new;

    new.X = 1.0f;
    new.Y = 1.0f;
    new.Angle = 0.0f;
    new.FOV = M_PI / 4.0f;
    new.Depth = 16;
    new.speed = 1.f;
    return (new);
}