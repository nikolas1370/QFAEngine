#include "Mesh.h"

ShaderProgram* QMesh::SShaderProgram = nullptr;
ShaderProgram* QMesh::SShaderShadowProgram = nullptr;
std::string QMesh::VertexShaderPath = "Engine/Shaders/VertexShaderAnimInterpolation.shader";
std::string QMesh::FragmentShaderPath = "Engine/Shaders/PixelShader.shader";


std::string QMesh::VertexShaderShaderPath = "Engine/Shaders/DirectionLightShadow/VertexInterpolation.shader";
std::string QMesh::FragmentShaderShaderPath = "Engine/Shaders/DirectionLightShadow/Fragment.shader";

/*-----*/
void QMesh::StartFrame()
{
	if (!Played)
		return;
	//csacsa


	double deltaTime = QTime::GetDeltaTime() ;

	PlayTime += deltaTime;
	if (PlayTime >= Mf->GetAnimationTime())
	{		
		if (!Repeat)
		{
			curentFrame = Mf->GetFrameCount();
			PlayTime = Mf->GetAnimationTime() + 1.0;
			Played = false;
			return;
		}
	}

	if (Mf->GetFrameTime() >= deltaTime)// offset frame not more 1// fps in scren more than in animation
	{		
		double nextFrameTime = Mf->GetFrameTime() * curentFrame;
		double remainderToEndFrame = nextFrameTime - PlayTime;				
		if (PlayTime >= nextFrameTime)
		{
			remainderToEndFrame = Mf->GetFrameTime() + remainderToEndFrame;
			if (++curentFrame > Mf->GetFrameCount())
			{
				curentFrame = 1;				
				PlayTime = Mf->GetFrameTime() - remainderToEndFrame;
			}
		}		
		
		InterpolationTime = (float)(1.0 - remainderToEndFrame / Mf->GetFrameTime());// convert percent before end frame TO percent from start frame
	}   
	else// not use Interpolation if screen fps less than animation fps
	{
		InterpolationTime = 0;
		if (PlayTime > Mf->GetAnimationTime())
		{
			PlayTime = PlayTime - Mf->GetAnimationTime();
			curentFrame = (int)floor(PlayTime / Mf->GetFrameTime()) + 1;
		}
		else
		{
			double remainder = PlayTime - curentFrame * Mf->GetFrameTime();
			curentFrame += (int)floor(remainder / Mf->GetFrameTime());
		}		
	}
}

void QMesh::Play()
{
	if (Played)
		return;

	if (PlayTime >= Mf->GetAnimationTime())
		PlayTime = 0;

	Played = true;
}

void QMesh::Stop()
{
	Played = false;
// size_t
}


void QMesh::Bind( bool isShadow)//  isShadow ���� ����
{	
	// be in other plase
	StartFrame();

	// remove
	if (isShadow)
	{
		SShaderShadowProgram->Use();
	}
	else
	{
		SShaderProgram->Use();
		SShaderProgram->SetMaterials(Mf->GetMaterialData(), Mf->GetMaterialCount());
	}


	GLCall(glBindVertexArray(VAO));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));



	

	GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, Mf->GetFrameSize() * 2, Mf->GetFrameData(curentFrame == Mf->GetFrameCount() ? curentFrame - 1 : curentFrame)));
	//SShaderProgram->SetInterpolationTime(curentFrame == Mf->FrameCount ? 1 : InterpolationTime);
	SShaderProgram->SetInterpolationTime(InterpolationTime);
	//	std::cout << InterpolationTime << std::endl;
	
}
	



int QMesh::GetIndexCount()
{
	return (Mf ? Mf->GetIndexCount() : -1);
}

QMesh::QMesh(MeshFrames* mf)
{
	if (!SShaderProgram)
	{
		SShaderProgram = new ShaderProgram(VertexShaderPath, FragmentShaderPath);
		SShaderShadowProgram = new ShaderProgram(VertexShaderShaderPath, FragmentShaderShaderPath);
	}

	if (!mf)
	{
		std::cout << "QMesh::constructor mf == null" << std::endl;
		__debugbreak();
		return;
	}

	Mf = mf;

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

	glEnableVertexAttribArray(3);// for interpolation. same parameter for before steb but with offset one frame
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(SSVertexMaterial), (void*)(offsetof(SSVertexMaterial, Position) + mf->GetFrameSize()));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(SSVertexMaterial), (void*)(offsetof(SSVertexMaterial, Normal) + mf->GetFrameSize()));
	glEnableVertexAttribArray(5);// not use but need 
	glVertexAttribIPointer(5, 1, GL_INT, sizeof(SSVertexMaterial), (void*)offsetof(SSVertexMaterial, materialIndex));
	GLCall(glBufferData(GL_ARRAY_BUFFER, Mf->GetFrameSize() * 2, Mf->GetFrameData(1), GL_STATIC_DRAW));// specify what data and size
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));



	//  glBufferSubData,   modify data
	/*   IndexBuffer   */

	glGenBuffers(1, &IBO); // gen index buffer
	// ������� �� ���� ���� ������  � ���
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Mf->GetIndexCount() * sizeof(unsigned int), Mf->GetIndexData(), GL_STATIC_DRAW);// DYNAMIC
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}