#ifndef __RENDER_TRACER__
#define __RENDER_TRACER__

#include "Engine/Render.h"
#include "Engine/camera/camera.h"
#include "pch.h"
#include "tgaimage.h"
namespace render  {

// 光线追踪器
class Tracer: public IRender {
public:
    Tracer() = default;
    void render(const Scene& scene) override;
    void clear() override; 
    void loadBuffer(TGAImage* image);
    void loadCamera(Camera * camera);
private:
    TGAImage* image_;
    Camera* camera_;
};
}
#endif