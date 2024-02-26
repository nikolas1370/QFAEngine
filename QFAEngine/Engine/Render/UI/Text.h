#pragma once

#include <Tools/Array.h>
#include <Math/Vector.h>
#include <Render/UI/RenderUnit.h>
#include <Render/Buffer/VKBuffer.h>
#include <Render/Window/Window.h>

/* ttf type */
typedef unsigned long  FT_ULong;
struct FT_LibraryRec;
struct FT_FaceRec_;
typedef struct FT_LibraryRec_* FT_Library;
typedef struct FT_FaceRec_* FT_Face;
/* ------ */


class QFAViewport;
class QFAOverlord;

class QFAVKVertexBuffer;

class QFAImage;
class QFAVKPipeline;

class QFAVKImageView;
class QFAVKTextureSampler;
class QFAUITextInput;
class QFAText : public QFAUIRenderUnit
{
    friend QFAViewport;
    friend QFAOverlord;
    friend QFAWindow;
    friend QFAVKPipeline;
    friend QFAUITextInput;

    struct SGlyphAtlasListRow
    {
        unsigned int x = 0;// when start new Glyph in pixel
        unsigned int y = 0;// when row start/ Glyph be have y position      
    };

    struct GlyphInfo
    {
        // add size between base line searsh in ttf lib and add it
//unsigned int FontHeight; // value in FT_Set_Pixel_Sizes
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

        float ratio;
        /*
            difference between face->glyph->bitmap.width
            before FT_Render_Glyph and after
            necessary for calculate the position of the pen
        */
        float differenceWidth; 
    };

    struct Symbol
    {
        FT_ULong symbol;// FT_Load_Char
        GlyphInfo Glyph;// store Glyph for different Height
    };

    struct GlyphShaderVertex
    {
        float x ;
        float y ;
        float z ;
        float texture_x = 0;
        float texture_y = 0;
        int AtlasNum = 0;
    };

    struct GlyphShader
    {
        /* first triangle */
        GlyphShaderVertex leftTop_1;
        GlyphShaderVertex leftBottom_1;
        GlyphShaderVertex rightBottom_1;
        /* second triangle */
        GlyphShaderVertex leftTop_2;
        GlyphShaderVertex rightBottom_2;
        GlyphShaderVertex rightTop_2;
    };
    struct PrepareData
    {

        unsigned int symbolIndex;
        // row( row at screen) where be symbol
        unsigned int row;
    };

    struct SGlyphAtlas
    {
        int atlasIndex = -1; // not created atlas hawe index -1
        int y = 0;// when start new row in pixel
        QFAArray<SGlyphAtlasListRow> Rows;
        QFAImage* texture;
        QFAVKImageView* view;
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

    QFAText();
    ~QFAText();

    void SetText(std::u32string  text);

    void SetTextSize(unsigned int height);
    /*----*/

    void Destroy();
    void SetOverflowWrap(EOverflowWrap wrap);
    void SetTextAlign(ETextAlign aligh);


private:

    void SetSizeParent(unsigned int w, unsigned int h) override;
    void SetPositionParent(int x, int y) override;


    void updateUniformBuffer();
    void PrepareSymbolsToGpu();
    void ProcessText();
    static void AddGlyph(FT_ULong symbol);




    void Render(VkCommandBuffer comandebuffer) override;
    void RenderPen(VkCommandBuffer comandebuffer);
    static void StartFrame();


    static void Init(VkRenderPass renderPass, VkCommandPool commandPool);
    static void EndLife();



    bool TextChange;

    QFAVKVertexBuffer* vertexBufer;
    static QFAVKVertexBuffer* PenVertexBufer;

    EOverflowWrap OverflowWrap = EOverflowWrap::OWWord;

    ETextAlign TextAlign = ETextAlign::TALeft;
   

    unsigned int FontHeight = -1;// curent text height
    unsigned int CountGlyphInGUP = 100;
    unsigned int CountSymbolForRender = 0; // replase to TextMetadata.size()
    

