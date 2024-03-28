#pragma once

#include <Tools/Array.h>
#include <Math/Vector.h>
#include <Render/UI/RenderUnit.h>
#include <Render/Buffer/VKBuffer.h>
#include <Render/Window/Window.h>
#include <Tools/Color.h>

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
class QFAUIScroll;

class QFAText : public QFAUIRenderUnit
{
    friend QFAViewport;
    friend QFAOverlord;
    friend QFAWindow;
    friend QFAVKPipeline;
    friend QFAUITextInput;
    friend QFAUIScroll;

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
            for some symbol in ukrainian need more spase above base line('¥'(U+0490) and '¯'(U+0407))
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

    QFAColorF Color = QFAColor(255).GetColorF();
public:
    enum EOverflowWrap
    {
        OWNone,
        OWSymbol,
        OWWord,// if word bigger than Width word will go out of bounds
        OWWordBreak // if word bigger than Width word will break
    };

    enum ETextAlign
    {
        TALeft,
        TACenter,
        TARight
    };


    QFAColorF OutlineColor ; // need create set function for OutlineColor
    bool Outline = false;

    QFAText();
    ~QFAText();

    void SetText(std::u32string  text);

    void SetTextSize(unsigned int height);
    
    void SetTextColor(QFAColor color)
    { 
        glm::vec3 linear = Math::srgb_to_linear(glm::vec3((float)color.R / 255.0f, (float)color.G / 255.0f, (float)color.B / 255.0f));
        Color = QFAColorF(linear.r, linear.g, linear.b);
    }

    void SetTextColor(QFAColorB color)
    {
        glm::vec3 linear = Math::srgb_to_linear(glm::vec3((float)color.R / 255.0f, (float)color.G / 255.0f, (float)color.B / 255.0f));
        Color = QFAColorF(linear.r, linear.g, linear.b);
    }

    void SetTextColor(QFAColorF color)
    {
        glm::vec3 linear = Math::srgb_to_linear(glm::vec3(color.R, color.G, color.B));
        Color = QFAColorF(linear.r, linear.g, linear.b);
    }

    void SetTextColor(unsigned char r, unsigned char g, unsigned char b)
    {        
        glm::vec3 linear = Math::srgb_to_linear(glm::vec3((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f));
        Color = QFAColorF(linear.r, linear.g, linear.b);
    }

    inline QFAColorF GetTextColor()
    {
        return Color;
    }

    /*----*/

    void Destroy();
    void SetOverflowWrap(EOverflowWrap wrap);
    void SetTextAlign(ETextAlign aligh);



    class QFAStringConventor
    {

    public:
        static std::u32string ToU32String(const char* str)
        {
            if (!str)
                return std::u32string();

            std::u32string string;
            size_t strL = strlen(str);
            string.resize(strL);
            for (size_t i = 0; i < strL; i++)
                string[i] = (char32_t)str[i];

            return string;
        }
    };

    

    struct SFont
    {
        friend QFAText;

        std::u32string GetFamilyName()
        {
            return familyName;
        }

        std::u32string GetStyleName()
        {
            return styleName;
        }

        /*
        
        return false if in newFamilyName exist font with styleName
        */
        bool SetFamilyName(std::u32string newFamilyName)
        {
            for (size_t i = 0; i < Fonts.size(); i++)
            {
                if (Fonts[i] == this)
                    continue;

                if (Fonts[i]->familyName == newFamilyName && Fonts[i]->styleName == styleName)
                    return false;
            }

            familyName = newFamilyName;
            return true;
        }

        /*  
        
        return false if font with newStyleName in familyName exist
        */
        bool SetStyleName(std::u32string newStyleName)
        {
            for (size_t i = 0; i < Fonts.size(); i++)
            {
                if (Fonts[i] == this)
                    continue;

                if (Fonts[i]->familyName == familyName && Fonts[i]->styleName == newStyleName)
                    return false;
            }

            styleName = newStyleName;
            return true;
        }

    private:        
        SFont()
        {

        }
        std::u32string familyName;
        std::u32string styleName; 
        FT_Face face;
        QFAArray<Symbol> Symbols;
    };
    
    enum ELoadFontResult
    {
        LRFSucceed = 0,
        LRFLoadError = 1,
        LRFFontStyleNameExist = 2,
        LRFFontFamilyNameNotFound = 3,// if ib font FamilyName == null, not return if in familyName not void
        LRFFontStyleNameNotFound = 4,// if ib font StyleName == null, not return if in styleName not void
        LRFPathWasNull = 5,
        LRFFreeTypeError = 6, //Could not init FreeType Library
        LRFEngineNotInit = 7 // if LoadFont called before Engine was initialized and first window be created
    };
    
    static ELoadFontResult LoadFont(const char* fontPath, SFont*& outFont, std::u32string* familyName = nullptr, std::u32string* styleName = nullptr);
    inline static SFont* GetFont(size_t index)
    {
        return Fonts[index];
    }

    static SFont* GetFont(std::u32string familyName, std::u32string styleName)
    {
        for (size_t i = 0; i < Fonts.size(); i++)
            if (Fonts[i]->familyName == familyName && Fonts[i]->styleName == styleName)
                return Fonts[i];

        return nullptr;
    }

    inline static size_t GetFontCount()
    {
        return Fonts.size();
    }

    bool SetFont(SFont* font)
    {
        if (!font)
            return false;

        for (size_t i = 0; i < Fonts.size(); i++)
        {
            if (Fonts[i] == font)
            {
                CurentFontIndex = i;
                TextChange = true;
                return true;
            }
        }

        return false;
    }

private:
    static std::vector<SFont*> Fonts;

    size_t CurentFontIndex;// store font for this text


    void SetSizeParent(unsigned int w, unsigned int h) override;
    void SetPositionParent(int x, int y) override;


    void updateUniformBuffer();
    void PrepareSymbolsToGpu();
    void ProcessText();
    static void AddGlyph(FT_ULong symbol, SFont* font);




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
    


    static const unsigned int FontLoadCharHeight = 50;// 'j' == 47/50(Height) after render 62
    static const unsigned int AtlasRowHeight = (unsigned int)((float)FontLoadCharHeight * 1.4f); // after call FT_Render_Glyph Glyph becomes bigger
    static const int OffsetBetweenGlyph = 2;//  
    static FT_Library ft;

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
        int outline;
        FVector outlineColor;
        float opacity;
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



    bool kio = false;
};