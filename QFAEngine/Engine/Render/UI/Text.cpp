﻿#include "Text.h"
#include <Render/vk/LogicalDevice.h>
#include <Render/Image.h>
#include <Render/Buffer/VertexBuffer.h>
#include <Render/UI/UIParentMultipleUnit.h>
#include <Render/Pipline/Pipline.h>

#include <ft2build.h>
#include FT_FREETYPE_H  

FT_Library QFAText::ft;
FT_Face QFAText::face;
QFAText::GlyphShader* QFAText::GlyphInfoData;

QFAVKPipeline* QFAText::Pipeline = nullptr;
QFAVKPipeline* QFAText::OldPipeline = nullptr;

VkCommandPool QFAText::commandPool;

QFAArray<QFAText::Symbol> QFAText::Symbols;
QFAArray<QFAText::PrepareData> QFAText::SymbolsForRender_;
std::vector<QFAText::SGlyphAtlas> QFAText::GlyphAtlasList;
std::vector<VkDescriptorImageInfo> QFAText::DII;
std::vector<QFAVKBuffer*> QFAText::textParamBuffers;
std::vector<QFAVKBuffer*> QFAText::textVertexParamBuffers;

VkRenderPass QFAText::RenderPass;
QFAVKTextureSampler* QFAText::AtlassSampler;

VkDescriptorSet QFAText::CurentDescriptorSet;
VkDescriptorSet QFAText::CurentDescriptorSetProject;

QFAVKVertexBuffer* QFAText::PenVertexBufer;

unsigned int QFAText::CountGlyphInBuffer = 100;
unsigned int QFAText::MaxAttlas = 10;
int QFAText::NumberTextInFrame;
int QFAText::maxTextInframe = 0;
VkDescriptorSet QFAText::PenDescriptorSet;
QFAText::QFAText()
{
    CanRender = true;
    Type = QFAUIType::Text;
    vertexBufer = new QFAVKVertexBuffer(sizeof(GlyphShader) * CountGlyphInGUP, nullptr, commandPool);
}

void QFAText::Init(VkRenderPass renderPass, VkCommandPool commandPool_)
{
    commandPool = commandPool_;
    RenderPass = renderPass;

    AtlassSampler = new QFAVKTextureSampler();
    
    CreatePipeline();


    if (FT_Init_FreeType(&QFAText::ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return;
    }

    if (FT_New_Face(QFAText::ft, "SomeFont/Roboto-Regular.ttf", 0, &face))
    {
        stopExecute("ERROR::FREETYPE: Failed to load font");
        return;
    }

    QFAText::GlyphInfoData = (GlyphShader*)malloc(sizeof(GlyphShader) * QFAText::CountGlyphInBuffer);
    AddGlyph(U'Q'); // add just in case
    PenVertexBufer = new QFAVKVertexBuffer(sizeof(GlyphShader), nullptr, commandPool);
    CreateTextParameterSet();// create set for Pen
    PenDescriptorSet = Pipeline->GetSet(1, 0);
}

QFAText::~QFAText()
{
    delete vertexBufer;
}

void QFAText::AddGlyph(FT_ULong symbol)
{
    GlyphInfo gi;
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

    float before = face->glyph->bitmap.width;
    if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_SDF))
        std::cout << "ERORRRRRRR" << std::endl;

    if (face->glyph->bitmap.width == 0) //space(' ')
        gi.differenceWidth = 1;
    else
        gi.differenceWidth = before / face->glyph->bitmap.width;
    
    SGlyphAtlas* atlas = nullptr;;
    SGlyphAtlasListRow* row = nullptr;
    bool find = false;
    for (size_t i = 0; i < GlyphAtlasList.size() && !find; i++)
    {
        for (size_t j = 0; j < GlyphAtlasList[i].Rows.Length(); j++)
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
        for (size_t i = 0; i < GlyphAtlasList.size(); i++)
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
            RecreatePipeline();
            atlas = &GlyphAtlasList.back();
            GlyphAtlasList.back().Rows.Add(SGlyphAtlasListRow{});
            row = &GlyphAtlasList.back().Rows.Last();
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

    if (face->glyph->bitmap.pitch != 0 || face->glyph->bitmap.rows != 0)
    {
        GlyphAtlasList[atlas->atlasIndex].texture->buffer->UpdateData(gi.height * gi.width, face->glyph->bitmap.buffer);
        GlyphAtlasList[atlas->atlasIndex].texture->buffer->copyInImage(GlyphAtlasList[atlas->atlasIndex].texture, gi.width, gi.height, commandPool, (unsigned int)(gi.x * (float)GlyphAtlasWidth), (unsigned int)(gi.y * (float)GlyphAtlasHeight));
    }

    row->x += face->glyph->bitmap.width + OffsetBetweenGlyph;
    Symbols.Add(Symbol{ symbol, gi });


}

