#include "Text.h"
#include <iostream>
#include <Render/vk/LogicalDevice.h>
#include <Render/vk/TextureImage.h>
#include <Render/Buffer/VertexBuffer.h>
#include <Render/Pipline/TextPipeline.h>
#include <Render/vk/ImageView.h>
#include <Render/vk/TextureSampler.h>
#include <Render/Window/Window.h>
#include <Render/UI/UIParentComponent.h>
#include <ft2build.h>
#include FT_FREETYPE_H  

FT_Library QFAText::ft;
FT_Face QFAText::face;
unsigned int QFAText::CountGlyphInBuffer = 100;
QFAText::GlyphShader* QFAText::GlyphInfoData;
std::vector<QFAText::SGlyphAtlas> QFAText::GlyphAtlasList;
QFAArray<QFAText::PrepareData> QFAText::SymbolsForRender;

VkDescriptorPool QFAText::TextProjectPoolOld;

std::array<QFAText::STextProjectSet, QFAWindow::MaxActiveViewPort> QFAText::ViewportsUIProjectOld;// delete
/*----*/


#pragma comment(lib, "freetype.lib")
/*----*/

QFAVKTextPipeline* QFAText::Pipeline = nullptr;
QFAVKTextPipeline* QFAText::OldPipeline = nullptr;
const std::string QFAText::VertexShaderPath = "Engine/Shaders/TextVertex.spv";
const std::string QFAText::FragmentShaderPath = "Engine/Shaders/TextFragment.spv";


std::vector<QFAText::STextSet> QFAText::textParamSetsOld;
std::vector<VkDescriptorPool> QFAText::TextParamPoolsOld;// descriptorPools;


QFAArray<QFAText::Symbol> QFAText::Symbols;




VkCommandPool QFAText::commandPool;
unsigned int QFAText::MaxAttlas = 10;

std::vector<VkDescriptorImageInfo> QFAText::DII;
VkRenderPass QFAText::RenderPass;
QFAVKTextureSampler* QFAText::AtlassSampler;


int QFAText::maxTextInframe = QFAText::AmountSetsInPool;

std::array<QFAText::STextProjectSet, QFAWindow::MaxActiveViewPort> QFAText::ViewportsUIProject;
VkDescriptorPool QFAText::TextProjectPool;

VkDescriptorSet QFAText::CurentDescriptorSet;
VkDescriptorSet QFAText::CurentDescriptorSetProject;

std::vector<VkDescriptorPool> QFAText::TextParamPools;
std::vector<QFAText::STextSet> QFAText::textParamSets;

int QFAText::NumberTextInFrame;

struct UniformBufferObject
{// more detail https://fvcaputo.github.io/2019/02/06/memory-alignment.html
    /*
        An array or structure type has an extended
        alignment equal to the largest extended alignment of any of
        its members, rounded up to a multiple of 16.
    */
    alignas(16) glm::mat4 projection;
};

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

    CreatePiline();

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

    QFAText::GlyphInfoData = (GlyphShader*)malloc(sizeof(GlyphShader) * QFAText::CountGlyphInBuffer);
}

