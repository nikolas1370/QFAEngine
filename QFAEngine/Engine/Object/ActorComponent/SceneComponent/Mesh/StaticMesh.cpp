#include "StaticMesh.h"
#include <Tools/Debug/OpenGlStuff.h>

QFAShaderProgram* QStaticMesh::SShaderProgram = nullptr;
QFAShaderProgram* QStaticMesh::SShaderShadowProgram = nullptr;
std::string QStaticMesh::VertexShaderPath = "Engine/Shaders/VertexShader.shader";
std::string QStaticMesh::FragmentShaderPath = "Engine/Shaders/PixelShader.shader";

std::string QStaticMesh::VertexShadowShaderPath = "Engine/Shaders/DirectionLightShadow/Vertex.shader";
std::string QStaticMesh::FragmentShadowShaderPath = "Engine/Shaders/DirectionLightShadow/Fragment.shader";


QStaticMesh::QStaticMesh(MeshFrames* meshFrames)
{
	SetMesh(meshFrames);
}

void QStaticMesh::SetMesh(MeshFrames* meshFrames) 
{
	//last MeshFrames need delete
	if (!SShaderProgram)
	{
		SShaderProgram = new QFAShaderProgram(VertexShaderPath, FragmentShaderPath);
		SShaderShadowProgram = new QFAShaderProgram(VertexShadowShaderPath, FragmentShadowShaderPath);
	}
	
	MFs = meshFrames;

	if (!Init)
	{
		Init = true;

		GLCall(glGenVertexArrays(1, &VAO)); // for othe glVertexAttribPointer need create new vertex array object
		GLCall(glBindVertexArray(VAO));

		GLCall(glGenBuffers(1, &VBO));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));// bind for future render(glDrawElements)
		// next is paraters in vertex shader // what order data in
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SSVertexMaterial), (void*)offsetof(SSVertexMaterial, Position));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SSVertexMaterial), (void*)offsetof(SSVertexMaterial, Normal));
		glEnableVertexAttribArray(2);
		glVertexAttribIPointer(2, 1, GL_INT, sizeof(SSVertexMaterial), (void*)offsetof(SSVertexMaterial, materialIndex));

		glGenBuffers(1, &IBO); // gen index buffer
	}
	else
	{
		GLCall(glBindVertexArray(VAO));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));// bind for future render(glDrawElements)
		
	}

	/*
	some time crass program
	Exception thrown at 0x00007FFBF6D7003B (nvoglv64.dll) in superCode.exe: 0xC0000005: Access violation reading location 
	
	here problem
	new QStaticMesh(LoadModel(path, 0));
	*/

	// both for debug
	SSVertexMaterial* xc = MFs->GetVerticesDate();
	if (!xc)
		__debugbreak();

	size_t zx = MFs->GetVerticesSize();
	1 + 1;
	GLCall(glBufferData(GL_ARRAY_BUFFER, zx, xc, GL_STATIC_DRAW));
	//  glBufferSubData,   modify data
	/*   IndexBuffer   */ 

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, MFs->GetIndexCount() * sizeof(unsigned int), MFs->GetIndexData(), GL_STATIC_DRAW);// DYNAMIC
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void QStaticMesh::Bind( bool isShadow )
{
	if (isShadow)
	{
		SShaderShadowProgram->Use();
	}
	else
	{
		SShaderProgram->Use();
		SShaderProgram->SetMaterials(MFs->GetMaterialData(), MFs->GetMaterialCount());
	}

	GLCall(glBindVertexArray(VAO));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
}

int QStaticMesh::GetIndexCount()
{
	return MFs->GetIndexCount();
}

// be rebuild
QStaticMesh* QStaticMesh::GetCopyMesh()
{
	return new QStaticMesh(MFs);
}