void QFAText::ProcessText()
{
    InnerHeight = 0;
    TextMetadata.Clear();

    countTextRow = 0;
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
            if (Width < (int)glyphWidth)
            {
                TextMetadata.Add(PrepareData{ (unsigned int)symI, countTextRow });
                countTextRow++;
                x = 0;
            }
            else if (x + glyphBitmap_left + (int)glyphWidth >= Width)
            {
                countTextRow++;
                x = 0;
                TextMetadata.Add(PrepareData{ (unsigned int)symI, countTextRow });
                x += (int)((float)Symbols[symI].Glyph.advance_x * tem);
            }
            else
            {
                TextMetadata.Add(PrepareData{ (unsigned int)symI, countTextRow });
                x += (int)((float)Symbols[symI].Glyph.advance_x * tem);
            }
        }
        else if (OverflowWrap == EOverflowWrap::OWWord)
        {
            wordW += (int)((float)Symbols[symI].Glyph.advance_x * tem);
            if (wordLen++ == 0)
                wordStart = (unsigned int)i;

            TextMetadata.Add(PrepareData{ (unsigned int)symI });
            if (Text[i] == U' ' || i == Text.size() - 1)
            {
                int spaceS = 0;
                if (Text[i] == U' ')
                {
                    spaceS = (int)((float)Symbols[symI].Glyph.advance_x * tem);
                    wordW -= spaceS;
                }

                if (Width < (int)wordW)
                {
                    if (x != 0)
                        countTextRow++;

                    for (size_t j = wordStart; j < wordStart + wordLen; j++)
                        TextMetadata[j].row = countTextRow;

                    countTextRow++;
                    x = 0;
                }
                else if (x + (int)wordW <= Width)
                {
                    for (size_t j = wordStart; j < wordStart + wordLen; j++)
                        TextMetadata[j].row = countTextRow;

                    x += wordW + spaceS;
                }
                else
                {
                    x = wordW + spaceS;
                    countTextRow++;

                    for (size_t j = wordStart; j < wordStart + wordLen; j++)
                        TextMetadata[j].row = countTextRow;
                }

                wordW = 0;
                wordLen = 0;
                wordStart = 0;
            }
        }
        else
            TextMetadata.Add(PrepareData{ (unsigned int)symI, 0 });
    }

    CountSymbolForRender = (unsigned int)TextMetadata.size();
    PrepareSymbolsToGpu();

    if (CountSymbolForRender > CountGlyphInGUP)
    {
        CountGlyphInGUP = (unsigned int)((float)CountSymbolForRender * 1.5f);
        delete vertexBufer;
        vertexBufer = new QFAVKVertexBuffer(sizeof(GlyphShader) * CountGlyphInGUP, GlyphInfoData, commandPool);
    }
    else
        vertexBufer->UpdateData(sizeof(GlyphShader)* CountSymbolForRender, GlyphInfoData);

    TextChange = false;
}

