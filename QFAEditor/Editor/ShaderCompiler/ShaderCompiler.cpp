#include "ShaderCompiler.h"
#include <Tools/File/FileSystem.h>
#include <iostream>
#include <Tools/VulkanSuff.h>
#include <Render/Time.h>



QFAShaderCompiler::QFAShaderCompiler()
{
   
}

QFAShaderCompiler::~QFAShaderCompiler()
{
}

void QFAShaderCompiler::ProcessShaders()
{
    if (!QFAFileSystem::exists(U"Editor/shaders/src/"))
        QFAFileSystem::CreateFolders(U"Editor/shaders/src/");

    if (!QFAFileSystem::exists(U"Editor/shaders/spv/"))
        QFAFileSystem::CreateFolders(U"Editor/shaders/spv/");

    ShaderListStart* sl = nullptr;
    void* shaders = nullptr;

    QFAFile file;

    std::filesystem::path shaderRootFolder = "Editor/shaders/";
    std::filesystem::path shaderListPath = std::filesystem::path(shaderRootFolder).append(U"ShaderList.qfa");

    QFAFileSystem::LoadFile(shaderListPath.u32string(), &file);
    if (file.GetStatus() == QFAFile::Result::FileNotExists)
        sl = &DefaultList;
    else
    {
        sl = (ShaderListStart*)file.GetData();
        shaders = &sl[1];
    }

    std::vector<ShaderPath> sp;
    sp.resize(sl->shaderCount);

    if (sl->shaderCount > 0)
    {
        char* pwritet = (char*)shaders;
        for (size_t i = 0; i < sl->shaderCount; i++)
        {
            sp[i].writeTime = *((size_t*)pwritet);
            pwritet += sizeof(size_t);

            size_t size = *((size_t*)pwritet);
            pwritet += sizeof(size_t);

            sp[i].path = std::u32string((char32_t*)pwritet, size);
            pwritet += size * sizeof(char32_t);
        }
    }

    bool needUpdateList = false;
    std::vector<std::u32string> filePaths;
    if (QFAFileSystem::GetFileList(U"Editor/shaders/src", filePaths))
    {
        for (size_t i = 0; i < filePaths.size(); i++)
        {
            shaderc_shader_kind kind;
            if ((QFAFileSystem::GetExtension(filePaths[i]) == std::u32string(U".vert")))
                kind = shaderc_glsl_default_vertex_shader;
            else if (QFAFileSystem::GetExtension(filePaths[i]) == std::u32string(U".frag"))
                kind = shaderc_glsl_default_fragment_shader;
            else // if file not shader 
                continue;

            bool needCompile = true;
            int indexInList = -1; // -1 == need add file in list

            size_t last = QFAFileSystem::LastWriteTime(filePaths[i]);
            for (int j = 0; j < (int)sp.size(); j++)
            {
                if (sp[j].path == filePaths[i])
                {
                    indexInList = j;
                    if (last == sp[j].writeTime)
                        needCompile = false;

                    break;
                }
            }

            std::filesystem::path compFileName = filePaths[i];
            switch (kind)
            {
            case shaderc_glsl_default_vertex_shader:
                compFileName.replace_extension("spvv");
                break;
            case shaderc_glsl_default_fragment_shader:
                compFileName.replace_extension("spvf");
                break;
            default:
                break;
            }

            compFileName = std::u32string(shaderRootFolder.u32string()).append(U"spv/").append(compFileName.filename().u32string());

            if (needCompile)
            {
                Compile(filePaths[i], kind, indexInList, sp, compFileName);
                needUpdateList = true;
            }
            else
            {
                if (QFAFileSystem::exists(compFileName.u32string()))
                {
                    QFAVKPipeline::SShaderData sh;
                    sh.loadFromFile = true;
                    sh.path = compFileName.u32string();
                    sh.name = compFileName.filename().u32string();
                    ShaderData.push_back(sh);
                }
                else
                {
                    Compile(filePaths[i], kind, indexInList, sp, compFileName);
                    needUpdateList = true;
                }
            }
        }
    }


    if (needUpdateList)
    {
        sl->shaderCount = sp.size();
        QFAFileSystem::WriteFile(shaderListPath.u32string(), sl, sizeof(ShaderListStart));

        QFAFile file;
        QFAFileSystem::OpenFile(shaderListPath.u32string(), &file, false);
        for (size_t i = 0; i < sp.size(); i++)
        {

            size_t size[2] = { sp[i].writeTime, sp[i].path.size() };
            QFAFileSystem::AppendFile(&file, &size, sizeof(size));
            QFAFileSystem::AppendFile(&file, (void*)sp[i].path.c_str(), sp[i].path.size() * sizeof(char32_t));
        }

        QFAFileSystem::CloseFile(&file);
    }
}

void QFAShaderCompiler::Compile(std::u32string filePath, shaderc_shader_kind kind, int indexInList, std::vector<ShaderPath>& sp, std::filesystem::path compiledFilePath)
{
    QFAFile file;
    file.DisableAutoDeleteFileData();

    QFAFileSystem::LoadFile(filePath, &file);

    if(!Compiler)
        Compiler = shaderc_compiler_initialize();

    auto lop = QTime::GetSystemTime();

    shaderc_compilation_result_t shaderRes = shaderc_compile_into_spv(Compiler, (const char*)file.GetData(), file.GetDataSize(), kind, (char*)std::filesystem::path(filePath).filename().c_str(), "main", nullptr);
    if (shaderc_result_get_num_errors(shaderRes))
        stopExecute(shaderc_result_get_error_message(shaderRes))
    
    ShadercCompilationResult.push_back(shaderRes);
    std::wcout << "Shader " << std::filesystem::path(filePath).filename().wstring() << " was compiled. " << "Time " << (QTime::GetSystemTime() - lop) / 10000 <<  "ms\n";

    QFAVKPipeline::SShaderData sh;
    sh.loadFromFile = false;
    sh.name = compiledFilePath.filename().u32string();
    sh.module = (uint32_t*)shaderc_result_get_bytes(shaderRes);
    sh.moduleSize = shaderc_result_get_length(shaderRes);    
    ShaderData.push_back(sh);
    
        
    if (indexInList == -1)
        sp.push_back(ShaderPath(filePath, QFAFileSystem::LastWriteTime(filePath)));
    else
        sp[indexInList].writeTime = QFAFileSystem::LastWriteTime(filePath); 


    QFAFileSystem::WriteFile(compiledFilePath.u32string(), (void*)sh.module, sh.moduleSize);
}