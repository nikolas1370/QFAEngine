#pragma once
#include <Tools/VulkanSuff.h>
#include <Tools/Array.h>
#include <Math/Vector.h>
#include <Render/UI/UIUnit.h>
#include <Render/Buffer/VKBuffer.h>

/* ttf type */
typedef unsigned long  FT_ULong;
struct FT_LibraryRec;
struct FT_FaceRec_;
typedef struct FT_LibraryRec_  *FT_Library;
typedef struct FT_FaceRec_*  FT_Face;
/* ------ */


class QFAViewport;
class QFAOverlord;
class QFAWindow;
class QFAVKVertexBuffer;

class QFAImage;
class QFAVKTextPipeline;
class QFAVKImageView;
class QFAVKTextureSampler;

class QFAPresentImagePipeline;

class QFAPresentImage
{
    
    friend QFAWindow;
    friend QFAPresentImagePipeline;

public:
  

    QFAPresentImage(VkCommandPool _commandPool);
    ~QFAPresentImage();


private: 
    void Init(VkRenderPass renderPass, VkCommandPool commandPool, QFAImage* imago, VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT);
    static void EndLife();

    

    QFAVKVertexBuffer* vertexBufer;
    static QFAPresentImagePipeline* Pipeline;
    static VkCommandPool commandPool;
    static QFAVKTextureSampler* ImageSampler;
    static VkRenderPass RenderPass;
    static QFAImage* image;
    static VkDescriptorImageInfo imageInfo;
    QFAVKImageView* view;

    struct ImageShaderVertex
    {
        float x;
        float y;
        float textureX;
        float textureY;
    };

    ImageShaderVertex quad[6];



};