void QFAText::PrepareSymbolsToGpu()
{
    if (CountSymbolForRender == 0)
        return;

    unsigned int temCountSymbolForRender = CountSymbolForRender;

    int w = 0;
    unsigned int row = 0;
    float tem = ((float)FontHeight / (float)FontLoadCharHeight);
    if (TextAlign == ETextAlign::TALeft)
    {
        for (size_t i = 0; i < CountSymbolForRender; i++)
        {
            if (row != TextMetadata[i].row)
            {
                row = TextMetadata[i].row;
                w = 0;
            }

            GlyphInfo* gi = &Symbols[TextMetadata[i].symbolIndex].Glyph;

            float glypgWidth = (float)gi->width;
            if (Symbols[TextMetadata[i].symbolIndex].symbol == U' ')
                glypgWidth = gi->advance_x;

            float temX = ((float)Position_x + (float)w + gi->bitmap_left * tem);
            float temXEnd = temX +  glypgWidth * tem;

            float start_y = (float)Position_y + ((float)FontHeight * gi->HeightMultiplier - (float)FontHeight) + ((float)row * ((float)FontHeight * gi->HeightMultiplier));
            
            float temY = start_y + (float)(gi->MaxAscender - gi->bitmap_top) * tem;
            float temYEnd = temY + (float)gi->height * tem;

            QFAText::GlyphInfoData[i].leftTop_1 = GlyphShaderVertex{ temX, temY , (float)ZIndex,   gi->x, gi->y,  (int)gi->atlasIndex };
            GlyphInfoData[i].leftBottom_1 = { temX, temYEnd,   (float)ZIndex, gi->x, gi->yEnd,(int)gi->atlasIndex };
            GlyphInfoData[i].rightBottom_1 = { temXEnd, temYEnd,   (float)ZIndex, gi->xEnd, gi->yEnd, (int)gi->atlasIndex };

            GlyphInfoData[i].leftTop_2 = GlyphInfoData[i].leftTop_1;
            GlyphInfoData[i].rightBottom_2 = GlyphInfoData[i].rightBottom_1;
            GlyphInfoData[i].rightTop_2 = GlyphShaderVertex{ temXEnd, temY,  (float)ZIndex,  gi->xEnd, gi->y, (int)gi->atlasIndex };

            if(temYEnd - Position_y > InnerHeight)
                InnerHeight = (unsigned int)((int)temYEnd - Position_y);

            w += (int)((float)gi->advance_x * tem);
        }
    }
    else if (TextAlign == ETextAlign::TACenter)
    {
        int rowLen = 0;
        for (size_t startRow = 0; startRow < CountSymbolForRender; startRow += rowLen)
        {
            int row = TextMetadata[startRow].row;
            int rowW = 0;
            rowLen = 0;
            for (size_t i = startRow; i < CountSymbolForRender; i++)
            {
                if (TextMetadata[i].row != row)
                {
                    if ((Symbols[TextMetadata[i - 1].symbolIndex].symbol == U' '))
                        rowW -= (int)((float)Symbols[TextMetadata[i - 1].symbolIndex].Glyph.advance_x * tem) - 1;

                    break;
                }

                rowLen++;
                rowW += (int)((float)Symbols[TextMetadata[i].symbolIndex].Glyph.advance_x * tem);
            }

            w = ((int)Width - rowW) / 2;// rowW can be larger Width
            for (size_t i = startRow; i < startRow + rowLen; i++)
            {
                GlyphInfo* gi = &Symbols[TextMetadata[i].symbolIndex].Glyph;
                float glypgWidth = (float)gi->width;
                if (Symbols[TextMetadata[i].symbolIndex].symbol == U' ')
                    glypgWidth = gi->advance_x;

                float temX = ((float)Position_x + (float)w + gi->bitmap_left * tem);
                float temXEnd = temX + glypgWidth * tem;

                float start_y = (float)Position_y + ((float)FontHeight * gi->HeightMultiplier - (float)FontHeight) + ((float)row * ((float)FontHeight * gi->HeightMultiplier));
                float temY = start_y + (float)(gi->MaxAscender - gi->bitmap_top) * tem;
                float temYEnd = temY + (float)gi->height * tem;

                QFAText::GlyphInfoData[i].leftTop_1 = GlyphShaderVertex{ temX, temY ,  (float)ZIndex,  gi->x, gi->y,  (int)gi->atlasIndex };
                GlyphInfoData[i].leftBottom_1 = { temX, temYEnd,  (float)ZIndex,  gi->x, gi->yEnd, (int)gi->atlasIndex };
                GlyphInfoData[i].rightBottom_1 = { temXEnd, temYEnd,   (float)ZIndex, gi->xEnd, gi->yEnd, (int)gi->atlasIndex };

                GlyphInfoData[i].leftTop_2 = GlyphInfoData[i].leftTop_1;
                GlyphInfoData[i].rightBottom_2 = GlyphInfoData[i].rightBottom_1;
                GlyphInfoData[i].rightTop_2 = GlyphShaderVertex{ temXEnd, temY,  (float)ZIndex,  gi->xEnd, gi->y, (int)gi->atlasIndex };

                if (temYEnd - Position_y > InnerHeight)
                    InnerHeight = (unsigned int)((int)temYEnd - Position_y);

                w += (int)((float)gi->advance_x * tem);
            }
        }
    }
    else if (TextAlign == ETextAlign::TARight)
    {// do revers
        int statrtFor = 0;// if last symbol space(' ') symbol past and if pen not be render
        if (Symbols[TextMetadata[CountSymbolForRender - 1].symbolIndex].symbol == U' ' &&
            !(Text.penEnable && Text.inputFocus)) 
            CountSymbolForRender--;

        row = TextMetadata[CountSymbolForRender - 1].row;
        w = Width;
        for (int i = CountSymbolForRender - 1; i >= 0; i--)
        {
            if (row != TextMetadata[i].row)
            {
                row = TextMetadata[i].row;
                w = Width;
            }
            
            if (!(Text.penEnable && Text.inputFocus) && 
                Symbols[TextMetadata[i].symbolIndex].symbol == U' ' && TextMetadata[i].row != TextMetadata[i + 1].row)
            {// if pen not be render // if last symbol in row be space(' ') symbol have all parameters 0                
                QFAText::GlyphInfoData[i].leftTop_1 = GlyphShaderVertex{ 0, 0,   0, 0, 0, 0 };
                GlyphInfoData[i].leftBottom_1 = { 0, 0,   0, 0, 0, 0 };
                GlyphInfoData[i].rightBottom_1 = { 0, 0,   0, 0, 0 , 0};

                GlyphInfoData[i].leftTop_2 = GlyphInfoData[i].leftTop_1;
                GlyphInfoData[i].rightBottom_2 = GlyphInfoData[i].rightBottom_1;
                GlyphInfoData[i].rightTop_2 = GlyphShaderVertex{ 0, 0,   0, 0, 0 , 0};
                GlyphInfo* temGi = &Symbols[TextMetadata[i].symbolIndex].Glyph;

                w += (int)((float)temGi->width * tem - 1.0f);
                continue;
            }
            

            GlyphInfo* gi = &Symbols[TextMetadata[i].symbolIndex].Glyph;
            float glypgWidth = (float)gi->width;
            if (Symbols[TextMetadata[i].symbolIndex].symbol == U' ')
                glypgWidth = gi->advance_x;

            float temX = ((float)Position_x + (float)w - (float)gi->advance_x * tem + (float)gi->bitmap_left * tem);
            float temXEnd = temX + glypgWidth * tem;

            float start_y = (float)Position_y + ((float)FontHeight * gi->HeightMultiplier - (float)FontHeight) + ((float)row * ((float)FontHeight * gi->HeightMultiplier));
            float temY = start_y + (float)(gi->MaxAscender - gi->bitmap_top) * tem;
            float temYEnd = temY + (float)gi->height * tem;

            QFAText::GlyphInfoData[i].leftTop_1 = GlyphShaderVertex{ temX, temY ,  (float)ZIndex,  gi->x, gi->y,  (int)gi->atlasIndex };
            GlyphInfoData[i].leftBottom_1 = { temX, temYEnd,   (float)ZIndex, gi->x, gi->yEnd,  (int)gi->atlasIndex };
            GlyphInfoData[i].rightBottom_1 = { temXEnd, temYEnd,   (float)ZIndex, gi->xEnd, gi->yEnd, (int)gi->atlasIndex };

            GlyphInfoData[i].leftTop_2 = GlyphInfoData[i].leftTop_1;
            GlyphInfoData[i].rightBottom_2 = GlyphInfoData[i].rightBottom_1;
            GlyphInfoData[i].rightTop_2 = GlyphShaderVertex{ temXEnd, temY,  (float)ZIndex,  gi->xEnd, gi->y,  (int)gi->atlasIndex };

            if (temYEnd - Position_y > InnerHeight)
                InnerHeight = (unsigned int)((int)temYEnd - Position_y);

            w -= (int)((float)gi->advance_x * tem);
        }
    }
}


