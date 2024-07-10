#pragma once

#include <vector>

#include <shaderc/shaderc.h>

#include <filesystem>
#include <EngineStuff/Pipline/Pipline.h>
//
class QFAEditorOverlord;
class QFAShaderCompiler
{
	friend QFAEditorOverlord;

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

	shaderc_compiler_t Compiler;
	ShaderListStart DefaultList;
	std::vector<QFAVKPipeline::SShaderData> ShaderData;
	std::vector<shaderc_compilation_result_t> ShadercCompilationResult;

	QFAShaderCompiler();
	~QFAShaderCompiler();

	void ProcessShaders();

	void EndLife()
	{
		for (size_t i = 0; i < ShadercCompilationResult.size(); i++)
			shaderc_result_release(ShadercCompilationResult[i]);

		shaderc_compiler_release(Compiler);
	}

	void Compile(std::u32string filePath, shaderc_shader_kind kind, int indexInList, std::vector<ShaderPath>& sp, std::filesystem::path compiledFilePath);
};
