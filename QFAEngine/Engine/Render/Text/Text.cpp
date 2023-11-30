#include "Text.h"
#include <iostream>
#include <Render/Shader/ShaderProgram.h>
#include "Render/Window/Viewport.h"

bool QFARenderText::ISInit = false;
unsigned char* QFARenderText::RawTexture;
QFAShaderProgram* QFARenderText::shader;
FT_Library QFARenderText::ft;
FT_Face QFARenderText::face;
unsigned int QFARenderText::CountGlyphInBuffer = 100;
QFARenderText::GlyphShader* QFARenderText::GlyphInfoData;
QFAArray<QFARenderText::SGlyphAtlas> QFARenderText::GlyphAtlasList;
glm::mat4 QFARenderText::projection;
/*----*/

#pragma comment(lib, "freetype.lib")
/*----*/

void QFARenderText::CopyGlyph(const FT_GlyphSlot gl)
{
    int ic = gl->bitmap.rows - 1;
    for (int i = ic; i >= 0 ; i--)
        for (int j = 0; j < gl->bitmap.pitch; j++)
            RawTexture[(ic - i) * gl->bitmap.pitch + j] = gl->bitmap.buffer[i * gl->bitmap.pitch + j];
}

QFARenderText::QFARenderText()
{
    if (!ISInit)
    {        
        QFARenderText::RawTexture = (unsigned char*)malloc(90000);//  300 300

        shader = new QFAShaderProgram("Engine/Shaders/Font/Vertex.shader", "Engine/Shaders/Font/Fragment.shader");
        
        if (FT_Init_FreeType(&QFARenderText::ft))
        {
            std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
            return;
        }

        if (FT_New_Face(QFARenderText::ft, "Editor/Font/Roboto-Regular.ttf", 0, &face))
        {
            std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
            return;
        }

        ISInit = true;
        QFARenderText::GlyphInfoData = (GlyphShader*)malloc(sizeof(GlyphShader) * QFARenderText::CountGlyphInBuffer);
    }
    
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GlyphShader) * CountGlyphInGUP, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GlyphShaderVertex) , 0);
    glEnableVertexAttribArray(1);
    glVertexAttribIPointer(1, 1, GL_INT, sizeof(GlyphShaderVertex), (void*)offsetof(GlyphShaderVertex, AtlasNum));
}

QFARenderText::~QFARenderText()
{
    // delete VBO and other

    if (Viewport)
        Viewport->RemoveText(this);
}

