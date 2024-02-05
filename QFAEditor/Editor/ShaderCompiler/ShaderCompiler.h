#pragma once

#include <vector>

#include <shaderc/shaderc.h>

#include <filesystem>
#include <Render/Pipline/Pipline.h>

class QFAEditorOverlord;
class QFAShaderCompiler
{
	friend QFAEditorOverlord;
	shaderc_compiler_t Compiler;

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

	ShaderListStart DefaultList;
	struct ShaderPath
	{
		std::u32string path;
		size_t writeTime;
	};


	void ProcessShaders();

	QFAShaderCompiler();
	~QFAShaderCompiler();

	void EndLife()
	{
		for (size_t i = 0; i < ShadercCompilationResult.size(); i++)
			shaderc_result_release(ShadercCompilationResult[i]);

		shaderc_compiler_release(Compiler);
	}
	
	std::vector<QFAVKPipeline::SShaderData> ShaderData;
private:


	std::vector< shaderc_compilation_result_t> ShadercCompilationResult;

	void Compile(std::u32string filePath, shaderc_shader_kind kind, int indexInList, std::vector<ShaderPath>& sp, std::filesystem::path compiledFilePath);
};