    static QFAArray<Symbol> Symbols;
    static const unsigned int FontLoadCharHeight = 50;// 'j' == 47/50(Height) after render 62
    static const unsigned int AtlasRowHeight = (unsigned int)((float)FontLoadCharHeight * 1.4f); // after call FT_Render_Glyph Glyph becomes bigger
    static const int OffsetBetweenGlyph = 2;//  
    static FT_Library ft;
    static FT_Face face;
    static const unsigned int defaultTextSize = 30;
    static const int GlyphAtlasHeight = 1000;
    static const int GlyphAtlasWidth = 1000;



    static QFAVKPipeline* Pipeline;
    static QFAVKPipeline* OldPipeline;


    static unsigned int CountGlyphInBuffer;

    /*
        use for all non textinput text
        textinput have own exemplar
    */
    static GlyphShader* GlyphInfoData;
    
    // not use directly get from TextMetadata
    static QFAArray<PrepareData> SymbolsForRender_; // remove '_'

    static VkCommandPool commandPool;


    static QFAVKTextureSampler* AtlassSampler;


    static VkRenderPass RenderPass;

    static unsigned int MaxAttlas;
    static std::vector<SGlyphAtlas> GlyphAtlasList;

    
    static VkDescriptorSet CurentDescriptorSet;
    static VkDescriptorSet CurentDescriptorSetProject;

    static std::vector<QFAVKBuffer*> textParamBuffers;
    static std::vector<QFAVKBuffer*> textVertexParamBuffers;
    
    static std::vector<VkDescriptorImageInfo> DII;

    
    static void CreatePipeline();
    static void RecreatePipeline();


    static void CreateTextProjectionSet(VkBuffer buffer);

    static void CreateTextParameterSet();
    static void RecreateTextParameterSet(VkBuffer buffer, VkBuffer VertexBuffer);

    static const int AmountSetsInTextParamPool = 20;
    static int maxTextInframe;

    static int NumberTextInFrame;
    static const int MinNumberTextInFrame = 1;// zero index reserv fore pen set
    struct UniformBufferTextParam
    {
        alignas(16) FVector textColor;
        alignas(4) int outline;
        alignas(16) FVector outlineColor;
        alignas(4) float opacity;
        UniformOverflow overflow;
        int pen = 0;
    };

    struct UniformBufferTextParamVertex
    {
        float offset; 
        float offsetX;
    };

    inline QFAVKPipeline* GetPipeline() override
    {
        return Pipeline;
    }

protected:
    struct SText
    {
        SText() {}
        SText(std::u32string& string)
        {
            text = string;
        }

        std::u32string text;
        char32_t* pText = nullptr; // need for textInput
        size_t pTextSize = 0; // if pTextSize == 0 use std::u32string text
        size_t maxSize;  // set in textInput
        bool penEnable = false;
        bool inputFocus = false; // if input have focus for text input
        bool CanSeePen = false;
        // symbol index when pan stay
        unsigned int pen = 0; // carriage
        inline size_t size()
        {
            return pTextSize == 0 ? text.size() : pTextSize;
        }

        inline char32_t operator[](size_t index) const
        {
            return pTextSize == 0 ? text[index] : pText[index];
        }
    };
    SText Text;

    /*    
        store list symbol with metadata( struct PrepareData ) for second render step
        and for calculate pen

        for all non textinput text use one block memory
        each textinput have own metadata block memory
    */
    struct STextMetadata
    {// QFAArray<PrepareData> SymbolsForRender_;
        friend QFAText;
        bool onlyText = true; 
        size_t count = 0;
        PrepareData* inputData = nullptr;
        inline size_t size()
        {
            return count;
        }

        inline PrepareData& operator[](size_t index) const
        {
            return onlyText ? SymbolsForRender_[index] : inputData[index];
        }

        void Add(PrepareData pd)
        {
            if (onlyText)
                SymbolsForRender_.Add(pd);
            else
                inputData[count] = pd;

            count++;
        }

        void Clear()
        {
            count = 0;
            if (onlyText)
                SymbolsForRender_.Clear();
        }
    };

    
    STextMetadata TextMetadata;
    unsigned int countTextRow = 0;

    void GetPenPosition(QFAText::GlyphShader& gs);
    void WritePenInfo();

    UniformBufferTextParam TextUniformParam;
    static VkDescriptorSet PenDescriptorSet;
    // set pointer to input text
    void SetInputText(char32_t* pText, size_t pTextSize, size_t maxSize);
};