void QFAText::Destroy()
{
    delete this;
}

void QFAText::SetText(std::u32string  text)
{
    if (text.length() > CountGlyphInBuffer)
    {
        free(QFAText::GlyphInfoData);
        QFAText::CountGlyphInBuffer = (unsigned int)((float)text.length() * 1.5);
        QFAText::GlyphInfoData = (GlyphShader*)malloc(sizeof(GlyphShader) * QFAText::CountGlyphInBuffer);
    }

    Text = text;
    if (Text.size() > 0)
        TextChange = true;

    if (FontHeight == -1)
        SetTextSize(defaultTextSize);
}

void QFAText::SetInputText(char32_t* pText, size_t pTextSize, size_t maxSize)
{
    if (maxSize > CountGlyphInBuffer)
    {
        free(QFAText::GlyphInfoData);
        QFAText::CountGlyphInBuffer = (unsigned int)((float)maxSize * 1.5);
        QFAText::GlyphInfoData = (GlyphShader*)malloc(sizeof(GlyphShader) * QFAText::CountGlyphInBuffer);
    }

    if(pTextSize > 0)
        TextChange = true;

    Text.pText = pText;
    Text.maxSize = maxSize;
    Text.pTextSize = pTextSize;    
    if (FontHeight == -1)
        SetTextSize(defaultTextSize);
}