QFARenderText::GlyphInfo QFARenderText::AddGlyph(FT_ULong symbol)
{
    GlyphInfo gi;
    if (GlyphAtlasList.Length() == 0)
        CreateAtlas();    
    
    FT_Set_Pixel_Sizes(face, 0, FontLoadCharHeight);
    if (FT_Load_Char(face, symbol, FT_LOAD_RENDER))
    {
        std::cout << "QFARenderText::AddGlyph FT_Load_Char error" << std::endl;
        __debugbreak();
        return gi;
        /* do some magic*/
    }   
    // 
    //std::cout <<  << " <----\n";
    //std::cout << face->max_advance_width << " <----\n";

    if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_SDF))
    {
        std::cout << "ERORRRRRRR" << std::endl;
    }
    
    SGlyphAtlas* atlas = nullptr;;
    SGlyphAtlasListRow* row = nullptr;
    bool find = false;
    for (size_t i = 0; i < GlyphAtlasList.Length() && !find; i++)
    {        
        for (size_t j = 0; j < GlyphAtlasList[i].Rows.Length() ; j++)
        {
            if (face->glyph->bitmap.width + GlyphAtlasList[i].Rows[j].x <= GlyphAtlasWidth)
            {                
                atlas = &GlyphAtlasList[i];
                row = &GlyphAtlasList[i].Rows[j];
                find = true;
                break;
            }
        }        
    }

    if (!row)
    {
        for (size_t i = 0; i < GlyphAtlasList.Length(); i++)
        {
            if (GlyphAtlasList[i].y + AtlasRowHeight <= GlyphAtlasHeight)
            {
                GlyphAtlasList[i].Rows.Add(SGlyphAtlasListRow{});
                atlas = &GlyphAtlasList[i];                
                row = &GlyphAtlasList[i].Rows.Last();                     
                break;
            }
        }

        if (!row)
        {       
            CreateAtlas();
            atlas = &GlyphAtlasList.Last();            
            GlyphAtlasList.Last().Rows.Add(SGlyphAtlasListRow{});
            row = &GlyphAtlasList.Last().Rows.Last();                                    
        }

        row->y = atlas->y;
        atlas->y += AtlasRowHeight + OffsetBetweenGlyph;
    }

    gi.height = face->glyph->bitmap.rows;
    gi.width = face->glyph->bitmap.width;
    gi.x = (float)row->x / (float)GlyphAtlasWidth;
    gi.y = (float)row->y / (float)GlyphAtlasHeight;
    gi.xEnd = gi.x + ((float)gi.width / (float)GlyphAtlasWidth);
    gi.yEnd = gi.y + ((float)gi.height / (float)GlyphAtlasHeight);    
    gi.advance_x = face->glyph->advance.x / 64;    
    gi.bitmap_left = face->glyph->bitmap_left;
    gi.bitmap_top = face->glyph->bitmap_top;
    gi.bitmap_bottom = gi.height - gi.bitmap_top;    
    gi.atlasIndex = atlas->atlasIndex;

    gi.HeightMultiplier = (float)(face->ascender - face->descender) / (float)face->units_per_EM;
    float maxH = (float)(face->ascender - face->descender);// descender < 0
    gi.MaxAscender = (int)round(((float)FontLoadCharHeight * ((float)face->ascender / maxH)));
    gi.MaxDescender = (int)round(((float)FontLoadCharHeight * ((float)face->descender / maxH)));
    /* 
    gi.MaxAscender = (int)round(((float)FontHeight * ((float)face->ascender / maxH)));
    gi.MaxDescender = (int)round(((float)FontHeight * ((float)face->descender / maxH)));
    **/
    CopyGlyph(face->glyph);// RawTexture
    glBindTexture(GL_TEXTURE_2D, atlas->atlasId);
    glTexSubImage2D(GL_TEXTURE_2D, 0, row->x, row->y, gi.width, gi.height, GL_RED, GL_UNSIGNED_BYTE, RawTexture);

    row->x += face->glyph->bitmap.width + OffsetBetweenGlyph;
    
    Symbols.Add(Symbol{ symbol, gi });
    row->Glyphs.Add(gi);
    return gi;
}

unsigned int QFARenderText::CreateAtlas()
{
    std::cout <<  "CreateAtlas\n";
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    GLCall(glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        GlyphAtlasWidth,
        GlyphAtlasHeight,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        NULL
    ));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GlyphAtlasList.Add(SGlyphAtlas{ texture, (int)GlyphAtlasList.Length()});
    return texture;
}

