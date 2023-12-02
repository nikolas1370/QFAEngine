#include "Text.h"
#include <iostream>
#include <Render/Shader/ShaderProgram.h>
#include "Render/Window/Viewport.h"

bool QFAText::ISInit = false;
unsigned char* QFAText::RawTexture;
QFAShaderProgram* QFAText::shader;
FT_Library QFAText::ft;
FT_Face QFAText::face;
unsigned int QFAText::CountGlyphInBuffer = 100;
QFAText::GlyphShader* QFAText::GlyphInfoData;
QFAArray<QFAText::SGlyphAtlas> QFAText::GlyphAtlasList;
QFAArray<QFAText::PrepareData> QFAText::SymbolsForRender;
/*----*/

#pragma comment(lib, "freetype.lib")
/*----*/

void QFAText::CopyGlyph(const FT_GlyphSlot gl)
{
    int ic = gl->bitmap.rows - 1;
    for (int i = ic; i >= 0 ; i--)
        for (int j = 0; j < gl->bitmap.pitch; j++)
            RawTexture[(ic - i) * gl->bitmap.pitch + j] = gl->bitmap.buffer[i * gl->bitmap.pitch + j];
}

QFAText::QFAText()
{
    if (!ISInit)
    {        
        QFAText::RawTexture = (unsigned char*)malloc(90000);//  300 300

        shader = new QFAShaderProgram("Engine/Shaders/Font/Vertex.shader", "Engine/Shaders/Font/Fragment.shader");
        
        if (FT_Init_FreeType(&QFAText::ft))
        {
            std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
            return;
        }

        if (FT_New_Face(QFAText::ft, "Editor/Font/Roboto-Regular.ttf", 0, &face))
        {
            std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
            return;
        }

        ISInit = true;
        QFAText::GlyphInfoData = (GlyphShader*)malloc(sizeof(GlyphShader) * QFAText::CountGlyphInBuffer);
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

QFAText ::~QFAText()
{
    // delete VBO and other

    if (Viewport)
        Viewport->RemoveText(this);
}

void QFAText::AddGlyph(FT_ULong symbol)
{
    GlyphInfo gi;
    if (GlyphAtlasList.Length() == 0)
        CreateAtlas();    
    
    FT_Set_Pixel_Sizes(face, 0, FontLoadCharHeight);
    if (FT_Load_Char(face, symbol, FT_LOAD_RENDER))
    {
        std::cout << "QFAText ::AddGlyph FT_Load_Char error" << std::endl;
        __debugbreak();
    }   

    if (face->glyph->bitmap.rows > 0)
    {   
        if (face->glyph->bitmap.rows >= face->glyph->bitmap.width)
            gi.ratio = (float)face->glyph->bitmap.width / (float)face->glyph->bitmap.rows;
        else
            gi.ratio = (float)face->glyph->bitmap.rows / (float)face->glyph->bitmap.width;
    }
    else //space(' ')
        gi.ratio = (float)face->glyph->bitmap.width / (float)FontLoadCharHeight;

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
    
    CopyGlyph(face->glyph);
    glBindTexture(GL_TEXTURE_2D, atlas->atlasId);
    glTexSubImage2D(GL_TEXTURE_2D, 0, row->x, row->y, gi.width, gi.height, GL_RED, GL_UNSIGNED_BYTE, RawTexture);

    row->x += face->glyph->bitmap.width + OffsetBetweenGlyph;
    
    Symbols.Add(Symbol{ symbol, gi });    
}

unsigned int QFAText::CreateAtlas()
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

void QFAText::ProcessText()
{
    GLCall(glBindVertexArray(VAO));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    QFAText::SymbolsForRender.Clear();
        
    unsigned int numRow = 0;
    int x = 0;
    float tem = ((float)FontHeight / (float)FontLoadCharHeight);
    unsigned int wordLen = 0;
    unsigned int wordStart = 0;
    unsigned int wordW = 0;
    for (size_t i = 0; i < Text.size(); i++)
    { 
        bool find = false;     
        unsigned int symI;
        for (size_t j = 0; j < Symbols.Length(); j++)
        {
            if (Symbols[j].symbol == (FT_ULong)Text[i])
            {
                find = true;
                symI = (unsigned int)j;
                break;
            }
        }
        
        if (!find)
        {
            symI = (unsigned int)Symbols.Length();
            AddGlyph(Text[i]);           
        }

        unsigned int glyphWidth = (unsigned int)((float)FontHeight * Symbols[symI].Glyph.ratio);
        int glyphBitmap_left = (int)((float)Symbols[symI].Glyph.bitmap_left * tem);

        if (OverflowWrap == EOverflowWrap::OWSymbol)
        {                  
            if (Width < glyphWidth)
            {                
                QFAText::SymbolsForRender.Add(PrepareData{ (unsigned int)symI, numRow });
                numRow++;
                x = 0;
            }
            else if(x + glyphBitmap_left + glyphWidth >= Width)
            {
                numRow++;
                x = 0;
                QFAText::SymbolsForRender.Add(PrepareData{ (unsigned int)symI, numRow });                
                x += (int)((float)Symbols[symI].Glyph.advance_x * tem);
            }
            else
            {
                QFAText::SymbolsForRender.Add(PrepareData{ (unsigned int)symI, numRow });                
                x += (int)((float)Symbols[symI].Glyph.advance_x * tem);
            }            
        }        
        else if(OverflowWrap == EOverflowWrap::OWWord)
        {       
            wordW += (int)((float)Symbols[symI].Glyph.advance_x * tem);
            if (wordLen++ == 0)
                wordStart = (unsigned int)i;// in current time  all symbol in Text be presend in SymbolsForRender                         

            QFAText::SymbolsForRender.Add(PrepareData{ (unsigned int)symI});
            if (Text[i] == L' ' || i == Text.size() - 1)
            {
                int spaceS = 0;
                if (Text[i] == L' ')
                {
                    spaceS = (int)((float)Symbols[symI].Glyph.advance_x * tem);
                    wordW -= spaceS;
                }

                if (Width < wordW)
                {
                    if (x != 0)
                        numRow++;

                    for (size_t j = wordStart; j < wordStart + wordLen; j++)
                        QFAText::SymbolsForRender[j].row = numRow;
                    
                    numRow++;
                    x = 0;                    
                }
                else if(x + wordW <= Width)
                {
                    for (size_t j = wordStart; j < wordStart + wordLen; j++)
                        QFAText::SymbolsForRender[j].row = numRow;

                    x += wordW + spaceS;
                }
                else
                {
                    x = wordW + spaceS;
                    numRow++;
                    
                    for (size_t j = wordStart; j < wordStart + wordLen; j++)
                        QFAText::SymbolsForRender[j].row = numRow;        
                }                
                
                wordW = 0;
                wordLen = 0;
                wordStart = 0;
            }
        }
        else
            QFAText::SymbolsForRender.Add(PrepareData{ (unsigned int)symI, 0 });

    }    

    CountSymbolForRender = (unsigned int)SymbolsForRender.Length();
    PrepareSymbolsToGpu();
    


    if (CountSymbolForRender > CountGlyphInGUP)
    {
        CountGlyphInGUP = (unsigned int)((float)CountSymbolForRender * 1.5f);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GlyphShader) * CountGlyphInGUP, GlyphInfoData, GL_DYNAMIC_DRAW);
    }
    else
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GlyphShader) * CountSymbolForRender, GlyphInfoData);
   
    TextChange = false;
}

