#pragma once

#include <Math/Vector.h>
#include <Core/EngineStuff/Buffer/VKBuffer.h>
#include <UI/RenderUnit.h>
#include <Core/EngineStuff/Window/EngineWindow.h>
#include <Tools/Array.h>
#include <Tools/Color.h>
#include "UIParentHiddenChild.h"


/* ttf type */
typedef unsigned long  FT_ULong;
struct FT_LibraryRec;
struct FT_FaceRec_;
typedef struct FT_LibraryRec_* FT_Library;
typedef struct FT_FaceRec_* FT_Face;
/* ------ */

class QFAEngineViewport;
class QFAOverlord;

class QFAVKVertexBuffer;

class QFAImage;
class QFAVKPipeline;

class QFAVKImageView;
class QFAVKTextureSampler;
class QFAUITextInput;
class QFAUIScroll;
class QFATextBackground;
class QFAEngineTextLocalization;
class QFAEXPORT QFAText : public QFAUIRenderUnit
{
    friend QFAEngineViewport;
    friend QFAOverlord;
    friend QFAEngineWindow;
    friend QFAVKPipeline;
    friend QFAUITextInput;
    friend QFAUIScroll;
    friend QFATextBackground;
    friend QFAEngineTextLocalization;
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

protected:
    struct SText
    {
        std::u32string text;
        char32_t* pText = nullptr; // need for textInput
        size_t pTextSize = 0; // if pTextSize == 0 use std::u32string text
        size_t maxSize;  // set in textInput
        bool penEnable = false;
        bool inputFocus = false; // if input have focus for text input
        bool CanSeePen = false;
        // symbol index when pan stay
        unsigned int pen = 0; // carriage

        SText() {}
        SText(const std::u32string& string)
        {
            text = string;
        }

        inline size_t size()
        {
            return pTextSize == 0 ? text.size() : pTextSize;
        }

        inline char32_t operator[](size_t index) const
        {
            return pTextSize == 0 ? text[index] : pText[index];
        }
    };
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
        std::u32string familyName;
        std::u32string styleName;
        FT_Face face;
        QFAArray<Symbol> Symbols;
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
        bool SetFamilyName(std::u32string newFamilyName);

        /*

        return false if font with newStyleName in familyName exist
        */
        bool SetStyleName(std::u32string newStyleName);

        SFont(){}
    };

    struct SLoadFont
    {
        const char* fontPath;
        SFont* outFont;
        std::u32string* familyName;
        std::u32string* styleName;
    };

    enum ELoadFontResult
    {
        LRFSucceed = 0,
        LRFLoadError = 1,
        LRFFontStyleNameExist = 2,
        LRFFontFamilyNameNotFound = 3,// if ib font FamilyName == null, not return if in familyName not void
        LRFFontStyleNameNotFound = 4,// if ib font StyleName == null, not return if in styleName not void
        LRFPathWasNull = 5,
        LRFFreeTypeError = 6
    };

private:
    static std::vector<SFont*> Fonts;
    static QFAVKVertexBuffer* PenVertexBufer;
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
    static const int AmountSetsInTextParamPool = 20;
    static int maxTextInframe;
    static int NumberTextInFrame;
    static const int MinNumberTextInFrame = 1;// zero index reserv fore pen set
    static std::vector<SLoadFont> LoadFontList;

protected:
    static VkDescriptorSet PenDescriptorSet;

private:
    QFAColorF Color = QFAColor(255).GetColorF();
    size_t CurentFontIndex;// store font for this text
    bool TextChange;
    QFAVKVertexBuffer* vertexBufer;
    EOverflowWrap OverflowWrap = EOverflowWrap::OWWord;
    ETextAlign TextAlign = ETextAlign::TALeft;
    unsigned int FontHeight = -1;// curent text height
    unsigned int CountGlyphInGUP = 100;
    unsigned int CountSymbolForRender = 0; // replase to TextMetadata.size()
    QFAEngineTextLocalization* TextLocal = nullptr;

protected:
    SText Text;
    STextMetadata TextMetadata;
    unsigned int countTextRow = 0;
    UniformBufferTextParam TextUniformParam;

public:
    QFAColorF OutlineColor ; // need create set function for OutlineColor
    bool Outline = false;