void QFAText::SetTextSize(unsigned int height)
{
    if (height == 0)
        height = 1;

    if(Text.size() > 0)
        TextChange = true;

    FontHeight = height;
}

void QFAText::SetOverflowWrap(EOverflowWrap wrap)
{
    if (Text.size() > 0)
        TextChange = true;

    OverflowWrap = wrap;
}

void QFAText::SetTextAlign(ETextAlign aligh)
{
    if (Text.size() > 0)
        TextChange = true;

    TextAlign = aligh;
}

void QFAText::StartFrame()
{
    NumberTextInFrame = MinNumberTextInFrame;
    if (OldPipeline)
    {
        delete OldPipeline;
        OldPipeline = nullptr;
    }
}

void QFAText::Render(VkCommandBuffer comandebuffer)
{
    if (NumberTextInFrame >= maxTextInframe)
        CreateTextParameterSet();

    if (TextChange)
        ProcessText();

    CurentDescriptorSet = Pipeline->GetSet(1, NumberTextInFrame);
    updateUniformBuffer();
    
    VkBuffer vertexBuffers[] = { vertexBufer->GpuSideBuffer->Buffer };
    VkDeviceSize offsets[] = { 0 };

    vkCmdBindVertexBuffers(comandebuffer, 0, 1, vertexBuffers, offsets);

    VkDescriptorSet ar[] = { CurentDescriptorSetProject, CurentDescriptorSet };

    vkCmdBindDescriptorSets(comandebuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
        QFAText::Pipeline->GetPipelineLayout(), 0, 2, ar, 0, nullptr);

    vkCmdDraw(comandebuffer, static_cast<uint32_t>(CountSymbolForRender * 6), 1, 0, 0);
    
    NumberTextInFrame++;
    if (Text.penEnable && Text.inputFocus && Text.CanSeePen)
        RenderPen(comandebuffer);
}

void QFAText::RenderPen(VkCommandBuffer comandebuffer)
{
    TextUniformParam.pen = 1;
    textParamBuffers[0]->UpdateData(sizeof(UniformBufferTextParam), &TextUniformParam);// Pen buffer
    textVertexParamBuffers[0]->UpdateData(sizeof(UniformBufferTextParamVertex), &UnitScroll);

    WritePenInfo();
    VkBuffer vertexBuffers[] = { PenVertexBufer->GpuSideBuffer->Buffer };
    VkDeviceSize offsets[] = { 0 };

    vkCmdBindVertexBuffers(comandebuffer, 0, 1, vertexBuffers, offsets);

    VkDescriptorSet ar[] = { CurentDescriptorSetProject, PenDescriptorSet };

    vkCmdBindDescriptorSets(comandebuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
        QFAText::Pipeline->GetPipelineLayout(), 0, 2, ar, 0, nullptr);

    vkCmdDraw(comandebuffer, static_cast<uint32_t>(6), 1, 0, 0);
}

void QFAText::SetSizeParent(unsigned int w, unsigned int h)
{
    if (Text.size() > 0)
        TextChange = true;

    Width = w;
    Height = h;
}

void QFAText::SetPositionParent(int x, int y)
{
    if (Text.size() > 0)
        TextChange = true;

    Position_x = x;
    Position_y = y;
}

void QFAText::updateUniformBuffer()
{
    
    TextUniformParam.textColor = Color;
    TextUniformParam.outlineColor = OutlineColor;
    TextUniformParam.outline = (int)Outline;
    TextUniformParam.opacity = ProcessParentOpacity(Opacity, Parent);

    TextUniformParam.overflow.enable = 0;
    TextUniformParam.overflow.leftTopX = 0;
    TextUniformParam.overflow.leftTopY = 0;
    TextUniformParam.overflow.rightBottomX = 1000000;
    TextUniformParam.overflow.rightBottomY = 1000000;
    
    ProcessParentOverflow(TextUniformParam.overflow, Parent);

    TextUniformParam.pen = 0;
    memcpy(textParamBuffers[NumberTextInFrame]->MapData, &TextUniformParam, sizeof(UniformBufferTextParam));

    UniformBufferTextParamVertex ubtpv = { UnitScroll , UnitOffsetX };
    memcpy(textVertexParamBuffers[NumberTextInFrame]->MapData, &ubtpv, sizeof(UniformBufferTextParamVertex));
}


