#pragma once
#include <Tools/Debug/OpenGlStuff.h>
#include <Tools/Array.h>


#include <ft2build.h>
#include <Math/Vector2D.h>
#include FT_FREETYPE_H  

class QFAShaderProgram;
class QFAViewport;

class QFARenderText
{
    friend QFAViewport;
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
        QFAArray<QFARenderText::GlyphInfo> Glyphs;
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
        unsigned int FontHeight; // value in FT_Set_Pixel_Sizes
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
    };
    
    struct Symbol
    {
        FT_ULong symbol;// FT_Load_Char
        QFAArray<GlyphInfo> GlyphList;// store Glyph for different Height
    };

    QFAArray<Symbol> Symbols;
    static QFAArray<SGlyphAtlas> GlyphAtlasList;


	void CopyGlyph(const FT_GlyphSlot gl);

    unsigned int ViewPortWidth;
    unsigned int ViewPortHeight;

	int Width;
    int Height;
    int Position_x = 0;
    // 0 == top 
    int Position_y = 0;
    
    unsigned int FontHeight = -1;// curent text height
    static const int OffsetBetweenGlyph = 5;// 

	

	static FT_Library ft;
	static FT_Face face;

	std::string text = "";

    static const unsigned int defaultTextSize = 50;
    
	static QFAShaderProgram* shader;
	static bool ISInit;
	unsigned int VAO; 
	unsigned int VBO; 
    unsigned int CountGlyphInGUP = 100;

	static unsigned char* RawTexture;
	static const int GlyphAtlasHeight = 320;//1000;
    static const int GlyphAtlasWidth = 300;//1000;


    static unsigned int CountGlyphInBuffer;
    static GlyphShader* GlyphInfoData;

    static glm::mat4 projection;
    /*------*/

    std::string Text;
    bool TextChange;

    /* put in  result */
    void ProcessText();

    GlyphInfo AddGlyph(FT_ULong symbol);
    unsigned int CreateAtlas();

    /* call only in window thread call before Render*/
    static void StartTextRender();
    /* call only in window thread */
    void Render();
    /* call only in window thread call after Render*/
    static void EndTextRender();

    
    
    // call if viewport changed size or QFARenderText add in QFAViewport
    void ChangeProjection(unsigned int viewportWidth, unsigned int viewportHeight);
    void SetSize(unsigned int w, unsigned int h);

    
public:
	QFARenderText();
    ~QFARenderText();
    
    
    void SetText(std::string text);

    
    
    /* 0,0 == left top corner */
    void SetPosition(unsigned int x, unsigned int y);
    
    void SetTextSize(unsigned int height);

//    FT_Face face2;
    void Destroy();
};
