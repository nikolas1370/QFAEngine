#pragma once

#include <vector>

#include <shaderc/shaderc.h>

#include <filesystem>
#include <Core/EngineStuff/Pipline/Pipline.h>
class QFAOverlord;
class QFAShaderCompiler
{
	friend QFAOverlord;
	/*
	ShaderList structure (in file)
		char[21] file type;
		size_t	 shader file count;

		[
			size_t last write time;
			size_t count char32_t
			char32_t[] shader file name;
		]
	*/
	struct ShaderListStart
	{
		const char title[21] = "QFAEditor ShaderList";
		size_t shaderCount = 0;
	};

	struct ShaderPath
	{
		std::u32string path;
		size_t writeTime;
	};	

	static shaderc_compiler_t Compiler;
	static ShaderListStart DefaultList;

	static std::vector<shaderc_compilation_result_t> ShadercCompilationResult;

	QFAShaderCompiler();
	~QFAShaderCompiler();

	//shaderFilderPath path when store text of shader
	static void ProcessShaders(const std::u32string & shaderFolderPath);

	static void EndLife()
	{
		for (size_t i = 0; i < ShadercCompilationResult.size(); i++)
			shaderc_result_release(ShadercCompilationResult[i]);

		shaderc_compiler_release(Compiler);
	}

	static void Compile(std::u32string filePath, shaderc_shader_kind kind, int indexInList, std::vector<ShaderPath>& sp, std::filesystem::path compiledFilePath);
};
