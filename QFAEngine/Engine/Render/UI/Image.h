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

class QFAVKTextureImage;
class QFAVKTextPipeline;
class QFAVKImageView;
class QFAVKTextureSampler;

class QFAVKImagePipeline;



class QFAImage : public QFAUIUnit
{
    
    friend QFAWindow;
    friend QFAVKImagePipeline;
public:
  

    QFAImage(VkCommandPool _commandPool);
    ~QFAImage();
        
    void SetPosition(unsigned int x, unsigned int y);    
    
    void Destroy();

    void SetSize(unsigned int w, unsigned int h)
    {

    }
private: 
    void Init(VkRenderPass renderPass, VkCommandPool commandPool, QFAVKTextureImage* imago, VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT);
    static void EndLife();

    QFAVKVertexBuffer* vertexBufer;
    static QFAVKImagePipeline* Pipeline;
    static VkCommandPool commandPool;
    static QFAVKTextureSampler* ImageSampler;
    static VkRenderPass RenderPass;
    static QFAVKTextureImage* image;
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