void QFAText::EndLife()
{
    free(QFAText::GlyphInfoData);
    for (size_t i = 0; i < GlyphAtlasList.size(); i++)
    {
        delete GlyphAtlasList[i].texture;
        delete GlyphAtlasList[i].view;
    }

    delete Pipeline;
    delete AtlassSampler;
    if (OldPipeline)
        delete OldPipeline;

    if (face)
        FT_Done_Face(face);

    if (ft)
        FT_Done_FreeType(ft);
}

void QFAText::RecreatePipeline()
{
    OldPipeline = Pipeline;

    MaxAttlas *= 2;    
    CreatePipeline();    
    vkCmdBindPipeline(QFAWindow::ViewportStuff[QFAWindow::ViewportProcess].comandBuffer.Ui, VK_PIPELINE_BIND_POINT_GRAPHICS, Pipeline->GetPipeline());

    for (size_t i = 0; i < textParamBuffers.size(); i++)
        RecreateTextParameterSet(textParamBuffers[i]->Buffer, textVertexParamBuffers[i]->Buffer);
}

void QFAText::CreateTextProjectionSet(VkBuffer buffer)
{
    VkDescriptorBufferInfo bufferInfo;
    bufferInfo.buffer = buffer;
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(glm::mat4);

    std::array< QFAVKPipeline::QFADescriptorSetInfo, 1> descriptorSetInfo;
    descriptorSetInfo[0].dstBinding = 0;
    descriptorSetInfo[0].DescriptorBufferInfos = &bufferInfo;

    Pipeline->CreateSet(0, descriptorSetInfo.data());
}

void QFAText::CreateTextParameterSet()
{    
    textParamBuffers.push_back( new QFAVKBuffer(sizeof(QFAText::UniformBufferTextParam), nullptr, true, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT));
    textVertexParamBuffers.push_back(new QFAVKBuffer(sizeof(QFAText::UniformBufferTextParamVertex), nullptr, true, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT));

    VkDescriptorBufferInfo BufferInfo;
    BufferInfo.buffer = textParamBuffers.back()->Buffer;
    BufferInfo.offset = 0;
    BufferInfo.range = sizeof(QFAText::UniformBufferTextParam);

    VkDescriptorBufferInfo BufferInfo_Vertex;
    BufferInfo_Vertex.buffer = textVertexParamBuffers.back()->Buffer;
    BufferInfo_Vertex.offset = 0;
    BufferInfo_Vertex.range = sizeof(QFAText::UniformBufferTextParamVertex);

    std::array< QFAVKPipeline::QFADescriptorSetInfo, 3> descriptorSetInfo;
    descriptorSetInfo[0].dstBinding = 0;
    descriptorSetInfo[0].DescriptorImageInfos = QFAText::DII.data();
    descriptorSetInfo[1].dstBinding = 1;
    descriptorSetInfo[1].DescriptorBufferInfos= &BufferInfo;
    descriptorSetInfo[2].dstBinding = 2;
    descriptorSetInfo[2].DescriptorBufferInfos = &BufferInfo_Vertex;

    Pipeline->CreateSet(1, descriptorSetInfo.data());
    maxTextInframe++;  
}

void QFAText::RecreateTextParameterSet(VkBuffer buffer, VkBuffer VertexBuffer)
{
    VkDescriptorBufferInfo BufferInfo;
    BufferInfo.buffer = buffer;
    BufferInfo.offset = 0;
    BufferInfo.range = sizeof(QFAText::UniformBufferTextParam);

    VkDescriptorBufferInfo BufferInfo_Vertex;
    BufferInfo_Vertex.buffer = VertexBuffer;
    BufferInfo_Vertex.offset = 0;
    BufferInfo_Vertex.range = sizeof(QFAText::UniformBufferTextParamVertex);

    std::array< QFAVKPipeline::QFADescriptorSetInfo, 2> descriptorSetInfo;
    descriptorSetInfo[0].dstBinding = 0;
    descriptorSetInfo[0].DescriptorImageInfos = QFAText::DII.data();
    descriptorSetInfo[1].dstBinding = 1;
    descriptorSetInfo[1].DescriptorBufferInfos = &BufferInfo;
    descriptorSetInfo[2].dstBinding = 2;
    descriptorSetInfo[2].DescriptorBufferInfos = &BufferInfo_Vertex;


    Pipeline->CreateSet(1, descriptorSetInfo.data());    
}

