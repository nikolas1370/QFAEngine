#include <Tools/Debug/OpenGlStuff.h>
#include <Tools/Array.h>


#include <ft2build.h>
#include <Math/Vector2D.h>
#include <Math/Vector4D.h>
#include FT_FREETYPE_H  

class QFAShaderProgram;
class QFAViewport;
class QFAOverlord;

class QFAText
{
    friend QFAViewport;
    friend QFAOverlord;
    struct GlyphInfo;

    QFAViewport* Viewport = nullptr;

    struct GlyphShaderVertex
    {
        float x;
        float y;
        float texture_x;
        float texture_y;
        int AtlasNum;
    };
    // before name GlyphShaderInfo
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


    /*
    all Glyph in row have same Height( store in GlyphInfo.FontHeight)
*/
    struct SGlyphAtlasListRow
    {
        unsigned int x = 0;// when start new Glyph in pixel
        unsigned int y = 0;// when row start/ Glyph be have y position      
    };

    struct SGlyphAtlas
    {
        unsigned int atlasId;// texture id
        int atlasIndex;
        int y = 0;// when start new row in pixel
        QFAArray<SGlyphAtlasListRow> Rows;
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
        int bitmap_top; // offset from bottom base line to last glyph pixel
        int bitmap_bottom; // offset bottom base line to last glyph pixel (most positive or 0)
        int MaxAscender; // uper baseline
        int MaxDescender;// under baselin
        unsigned int atlasIndex;
        /*
            need for calculate max row Height(FontHeight)
            for some symbol in ukrainian need more spase above base line('•'(U+0490) and 'ѓ'(U+0407))
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

    QFAArray<Symbol> Symbols;
    static QFAArray<SGlyphAtlas> GlyphAtlasList;


	void CopyGlyph(const FT_GlyphSlot gl);

    unsigned int ViewPortWidth;
    unsigned int ViewPortHeight;



    int Position_x = 0;
    // 0 == top 
    int Position_y = 0;
    
    const unsigned int FontLoadCharHeight = 50;// 'j' == 47/50(Height) after render 62
    const unsigned int AtlasRowHeight = (unsigned int)((float)FontLoadCharHeight * 1.4f); // after call FT_Render_Glyph Glyph becomes bigger

    unsigned int FontHeight = -1;// curent text height
    static const int OffsetBetweenGlyph = 5;// 

	
	static FT_Library ft;
	static FT_Face face;

	std::wstring text;

    static const unsigned int defaultTextSize = 30;
    
	static QFAShaderProgram* shader;
	static bool ISInit;
	unsigned int VAO; 
	unsigned int VBO; 
    unsigned int CountGlyphInGUP = 100;

	static unsigned char* RawTexture;
	static const int GlyphAtlasHeight = 1000;//1000;
    static const int GlyphAtlasWidth = 1000;//1000;




    //glm::mat4 projection;
    /*------*/

    std::wstring  Text;
    bool TextChange;

    /* put in  result */
    void ProcessText();

    void AddGlyph(FT_ULong symbol);
    unsigned int CreateAtlas();

    /* call only in window thread call before Render*/
    static void StartTextRender(const glm::mat4& proj);
    /* call only in window thread */
    void Render();
    /* call only in window thread call after Render*/
    static void EndTextRender();

    
    
    
    void ChangeViewportSize(unsigned int viewportWidth, unsigned int viewportHeight);
    void SetSize(unsigned int w, unsigned int h);

    
public:
    QFAText();
    ~QFAText();
    
    
    void SetText(std::wstring  text);

    
    
    /* 0,0 == left top corner */
    void SetPosition(unsigned int x, unsigned int y);
    
    void SetTextSize(unsigned int height);

//    FT_Face face2;
    void Destroy();

    FVector Color = FVector(1);
    FVector OutlineColor = FVector(1, 0, 0);
    bool Outline = false;
    float Opacity = 1;

    

    /*
    text-overflow
        clip 
        ellipsis
    */
    /*
    text-align
        left Ч по левому краю. »спользуетс€ по умолчанию
        center Ч по центру
        right Ч по правому краю
    */
    enum EOverflowWrap
    {
        OWNone,
        OWSymbol, 
        OWWord// if word bigger than Width word will go out of bounds
    };
    
    void SetOverflowWrap(EOverflowWrap wrap);

    
    
private: 
    unsigned int Width = 300;
    unsigned int Height = 120;
    


    
    EOverflowWrap OverflowWrap = EOverflowWrap::OWWord;
    //EOverflowWrap OverflowWrap = EOverflowWrap::OWSymbol;
    

    struct PrepareData
    {
        
        unsigned int symbolIndex;
        // row( row at screen) where be symbol
        unsigned int row;
        
    };
    static unsigned int CountGlyphInBuffer;
    // send in gpu 
    static GlyphShader* GlyphInfoData;
    // need for PrepareSymbolsToGpu, it symbol be process in GlyphShader
    static QFAArray<PrepareData> SymbolsForRender;
    unsigned int CountSymbolForRender = 0;
    void PrepareSymbolsToGpu();

    static void EndLife();
};
