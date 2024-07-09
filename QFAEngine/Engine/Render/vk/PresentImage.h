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

class QFAVKPipeline;
class QFAPresentImage
{
    friend QFAWindow;

private:
    struct ImageShaderVertex
    {
        float x;
        float y;
        float textureX;
        float textureY;
    };

    static VkCommandPool commandPool;
    static QFAVKTextureSampler* ImageSampler;
    static std::array< VkDescriptorImageInfo, 1> imageInfos;

    QFAVKVertexBuffer* vertexBufer;
    QFAVKPipeline* Pipeline; // one QFAPresentImage one pipeline
    QFAVKImageView* view;
    ImageShaderVertex quad[6];

    static void EndLife();


public:
    QFAPresentImage(VkCommandPool _commandPool, VkRenderPass renderPass, QFAImage* image, VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT);
    ~QFAPresentImage();
};
