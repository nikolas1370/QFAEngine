#pragma once
#include <Tools/VulkanSuff.h>
#include <Tools/Array.h>
#include <Math/Vector.h>
#include <Render/UI/RenderUnit.h>
#include <Render/Buffer/VKBuffer.h>
#include <Tools/Color.h>



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
class QFAVKImageView;
class QFAVKTextureSampler;

class QFAUIParent;
class QFAVKPipeline;

class QFAUIImage : public QFAUIRenderUnit
{
    
    friend QFAWindow;
    friend QFAUIParent;
    


    void SetSizeParent(unsigned int w, unsigned int h) override;
    void SetPositionParent(int x, int y) override;

    QFAImage* Image = nullptr;
    /*    
        if Image == nullptr
        image be hawe this color
    */
    QFAColorF BackgroundColor = QFAColorF(0.0f,0.0f,0.0f,0.0f);


    static QFAVKPipeline* Pipeline;
    static void CreatePipeline();
    
    void Render(VkCommandBuffer comandebuffer) override;
    void UpdateUniforms();
    static VkDescriptorSet CurentDescriptorSetProject;

    // if this image use for Background image
    bool IBackground = false;

    // if false ratio be same like in image
    bool CanStretch = false;
public:
  

    QFAUIImage(QFAImage* image, bool iBackground = false);
    ~QFAUIImage();

    /*
        if Image == nullptr
        image area be have color of BackgroundColor
    */
    void SetImage(QFAImage* image);
   

    void SetBackgroundColor(QFAColor color)
    {

        glm::vec3 linear = Math::srgb_to_linear(glm::vec3((float)color.R / 255.0f, (float)color.G / 255.0f, (float)color.B / 255.0f));
        BackgroundColor = QFAColorF(linear.r, linear.g, linear.b, (float)color.A / 255.0f);
    }

    void SetBackgroundColor(QFAColorB color)
    {
        glm::vec3 linear = Math::srgb_to_linear(glm::vec3((float)color.R / 255.0f, (float)color.G / 255.0f, (float)color.B / 255.0f));
        BackgroundColor = QFAColorF(linear.r, linear.g, linear.b, (float)color.A / 255.0f);
    }

    void SetBackgroundColor(QFAColorF color)
    {
        glm::vec3 linear = Math::srgb_to_linear(glm::vec3(color.R, color.G, color.B));
        BackgroundColor = QFAColorF(linear.r, linear.g, linear.b, color.A);
    }


    void SetCanStretch(bool can);

private: 
    static void Init(VkRenderPass renderPass, VkCommandPool commandPool);
    static void EndLife();

    static void CreateProjectionSet(VkBuffer buffer);

    QFAVKVertexBuffer* vertexBufer;
    static QFAVKPipeline* QFAVKPipline;
    static VkCommandPool commandPool;
    static QFAVKTextureSampler* ImageSampler;
    static VkRenderPass RenderPass;

    static VkDescriptorImageInfo imageInfo;


    struct SImageShaderVertex
    {
        float x;
        float y;
        float z;
        float textureX;
        float textureY;
    };

    SImageShaderVertex quad[6];

    struct SImageIndex
    {
        QFAVKBuffer* buffer;
        QFAVKBuffer* bufferVertex;
        QFAUIImage* image = nullptr;
        unsigned int setIndex;
    };

    static std::vector<SImageIndex> ImageIndexs;
    unsigned int Index = 0;

    static const unsigned int AmountSetsInImageParamPool = 100;


    struct SImageParam
    {
        float opacity;
        UniformOverflow overflow;
        int BackgroundEnable = 0;
        alignas(16)QFAColorF BackgroundColor;
    };

    struct SImageVertexParam
    {
        float offset;        
    };

    void ChangeQuad();

    inline QFAVKPipeline* GetPipeline() override
    {
        return Pipeline;
    }
protected:
    void ParentEnable() override;
    void ParentDisable() override;
    void ParentAttach() override;
    void ParentDisconect() override;


    void PrepareSet();
    void DisableImage();

    static QFAImage* VoidImage;
};
