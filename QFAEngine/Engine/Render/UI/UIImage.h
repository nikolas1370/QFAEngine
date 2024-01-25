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

class QFAUIImage : public QFAUIUnit
{
    
    friend QFAWindow;
    friend QFAVKImagePipeline;

    void SetSizeParent(unsigned int w, unsigned int h) override {}
    void SetPositionParent(unsigned int x, unsigned int y) override {}

    QFAImage* Image;


public:
  

    QFAUIImage(VkCommandPool _commandPool);
    ~QFAUIImage();

    void SetImage(QFAImage* image);
private: 
    void Init(VkRenderPass renderPass, VkCommandPool commandPool);
    static void EndLife();

    

    QFAVKVertexBuffer* vertexBufer;
    static QFAVKImagePipeline* Pipeline;
    static VkCommandPool commandPool;
    static QFAVKTextureSampler* ImageSampler;
    static VkRenderPass RenderPass;

    static VkDescriptorImageInfo imageInfo;


    struct ImageShaderVertex
    {
        float x;
        float y;
        float textureX;
        float textureY;
    };

    ImageShaderVertex quad[6];


protected:
    void ParentEnable() override;
    void ParentDisable() override;
    void ParentAttach() override;
    void ParentDisconect() override;


    void RecreateCreatePiline();
    void CreatePiline();
};
