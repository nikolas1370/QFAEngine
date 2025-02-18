#pragma once
#include <Core/EngineStuff/VulkanSuff.h>
#include <Tools/Array.h>
#include <Math/Vector.h>
#include <UI/RenderUnit.h>
#include <Core/EngineStuff/Buffer/VKBuffer.h>
#include <Tools/Color.h>
#include <Core/EngineStuff/Image.h>


/* ttf type */
typedef unsigned long  FT_ULong;
struct FT_LibraryRec;
struct FT_FaceRec_;
typedef struct FT_LibraryRec_  *FT_Library;
typedef struct FT_FaceRec_*  FT_Face;
/* ------ */


class QFAEngineViewport;
class QFAOverlord;
class QFAEngineWindow;
class QFAVKVertexBuffer;

class QFAVKTextureImage;
class QFAVKImageView;
class QFAVKTextureSampler;

class QFAUIParent;
class QFAVKPipeline;

class QFAEXPORT QFAUIImage : public QFAUIRenderUnit
{
    
    friend QFAEngineWindow;
    friend QFAUIParent;
#if QFA_EDITOR_ONLY
    friend void QFAImage::UpdateImage(QFAImage* image);
#endif


    struct SImageParam
    {
        float opacity;
        UniformOverflow overflow;
        int BackgroundEnable = 0;
        alignas(16)QFAColorF BackgroundColor;
    };

    struct SImageShaderVertex
    {
        float x;
        float y;
        float z;
        float textureX;
        float textureY;
    };

    struct SImageIndex
    {
        QFAVKBuffer* buffer;
        QFAVKBuffer* bufferVertex;
        QFAUIImage* image = nullptr;
        unsigned int setIndex;
    };
    struct SImageVertexParam
    {
        float offset;
        float offsetX;
    };

    static const unsigned int AmountSetsInImageParamPool = 20;
    static VkDescriptorSet CurentDescriptorSetProject;
    static QFAVKPipeline* Pipeline;
    static QFAVKPipeline* QFAVKPipline;
    static VkCommandPool commandPool;
    static QFAVKTextureSampler* ImageSampler;
    static VkRenderPass RenderPass;
    static VkDescriptorImageInfo imageInfo;
    static std::vector<SImageIndex> ImageIndexs;

protected:
    static QFAImage* VoidImage;

private:
    QFAVKVertexBuffer* vertexBufer;
    SImageShaderVertex quad[6];
    unsigned int Index = 0;
    QFAImage* Image = nullptr;
    /*
        if Image == nullptr
        image be hawe this color
    */
    QFAColorF BackgroundColor = QFAColorF(0.0f, 0.0f, 0.0f, 0.0f);
    // if this image use for Background image
    bool IBackground = false;
    // if false ratio be same like in image
    bool CanStretch = false;

private:
    static void CreatePipeline();
    static void Init(VkRenderPass renderPass, VkCommandPool commandPool);
    static void EndLife();
    static void CreateProjectionSet(VkBuffer buffer);

    void Render(VkCommandBuffer comandebuffer) override;
    void UpdateUniforms();
    void ChangeQuad();
    QFAVKPipeline* GetPipeline() override;

protected:
    void ParentEnable() override;
    void ParentDisable() override;
    void ParentAttach() override;
    void ParentDisconect() override;

    bool CallPrepareSet = false;
    void PrepareSet();
    void DisableImage();

    void WidthChanged(int oldValue = 0) override;
    void HeightChanged(int oldValue = 0) override;
    void TopChanged(int oldValue = 0) override;
    void LeftChanged(int oldValue = 0) override;

public:
    QFAUIImage(QFAImage* image, bool iBackground = false);
    ~QFAUIImage();


    /*
        if Image == nullptr
        image area be have color of BackgroundColor
    */
    void SetImage(QFAImage* image);
    void SetBackgroundColor(QFAColor color);
    void SetBackgroundColor(QFAColorB color);
    void SetBackgroundColor(QFAColorF color);
    void SetCanStretch(bool can);
};
