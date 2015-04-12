#ifndef gears_h
#define gears_h
int glMain(int argc, char **argv);
typedef void (*loopfunc)(void);
void setOtherLoop(loopfunc);
void setPan(float x, float y, float z);
void setRotation(float x, float y, float z);
void setZoom(float amount);
#endif