void QFAText::GetPenPosition(QFAText::GlyphShader& gs)
{
    QFAText::GlyphShader* ownGlyphInfoData = (QFAText::GlyphShader*)vertexBufer->CpuSideBuffer->MapData;   
    unsigned int penH = 1;
    float yOffset = FontHeight < 25 ? 1 : 0;// if  font have low size need minus 1 to gs.leftTop_1.x
    if (TextMetadata.size() == 0)
    {
        if (TextAlign == TALeft)
            gs.leftTop_1 = { (float)Position_x, (float)Position_y };
        else if (TextAlign == TACenter)
            gs.leftTop_1 = { (float)(Position_x + Width / 2),(float)Position_y };
        else if (TextAlign == TARight)
            gs.leftTop_1 = { (float)(Position_x + Width - 1),(float)Position_y };

        gs.leftBottom_1 = { gs.leftTop_1.x, gs.leftTop_1.y + (Symbols[0].Glyph.HeightMultiplier * (float)FontHeight) };
        gs.rightBottom_1 = { gs.leftBottom_1.x + penH,  (float)gs.leftBottom_1.y };

        gs.leftTop_2 = gs.leftTop_1;
        gs.rightBottom_2 = gs.rightBottom_1;
        gs.rightTop_2 = { gs.rightBottom_1.x, gs.leftTop_1.y };
    }
    else
    {
        if (Text.pen >= TextMetadata.size())
        {
            if (TextAlign == ETextAlign::TARight)
            {
                gs.leftTop_1 = { (float)(Position_x + Width - 1),
                    (Symbols[TextMetadata[Text.pen - 1].symbolIndex].Glyph.HeightMultiplier * (float)FontHeight) * TextMetadata[Text.pen - 1].row + Position_y };
            }
            else
            {
                gs.leftTop_1 = { ownGlyphInfoData[Text.pen - 1].rightBottom_1.x + (Symbols[TextMetadata[Text.pen - 1].symbolIndex].Glyph.bitmap_left * Symbols[TextMetadata[Text.pen - 1].symbolIndex].Glyph.differenceWidth) ,
                                    (Symbols[TextMetadata[Text.pen - 1].symbolIndex].Glyph.HeightMultiplier * (float)FontHeight) * TextMetadata[Text.pen - 1].row + Position_y };
                
            }

            gs.leftBottom_1 = { gs.leftTop_1.x, gs.leftTop_1.y + (Symbols[TextMetadata[Text.pen - 1].symbolIndex].Glyph.HeightMultiplier * (float)FontHeight) };
            gs.rightBottom_1 = { gs.leftBottom_1.x + penH,  (float)gs.leftBottom_1.y };

            gs.leftTop_2 = gs.leftTop_1;
            gs.rightBottom_2 = gs.rightBottom_1;
            gs.rightTop_2 = { gs.rightBottom_1.x, gs.leftTop_1.y };
        }
        else
        {
            float tem = (float)Symbols[TextMetadata[Text.pen].symbolIndex].Glyph.bitmap_left * Symbols[TextMetadata[Text.pen].symbolIndex].Glyph.differenceWidth;// differenceWidth

            gs.leftTop_1 = { ownGlyphInfoData[Text.pen].leftTop_2.x - tem - yOffset, (Symbols[TextMetadata[Text.pen].symbolIndex].Glyph.HeightMultiplier * (float)FontHeight) * TextMetadata[Text.pen].row + Position_y };
            gs.leftBottom_1 = { gs.leftTop_1.x, gs.leftTop_1.y + (Symbols[TextMetadata[Text.pen].symbolIndex].Glyph.HeightMultiplier * (float)FontHeight) };
            gs.rightBottom_1 = { gs.leftBottom_1.x + penH,  (float)gs.leftBottom_1.y };

            gs.leftTop_2 = gs.leftTop_1;
            gs.rightBottom_2 = gs.rightBottom_1;
            gs.rightTop_2 = { gs.rightBottom_1.x, gs.leftTop_1.y };
        }
    }
}

void QFAText::WritePenInfo()
{   
    GlyphShader gs;
    GetPenPosition(gs);
    PenVertexBufer->UpdateData(sizeof(GlyphShader), &gs);
}

