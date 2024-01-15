#pragma once
#include <Tools/Debug/OpenGlStuff.h>
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

/*
    need do buffers like in QMeshBaseComponent
        create new pool not delete old



*/

class QFAText : public QFAUIUnit
{
    friend QFAViewport;
    friend QFAOverlord;    
    friend QFAWindow;
    friend QFAVKTextPipeline;
    
    struct SGlyphAtlasListRow
    {
        unsigned int x = 0;// when start new Glyph in pixel
        unsigned int y = 0;// when row start/ Glyph be have y position      
    };

    struct GlyphInfo
    {
        // add size between base line searsh in ttf lib and add it
//unsigned int FontHeight; // value in FT_Set_Pixel_Sizes
        //unsigned int symbol;// now only ascii
        float x; // start in atlass persent
        float y; // start in atlass persent
        float xEnd; // end in atlass persent
        float yEnd; // end in atlass persent
        unsigned int width;// count pixel in atlas
        unsigned int height;// count pixel in atlas            
        int advance_x; // count pixel from left side to right side 
        int bitmap_left; // offset from leftbase line to first glyph pixel
        int bitmap_top; // offset from base line to last glyph pixel
        int bitmap_bottom; // offset base line to last glyph pixel (most positive or 0)
        int MaxAscender; // uper baseline
        int MaxDescender;// under baselin
        unsigned int atlasIndex;
        /*
            need for calculate max row Height(FontHeight)
            for some symbol in ukrainian need more spase above base line('�'(U+0490) and '�'(U+0407))
            exclude from code if need less spase above base line (row be bawe less Height)
        */
        float HeightMultiplier; 
        
        float ratio; // Width / Height
    };
    
    struct Symbol
    {
        FT_ULong symbol;// FT_Load_Char
        GlyphInfo Glyph;// store Glyph for different Height
    };

    struct GlyphShaderVertex
    {
        float x;
        float y;
        float texture_x;
        float texture_y;
        int AtlasNum;
    };

    struct GlyphShader
    {
        /* first triangle */
        GlyphShaderVertex leftBottom_1;
        GlyphShaderVertex rightBottom_1;
        GlyphShaderVertex rightTop_1;
        /* second triangle */
        GlyphShaderVertex leftBottom_2;
        GlyphShaderVertex rightTop_2;
        GlyphShaderVertex leftTop_2;
    };
    struct PrepareData
    {

        unsigned int symbolIndex;
        // row( row at screen) where be symbol
        unsigned int row;
    };

    struct SGlyphAtlas
    {
        int atlasIndex;
        int y = 0;// when start new row in pixel
        QFAArray<SGlyphAtlasListRow> Rows;
        QFAVKTextureImage* texture;
        QFAVKImageView* view;
    };
    
    struct UniformBufferTextParam
    {
        alignas(16) FVector textColor;
        alignas(4) int outline;
        alignas(16) FVector outlineColor;
        alignas(4) float opacity;
    };
public:
    enum EOverflowWrap
    {
        OWNone,
        OWSymbol,
        OWWord// if word bigger than Width word will go out of bounds
    };

    enum ETextAlign
    {
        TALeft,
        TACenter,
        TARight
    };

    FVector Color = FVector(1);
    FVector OutlineColor = FVector(1, 0, 0);
    bool Outline = false;
    float Opacity = 1;

    QFAText();
    ~QFAText();
    
    void SetText(std::wstring  text);
    void SetSize(unsigned int w, unsigned int h);    
    /* 0,0 == left top corner */
    void SetPosition(unsigned int x, unsigned int y);    
    void SetTextSize(unsigned int height);
    void Destroy();
    void SetOverflowWrap(EOverflowWrap wrap);
    void SetTextAlign(ETextAlign aligh);
        

    
private: 

    void updateUniformBuffer();
    void PrepareSymbolsToGpu();
    void ProcessText();
    void AddGlyph(FT_ULong symbol);
    
    static bool ReTextRender;

    void Render();

    static void StartTextRender(const glm::mat4& proj);
    static void CreateAtlas();
    static void Init(VkRenderPass renderPass, VkCommandPool commandPool);
    static void EndLife();
    

    
    bool TextChange;


    
    QFAVKVertexBuffer* vertexBufer;

    EOverflowWrap OverflowWrap = EOverflowWrap::OWWord;

    ETextAlign TextAlign = ETextAlign::TALeft;

    std::wstring  Text;

    unsigned int FontHeight = -1;// curent text height
    unsigned int CountGlyphInGUP = 100;
    unsigned int CountSymbolForRender = 0;


    static QFAArray<Symbol> Symbols;
    static const unsigned int FontLoadCharHeight = 50;// 'j' == 47/50(Height) after render 62
    static const unsigned int AtlasRowHeight = (unsigned int)((float)FontLoadCharHeight * 1.4f); // after call FT_Render_Glyph Glyph becomes bigger
    static const int OffsetBetweenGlyph = 2;//  
    static FT_Library ft;
    static FT_Face face;
    static const unsigned int defaultTextSize = 30;
    static const int GlyphAtlasHeight = 1000;
    static const int GlyphAtlasWidth = 1000;


    static QFAVKBuffer* uniformBufferProj;



    static QFAVKTextPipeline* Pipeline;
    static QFAVKTextPipeline* OldPipeline;
    static const std::string VertexShaderPath;
    static const std::string FragmentShaderPath;

    static unsigned int CountGlyphInBuffer;

    static GlyphShader* GlyphInfoData;
    // need for PrepareSymbolsToGpu, it symbol be process in GlyphShader
    static QFAArray<PrepareData> SymbolsForRender;

    static VkCommandPool commandPool;


    static QFAVKTextureSampler* AtlassSampler;
    

    static VkRenderPass RenderPass;

    static unsigned int MaxAttlas;
    static QFAArray<SGlyphAtlas> GlyphAtlasList;
    static std::vector<VkDescriptorImageInfo> DII;

    

 
    static int maxTextInframe;
    static std::vector<QFAVKBuffer*> TPB; // do allocation beter
  
    
    VkDescriptorSet CurentDescriptorSet;
    static int NumberTextInFrame;    
};