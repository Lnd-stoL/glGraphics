
float calculateLinearDepth (vec2 clipPlanes, float zScreen)
{
    float zFar  = clipPlanes.y;
    float zNear = clipPlanes.x;

    return zFar * zNear / (zFar - zScreen * (zFar - zNear));
}