QFAText ::~QFAText()
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

    if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_SDF))
        std::cout << "ERORRRRRRR" << std::endl;

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
            CreateAtlas();
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
            else if (x + glyphBitmap_left + glyphWidth >= Width)
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
        else if (OverflowWrap == EOverflowWrap::OWWord)
        {
            wordW += (int)((float)Symbols[symI].Glyph.advance_x * tem);
            if (wordLen++ == 0)
                wordStart = (unsigned int)i;// in current time  all symbol in Text be presend in SymbolsForRender                         

            QFAText::SymbolsForRender.Add(PrepareData{ (unsigned int)symI });
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
                else if (x + wordW <= Width)
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
        delete vertexBufer;
        vertexBufer = new QFAVKVertexBuffer(sizeof(GlyphShader) * CountGlyphInGUP, GlyphInfoData, commandPool);
    }
    else
        vertexBufer->UpdateData(sizeof(GlyphShader) * CountSymbolForRender, GlyphInfoData);

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
            if (row != QFAText::SymbolsForRender[i].row)
            {
                row = QFAText::SymbolsForRender[i].row;
                w = 0;
            }

            GlyphInfo* gi = &Symbols[QFAText::SymbolsForRender[i].symbolIndex].Glyph;

            float temX = ((float)Position_x + (float)w + (float)gi->bitmap_left * tem);
            float temXEnd = temX + (float)gi->width * tem;

            float start_y = (float)Position_y + ((float)FontHeight * gi->HeightMultiplier - (float)FontHeight) + ((float)row * ((float)FontHeight * gi->HeightMultiplier));
            //float temY = start_y + ((float)gi->MaxAscender - gi->bitmap_top * tem); 
            float temY = start_y + (float)(gi->MaxAscender - gi->bitmap_top) * tem;
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
    else if (TextAlign == ETextAlign::TACenter)
    {
        int rowLen = 0;
        for (size_t startRow = 0; startRow < CountSymbolForRender; startRow += rowLen)
        {
            int row = QFAText::SymbolsForRender[startRow].row;
            int rowW = 0;
            rowLen = 0;
            for (size_t i = startRow; i < CountSymbolForRender; i++)
            {
                if (QFAText::SymbolsForRender[i].row != row)
                {
                    if ((Symbols[QFAText::SymbolsForRender[i - 1].symbolIndex].symbol == L' '))
                        rowW -= (int)((float)Symbols[QFAText::SymbolsForRender[i - 1].symbolIndex].Glyph.advance_x * tem) - 1;

                    break;
                }

                rowLen++;
                rowW += (int)((float)Symbols[QFAText::SymbolsForRender[i].symbolIndex].Glyph.advance_x * tem);
            }

            w = ((int)Width - rowW) / 2;// rowW can be larger Width
            for (size_t i = startRow; i < startRow + rowLen; i++)
            {
                GlyphInfo* gi = &Symbols[QFAText::SymbolsForRender[i].symbolIndex].Glyph;

                float temX = ((float)Position_x + (float)w + (float)gi->bitmap_left * tem);
                float temXEnd = temX + (float)gi->width * tem;

                float start_y = (float)Position_y + ((float)FontHeight * gi->HeightMultiplier - (float)FontHeight) + ((float)row * ((float)FontHeight * gi->HeightMultiplier));
                float temY = start_y + (float)(gi->MaxAscender - gi->bitmap_top) * tem;
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
    }
    else if (TextAlign == ETextAlign::TARight)
    {// do revers
        int statrtFor = 0;
        if (Symbols[QFAText::SymbolsForRender[CountSymbolForRender - 1].symbolIndex].symbol == L' ')
            CountSymbolForRender--;// if last symbol space(' ') symbol past

        row = QFAText::SymbolsForRender[CountSymbolForRender - 1].row;
        w = Width;
        for (int i = CountSymbolForRender - 1; i >= 0; i--)
        {
            if (row != QFAText::SymbolsForRender[i].row)
            {
                row = QFAText::SymbolsForRender[i].row;
                w = Width;
            }

            if (Symbols[QFAText::SymbolsForRender[i].symbolIndex].symbol == L' ' && QFAText::SymbolsForRender[i].row != QFAText::SymbolsForRender[i + 1].row)
            {// if last symbol in row be space(' ') symbol have all parameters 0                
                QFAText::GlyphInfoData[i].leftBottom_1 = GlyphShaderVertex{ 0, 0,   0, 0, 0 };
                GlyphInfoData[i].rightBottom_1 = { 0, 0,   0, 0, 0 };
                GlyphInfoData[i].rightTop_1 = { 0, 0,   0, 0, 0 };

                GlyphInfoData[i].leftBottom_2 = GlyphInfoData[i].leftBottom_1;
                GlyphInfoData[i].rightTop_2 = GlyphInfoData[i].rightTop_1;
                GlyphInfoData[i].leftTop_2 = GlyphShaderVertex{ 0, 0,   0, 0, 0 };
                GlyphInfo* temGi = &Symbols[QFAText::SymbolsForRender[i].symbolIndex].Glyph;

                w += (int)((float)temGi->width * tem - 1.0f);
                continue;
            }

            GlyphInfo* gi = &Symbols[QFAText::SymbolsForRender[i].symbolIndex].Glyph;

            float temX = ((float)Position_x + (float)w - (float)gi->advance_x * tem + (float)gi->bitmap_left * tem);
            float temXEnd = temX + (float)gi->width * tem;

            float start_y = (float)Position_y + ((float)FontHeight * gi->HeightMultiplier - (float)FontHeight) + ((float)row * ((float)FontHeight * gi->HeightMultiplier));
            float temY = start_y + (float)(gi->MaxAscender - gi->bitmap_top) * tem;
            float temYEnd = temY + (float)gi->height * tem;

            QFAText::GlyphInfoData[i].leftBottom_1 = GlyphShaderVertex{ temX, temY ,   gi->x, gi->y, (int)gi->atlasIndex };
            GlyphInfoData[i].rightBottom_1 = { temX, temYEnd,   gi->x, gi->yEnd, (int)gi->atlasIndex };
            GlyphInfoData[i].rightTop_1 = { temXEnd, temYEnd,   gi->xEnd, gi->yEnd, (int)gi->atlasIndex };

            GlyphInfoData[i].leftBottom_2 = GlyphInfoData[i].leftBottom_1;
            GlyphInfoData[i].rightTop_2 = GlyphInfoData[i].rightTop_1;
            GlyphInfoData[i].leftTop_2 = GlyphShaderVertex{ temXEnd, temY,   gi->xEnd, gi->y, (int)gi->atlasIndex };

            w -= (int)((float)gi->advance_x * tem);
        }
    }
}

void QFAText::SetPosition(unsigned int x, unsigned int y)
{
    if (ParentViewport)
        SetPositionParent(x, y);
}

void QFAText::SetSize(unsigned int w, unsigned int h)
{
    if (ParentViewport)
        SetSizeParent(w, h);
}

void QFAText::Destroy()
{
    delete this;
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


void QFAText::SetTextAlign(ETextAlign aligh)
{
    TextChange = true;
    TextAlign = aligh;
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

void QFAText::RecreateCreatePiline()
{
    OldPipeline = Pipeline;
    Pipeline = new QFAVKTextPipeline(RenderPass, QFAText::VertexShaderPath, QFAText::FragmentShaderPath);
    CreateTextProjectionPool();
    CreateTextParameterPool(false);
}

void QFAText::CreatePiline()
{
    Pipeline = new QFAVKTextPipeline(RenderPass, QFAText::VertexShaderPath, QFAText::FragmentShaderPath);
    CreateTextProjectionPool();
    CreateTextParameterPool(true);
}

void QFAText::CreateTextProjectionPool()
{
    TextProjectPoolOld = TextProjectPool;
    ViewportsUIProjectOld = ViewportsUIProject;

    std::array<VkDescriptorPoolSize, 1> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = 1;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = ViewportsUIProject.size();

    TextProjectPool = nullptr;
    if (vkCreateDescriptorPool(QFAVKLogicalDevice::GetDevice(), &poolInfo, nullptr, &TextProjectPool) != VK_SUCCESS)
        stopExecute("failed to create descriptor pool!");

    for (size_t i = 0; i < ViewportsUIProject.size(); i++)
    {
        if (ViewportsUIProject[i].buffer == nullptr)
            ViewportsUIProject[i].buffer = new QFAVKBuffer(sizeof(glm::mat4), nullptr, true, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = TextProjectPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &Pipeline->descriptorSetLayouts[0];

        if (vkAllocateDescriptorSets(QFAVKLogicalDevice::GetDevice(), &allocInfo, &ViewportsUIProject[i].set) != VK_SUCCESS)
            stopExecute("failed to allocate descriptor sets!");

        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = ViewportsUIProject[i].buffer->Buffer;
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(glm::mat4);

        std::array<VkWriteDescriptorSet, 1> descriptorWrites{};
        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = ViewportsUIProject[i].set;
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(QFAVKLogicalDevice::GetDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}

void QFAText::CreateTextParameterPool(bool addPool)
{
    if (addPool)
    {
        TextParamPools.resize(TextParamPools.size() + 1);
        textParamSets.resize(textParamSets.size() + AmountSetsInPool);
    }
    else
    {
        TextParamPoolsOld = TextParamPools;
        textParamSetsOld = textParamSets;
    }

    GlyphAtlasList.resize(MaxAttlas);
    DII.resize(MaxAttlas);
    for (size_t i = 0; i < MaxAttlas; i++) // create atlas if need
    {
        if (GlyphAtlasList[i].atlasIndex >= 0)
            continue;

        QFAVKTextureImage::SImageCreateInfo ici;
        ici.Width = GlyphAtlasWidth;
        ici.Height = GlyphAtlasHeight;
        ici.channelCount = 1;
        ici.format = VK_FORMAT_R8_UNORM;
        ici.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        SGlyphAtlas gi;
        gi.texture = new QFAVKTextureImage(commandPool, ici);
        gi.view = new QFAVKImageView(gi.texture);
        gi.atlasIndex = i;

        DII[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        DII[i].imageView = gi.view->ImageView;
        DII[i].sampler = AtlassSampler->textureSampler;
        GlyphAtlasList[i] = gi;
    }

    for (size_t i = (addPool ? TextParamPools.size() - 1 : 0); i < TextParamPools.size(); i++)
    {
        std::array<VkDescriptorPoolSize, 2> poolSizesSet2{};
        poolSizesSet2[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizesSet2[0].descriptorCount = QFAText::MaxAttlas;
        poolSizesSet2[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizesSet2[1].descriptorCount = 1;

        VkDescriptorPoolCreateInfo poolInfoSet2{};
        poolInfoSet2.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfoSet2.poolSizeCount = static_cast<uint32_t>(poolSizesSet2.size());
        poolInfoSet2.pPoolSizes = poolSizesSet2.data();
        poolInfoSet2.maxSets = QFAText::AmountSetsInPool;

        TextParamPools[i] = nullptr;
        if (vkCreateDescriptorPool(QFAVKLogicalDevice::GetDevice(), &poolInfoSet2, nullptr, &TextParamPools[i]))
            stopExecute("failed to create descriptor pool!");

        for (size_t j = i * AmountSetsInPool; j < textParamSets.size(); j++)
        {
            VkDescriptorSetAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.descriptorPool = TextParamPools[i];
            allocInfo.descriptorSetCount = 1;
            allocInfo.pSetLayouts = &Pipeline->descriptorSetLayouts[1];


            if (VkResult res = vkAllocateDescriptorSets(QFAVKLogicalDevice::GetDevice(), &allocInfo, &textParamSets[j].set))
                stopExecute("failed to allocate descriptor sets!", res);

            if (textParamSets[j].textParametrBuffer == nullptr)
                textParamSets[j].textParametrBuffer = new QFAVKBuffer(sizeof(QFAText::UniformBufferTextParam), nullptr, true, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

            VkDescriptorBufferInfo bufferInfo2{};
            bufferInfo2.buffer = textParamSets[j].textParametrBuffer->Buffer;
            bufferInfo2.offset = 0;
            bufferInfo2.range = sizeof(QFAText::UniformBufferTextParam);

            std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = textParamSets[j].set;
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[0].descriptorCount = QFAText::MaxAttlas;
            descriptorWrites[0].pImageInfo = QFAText::DII.data();

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = textParamSets[j].set;
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pBufferInfo = &bufferInfo2;

            vkUpdateDescriptorSets(QFAVKLogicalDevice::GetDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }
    }
}

void QFAText::CreateAtlas()
{
    if (GlyphAtlasList.size() >= MaxAttlas)
    {
        MaxAttlas *= 2;
        RecreateCreatePiline();
        vkCmdBindPipeline(QFAWindow::MainWindow->UICommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, Pipeline->graphicsPipeline);
    }
}

void QFAText::StartTextRender()
{
    NumberTextInFrame = 0;
    if (OldPipeline)
    {
        for (size_t i = 0; i < TextParamPoolsOld.size(); i++)
            vkDestroyDescriptorPool(QFAVKLogicalDevice::GetDevice(), TextParamPoolsOld[i], nullptr);

        vkDestroyDescriptorPool(QFAVKLogicalDevice::GetDevice(), TextProjectPoolOld, nullptr);
        delete OldPipeline;
        OldPipeline = nullptr;
    }
}

void QFAText::StartTextRenderViewPort(const glm::mat4& proj, unsigned int viewportIndex)
{
    CurentDescriptorSetProject = ViewportsUIProject[viewportIndex].set;

    UniformBufferObject ubo{};
    ubo.projection = proj;
    memcpy(ViewportsUIProject[viewportIndex].buffer->MapData, &ubo, sizeof(ubo.projection));
}

void QFAText::Render(VkCommandBuffer comandebuffer)
{
    if (TextChange)
        ProcessText();

    if (NumberTextInFrame >= maxTextInframe)
    {
        maxTextInframe += AmountSetsInPool;
        CreateTextParameterPool(true);
    }

    CurentDescriptorSet = textParamSets[NumberTextInFrame].set;
    updateUniformBuffer();

    /*----*/

    VkBuffer vertexBuffers[] = { vertexBufer->GpuSideBuffer->Buffer };
    VkDeviceSize offsets[] = { 0 };

    vkCmdBindVertexBuffers(comandebuffer, 0, 1, vertexBuffers, offsets);

    VkDescriptorSet ar[] = { CurentDescriptorSetProject, CurentDescriptorSet };

    vkCmdBindDescriptorSets(comandebuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
        QFAText::Pipeline->pipelineLayout, 0, 2, ar, 0, nullptr);

    vkCmdDraw(comandebuffer, static_cast<uint32_t>(CountSymbolForRender * 6), 1, 0, 0);

    /*----*/

    NumberTextInFrame++;
}

void QFAText::SetSizeParent(unsigned int w, unsigned int h)
{
    TextChange = true;
    Width = w;
    Height = h;
}

void QFAText::SetPositionParent(unsigned int x, unsigned int y)
{
    TextChange = true;
    Position_x = x;
    Position_y = y;
}



void QFAText::updateUniformBuffer()
{
    UniformBufferTextParam tem;
    tem.textColor = Color;
    tem.outlineColor = OutlineColor;
    tem.outline = (int)Outline;
    tem.opacity = Opacity;
    memcpy(textParamSets[NumberTextInFrame].textParametrBuffer->MapData, &tem, sizeof(UniformBufferTextParam));
}