private:
    static void AddGlyph(FT_ULong symbol, SFont* font);
    static void StartFrame();
    static void Init(VkRenderPass renderPass, VkCommandPool commandPool);
    static void EndLife();
    static void CreatePipeline();
    static void RecreatePipeline();
    static void CreateTextProjectionSet(VkBuffer buffer);
    static void CreateTextParameterSet();
    static void RecreateTextParameterSet(VkBuffer buffer, VkBuffer VertexBuffer);

public:    
    /*
        QFAText::SFont* font;
        QFAText::LoadFont("EngineFiles/Fonts/Roboto-Regular.ttf", font);
    */
    static ELoadFontResult LoadFont(const char* fontPath, SFont*& outFont, std::u32string* familyName = nullptr, std::u32string* styleName = nullptr);
    static SFont* GetFont(size_t index);
    static SFont* GetFont(std::u32string familyName, std::u32string styleName);
    static size_t GetFontCount();

private:
    /*
    void SetSizeParent(unsigned int w, unsigned int h) override;
    void SetPositionParent(int x, int y) override;
    */
    void updateUniformBuffer();
    void PrepareSymbolsToGpu();
    void ProcessText();
    void Render(VkCommandBuffer comandebuffer) override;
    void RenderPen(VkCommandBuffer comandebuffer);
    QFAVKPipeline* GetPipeline() override;
    // in QFATextLocalization  call
    void TextLocalizationChange();
protected:
    /*
        in position not include position_x and position_y
    */
    void GetPenPosition(QFAText::GlyphShader& gs);
    void WritePenInfo();
    // set pointer to input text
    void SetInputText(char32_t* pText, size_t pTextSize, size_t maxSize);

    void WidthChanged(int oldValue = 0) override;
    void HeightChanged(int oldValue = 0) override;
    void TopChanged(int oldValue = 0) override;
    void LeftChanged(int oldValue = 0) override;
public:// std::u32string(&CppFileButtonIconCode, 1)
    QFAText();
    ~QFAText();

    void SetText(const char32_t symbol);
    void SetText(const std::u32string& text);    
    void SetText(QFAEngineTextLocalization* text);
    inline void SetText(QFAEngineTextLocalization& text)
    {
        SetText(&text);
    }

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

    void SetOverflowWrap(EOverflowWrap wrap);
    void SetTextAlign(ETextAlign aligh);
    bool SetFont(SFont* font);
};


class QFAEXPORT QFATextBackground : public QFAParentHiddenChild
{
    QFAText Text;

protected:
    void WidthChanged(int oldValue = 0) override;
    void HeightChanged(int oldValue = 0) override;
    void TopChanged(int oldValue = 0) override;
    void LeftChanged(int oldValue = 0) override;

    virtual float UpdateInnerHeight()
    {
        return Height;
    }
    virtual float UpdateInnerWidth()
    {
        return Width;
    }

    // child call if his slot change
    virtual void MySlotChange(QFAUIUnit* unit) {}

public:
    QFATextBackground()
    {
        AddHiddenChild(&Text);
    }

    inline void SetText(std::u32string  text)
    {
        Text.SetText(text);
    }

    inline void SetTextSize(unsigned int height)
    {
        Text.SetTextSize(height);
    }

    inline void SetTextColor(QFAColor color)
    {
        Text.SetTextColor(color);
    }

    inline void SetTextColor(QFAColorB color)
    {
        Text.SetTextColor(color);
    }

    inline void SetTextColor(QFAColorF color)
    {
        Text.SetTextColor(color);
    }

    inline void SetTextColor(unsigned char r, unsigned char g, unsigned char b)
    {
        Text.SetTextColor(r, g, b);
    }

    inline QFAColorF GetTextColor()
    {
        return Text.GetTextColor();
    }

    inline void SetOverflowWrap(QFAText::EOverflowWrap wrap)
    {
        Text.SetOverflowWrap(wrap);
    }
    inline void SetTextAlign(QFAText::ETextAlign aligh)
    {
        Text.SetTextAlign(aligh);
    }

    inline void SetFont(QFAText::SFont* font)
    {
        Text.SetFont(font);
    }
};