void QFAText::CreatePipeline()
{
    QFAVKPipeline::QFAPipelineCreateInfo PipelineInfo;
    PipelineInfo.RenderPass = RenderPass;
    PipelineInfo.PipelineShaderStages.VertexShaderName = U"Text.spvv";
    PipelineInfo.PipelineShaderStages.FragmentShaderName = U"Text.spvf";


    PipelineInfo.VertexInputInfo.VertexBindingDescriptionCount = 1;
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(QFAText::GlyphShaderVertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    PipelineInfo.VertexInputInfo.VertexBindingDescriptions = &bindingDescription;
    

    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
    PipelineInfo.VertexInputInfo.VertexAttributeDescriptionCount = (uint32_t)attributeDescriptions.size();
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = 0;

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(QFAText::GlyphShaderVertex, texture_x);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32_SINT;
    attributeDescriptions[2].offset = offsetof(QFAText::GlyphShaderVertex, AtlasNum);

    PipelineInfo.VertexInputInfo.VertexAttributeDescriptions = attributeDescriptions.data();
    

    PipelineInfo.Rasterization.CullMode = VK_CULL_MODE_NONE;
    std::array< QFAVKPipeline::QFAPipelineColorBlendAttachment, 1> blendAttachments;
    blendAttachments[0].BlendEnable = VK_TRUE;

    PipelineInfo.ColorBlendState.attachmentCount = (uint32_t)blendAttachments.size();
    PipelineInfo.ColorBlendState.pAttachments = blendAttachments.data();
    

    std::array<VkDynamicState, 2> dynamicStates =
    {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    PipelineInfo.DynamicStates = dynamicStates.data();
    PipelineInfo.DynamicStateCount = (uint32_t)dynamicStates.size();


    std::array< QFAVKPipeline::QFADescriptorSetLayout, 2> DescriptorSetLayouts;    
    std::array<VkDescriptorSetLayoutBinding, 1> firsLayout;
    firsLayout[0].binding = 0;
    firsLayout[0].descriptorCount = 1;
    firsLayout[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    firsLayout[0].pImmutableSamplers = nullptr;
    firsLayout[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    
    std::array<VkDescriptorSetLayoutBinding, 3> secondLayout;
    secondLayout[0].binding = 0;
    secondLayout[0].descriptorCount = QFAText::MaxAttlas;
    secondLayout[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    secondLayout[0].pImmutableSamplers = nullptr;
    secondLayout[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    secondLayout[1].binding = 1;
    secondLayout[1].descriptorCount = 1;
    secondLayout[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    secondLayout[1].pImmutableSamplers = nullptr;
    secondLayout[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;    
    secondLayout[2].binding = 2;
    secondLayout[2].descriptorCount = 1;
    secondLayout[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    secondLayout[2].pImmutableSamplers = nullptr;
    secondLayout[2].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    DescriptorSetLayouts[0].BindingCount = (uint32_t)firsLayout.size();
    DescriptorSetLayouts[1].BindingCount = (uint32_t)secondLayout.size();
    DescriptorSetLayouts[0].Bindings = firsLayout.data();
    DescriptorSetLayouts[1].Bindings = secondLayout.data();

    PipelineInfo.DescriptorSetLayoutCount = (uint32_t)DescriptorSetLayouts.size();
    PipelineInfo.DescriptorSetLayouts = DescriptorSetLayouts.data();


    PipelineInfo.DepthStencil.DepthTestEnable = VK_TRUE;
    PipelineInfo.DepthStencil.DepthWriteEnable = VK_TRUE;
    PipelineInfo.DepthStencil.DepthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
 

    std::array< uint32_t, 2> MaxSets;
    MaxSets[0] = 10; // this set for viewport
    MaxSets[1] = QFAText::AmountSetsInTextParamPool;
    PipelineInfo.MaxSets = MaxSets.data();

    
    Pipeline = new QFAVKPipeline(PipelineInfo);
    /*------*/    

    GlyphAtlasList.resize(MaxAttlas);
    DII.resize(MaxAttlas);
    for (int i = 0; i < (int)MaxAttlas; i++) // create atlas if need
    {
        if (GlyphAtlasList[i].atlasIndex >= 0)
            continue;

        QFAImage::SImageCreateInfo ici;
        ici.Width = GlyphAtlasWidth;
        ici.Height = GlyphAtlasHeight;
        ici.channelCount = 1;
        ici.format = VK_FORMAT_R8_UNORM;
        ici.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        SGlyphAtlas gi;
        gi.texture = new QFAImage(ici);
        gi.view = new QFAVKImageView(gi.texture);
        gi.atlasIndex = i;

        DII[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        DII[i].imageView = gi.view->ImageView;
        DII[i].sampler = AtlassSampler->textureSampler;
        GlyphAtlasList[i] = gi;      
    }
}