void QFARenderText::ProcessText()
{
    GLCall(glBindVertexArray(VAO));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));

    int w = 0;
    
    for (size_t i = 0; i < Text.size(); i++)
    { 

        bool find = false;
        GlyphInfo gi;        
        for (size_t j = 0; j < Symbols.Length(); j++)
        {
            if (Symbols[j].symbol == (FT_ULong)Text[i])
            {
                gi = Symbols[j].Glyph;
                find = true;
                break;
            }
        }
        
        if (!find)
            gi = AddGlyph(Text[i]);

        

        float tem = ((float)FontHeight  / (float)FontLoadCharHeight);
                
        float temX = ((float)Position_x + (float)w + (float)gi.bitmap_left * tem);
        float temXEnd = temX + (float)gi.width * tem;
        
        float start_y = (float)ViewPortHeight - (float)Position_y - (float)FontHeight * gi.HeightMultiplier;// remove gi.HeightMultiplier if need less spase above base line
        float temY = start_y - ((float)gi.MaxDescender + (float)gi.bitmap_bottom) * tem ;
        float temYEnd = temY + (float)gi.height * tem ;

        GlyphInfoData[i].leftBottom_1 = { temX, temY ,   gi.x, gi.y, (int)gi.atlasIndex };
        GlyphInfoData[i].rightBottom_1 ={ temX, temYEnd,   gi.x, gi.yEnd, (int)gi.atlasIndex };
        GlyphInfoData[i].rightTop_1 = {temXEnd, temYEnd,   gi.xEnd, gi.yEnd, (int)gi.atlasIndex };

        GlyphInfoData[i].leftBottom_2 = GlyphInfoData[i].leftBottom_1;
        GlyphInfoData[i].rightTop_2 = GlyphInfoData[i].rightTop_1;
        GlyphInfoData[i].leftTop_2 = GlyphShaderVertex{temXEnd, temY,   gi.xEnd, gi.y, (int)gi.atlasIndex };
        
        w += (int)((float)gi.advance_x * tem );
    }
    
    if (Text.length() > CountGlyphInGUP)
    {
        CountGlyphInGUP = (unsigned int)((float)Text.length() * 1.5f);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GlyphShader) * CountGlyphInGUP, GlyphInfoData, GL_DYNAMIC_DRAW);
    }
    else
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GlyphShader) * Text.length(), GlyphInfoData);
   
    TextChange = false;
}

void QFARenderText::StartTextRender()
{
    shader->Use();
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    shader->SetProjectionMatrix(QFARenderText::projection);
    
    for (size_t i = 0; i < GlyphAtlasList.Length(); i++)
    {
        shader->SetGlyphsTestureUnit(GlyphAtlasList[i].atlasIndex);
        GLCall(glActiveTexture(GL_TEXTURE0 + GlyphAtlasList[i].atlasIndex));
        GLCall(glBindTexture(GL_TEXTURE_2D, GlyphAtlasList[i].atlasId));
    }
}
#include <Render/Time.h>

void QFARenderText::Render()
{   
    if (TextChange)
        ProcessText();

    GLCall(glBindVertexArray(VAO));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    
    shader->SetTextColor(Color);
    shader->SetOutlineColor(OutlineColor);
    shader->SetOutlineStatus(Outline);
    shader->SetOpacity(Opacity);

    GLCall(glDrawArrays(GL_TRIANGLES, 0, 6 * (int)Text.length()));// 6 * Text.length()    
}

void QFARenderText::EndTextRender()
{
    glDepthMask(GL_TRUE); // Re-enable writing to the depth buffer
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}

void QFARenderText::SetPosition(unsigned int x, unsigned int y)
{
    TextChange = true;
    Position_x = x;
    Position_y = y;
}

void QFARenderText::SetSize(unsigned int w, unsigned int h)
{
    TextChange = true;
    Width = w;
    Height = h;
}

void QFARenderText::Destroy()
{
    delete this;
}

void QFARenderText::ChangeProjection(unsigned int viewportWidth, unsigned int viewportHeight)
{
    TextChange = true;
    ViewPortWidth = viewportWidth;
    ViewPortHeight = viewportHeight;
    QFARenderText::projection = glm::ortho(0.0f, (float)viewportWidth, 0.0f, (float)viewportHeight);
}


void QFARenderText::SetText(std::wstring  text)
{
    if (text.length() > CountGlyphInBuffer)
    {
        
        free(QFARenderText::GlyphInfoData);
        QFARenderText::CountGlyphInBuffer *= 2;
        QFARenderText::GlyphInfoData = (GlyphShader*)malloc(sizeof(GlyphShader) * QFARenderText::CountGlyphInBuffer);
    }

    Text = text;
    TextChange = true;
    if (FontHeight == -1)
        SetTextSize(defaultTextSize);
}

void QFARenderText::SetTextSize(unsigned int height)
{
    TextChange = true;
    FontHeight = height;
}

/*
int lop = 0;
    glBindTexture(GL_TEXTURE_2D, texture);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_RED_SIZE, &lop);
*/