void QFAText::PrepareSymbolsToGpu()
{    
    int w = 0;
    unsigned int row = 0;
    float tem = ((float)FontHeight / (float)FontLoadCharHeight);
    for (size_t i = 0; i < CountSymbolForRender; i++)
    {    
        // for text-align. last ' ' in row not calculate in w
        if (row != QFAText::SymbolsForRender[i].row)
        {
            row = QFAText::SymbolsForRender[i].row;
            w = 0;
        }
        
        GlyphInfo* gi = &Symbols[QFAText::SymbolsForRender[i].symbolIndex].Glyph;

        float temX = ((float)Position_x + (float)w + (float)gi->bitmap_left * tem);
        float temXEnd = temX + (float)gi->width * tem;

        float start_y = (float)ViewPortHeight - (float)Position_y - (float)FontHeight * gi->HeightMultiplier - ((float)row *  ((float)FontHeight * gi->HeightMultiplier));// remove gi.HeightMultiplier if need less spase above base line
        float temY = start_y - ((float)gi->MaxDescender + (float)gi->bitmap_bottom) * tem;
        float temYEnd = temY + (float)gi->height * tem;
        
        QFAText::GlyphInfoData[i].leftBottom_1 = GlyphShaderVertex{ temX, temY ,   gi->x, gi->y, (int)gi->atlasIndex };       
        GlyphInfoData[i].rightBottom_1 = { temX, temYEnd,   gi->x, gi->yEnd, (int)gi->atlasIndex };
        GlyphInfoData[i].rightTop_1 = { temXEnd, temYEnd,   gi->xEnd, gi->yEnd, (int)gi->atlasIndex };
        
        GlyphInfoData[i].leftBottom_2 = GlyphInfoData[i].leftBottom_1;
        GlyphInfoData[i].rightTop_2 = GlyphInfoData[i].rightTop_1;
        GlyphInfoData[i].leftTop_2 = GlyphShaderVertex{ temXEnd, temY,   gi->xEnd, gi->y, (int)gi->atlasIndex };

        w += (int)((float)gi->advance_x * tem);
    }
}


void QFAText::StartTextRender(const glm::mat4& proj)
{
    shader->Use();
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    shader->SetProjectionMatrix(proj);
    
    for (size_t i = 0; i < GlyphAtlasList.Length(); i++)
    {
        shader->SetGlyphsTestureUnit(GlyphAtlasList[i].atlasIndex);
        GLCall(glActiveTexture(GL_TEXTURE0 + GlyphAtlasList[i].atlasIndex));
        GLCall(glBindTexture(GL_TEXTURE_2D, GlyphAtlasList[i].atlasId));
    }
}

void QFAText::Render()
{         
    if (TextChange)
        ProcessText();
    
    GLCall(glBindVertexArray(VAO));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    
    shader->SetTextColor(Color);
    shader->SetOutlineColor(OutlineColor);
    shader->SetOutlineStatus(Outline);
    shader->SetOpacity(Opacity);

    GLCall(glDrawArrays(GL_TRIANGLES, 0, 6 * (int)CountSymbolForRender));// 6 * Text.length()    
}

void QFAText::EndTextRender()
{    
    glDepthMask(GL_TRUE); 
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    
}

void QFAText::SetPosition(unsigned int x, unsigned int y)
{
    TextChange = true;
    Position_x = x;
    Position_y = y;
}

void QFAText::SetSize(unsigned int w, unsigned int h)
{
    TextChange = true;
    Width = w;
    Height = h;
}

void QFAText::Destroy()
{
    delete this;
}

void QFAText::ChangeViewportSize(unsigned int viewportWidth, unsigned int viewportHeight)
{
    TextChange = true;
    ViewPortWidth = viewportWidth;
    ViewPortHeight = viewportHeight;    
}


void QFAText::SetText(std::wstring  text)
{
    if (text.length() > CountGlyphInBuffer)
    {        
        free(QFAText::GlyphInfoData);
        QFAText::CountGlyphInBuffer = (unsigned int)((float)text.length() * 1.5);
        QFAText::GlyphInfoData = (GlyphShader*)malloc(sizeof(GlyphShader) * QFAText::CountGlyphInBuffer);
    }

    Text = text;
    TextChange = true;
    if (FontHeight == -1)
        SetTextSize(defaultTextSize);
}

void QFAText::SetTextSize(unsigned int height)
{
    if (height == 0)
        height = 1;

    TextChange = true;
    FontHeight = height;
}

void QFAText::SetOverflowWrap(EOverflowWrap wrap)
{
    TextChange = true;
    OverflowWrap = wrap;
}

void QFAText::EndLife()
{
    free(QFAText::GlyphInfoData);

    if(face)
        FT_Done_Face(face);

    if(ft)
        FT_Done_FreeType(ft);
}


/*
int lop = 0;
    glBindTexture(GL_TEXTURE_2D, texture);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_RED_SIZE, &lop);
*/
