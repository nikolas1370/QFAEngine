#include "MeshBase.h"
#include <Object/Actor/Actor.h>

MeshFrames::MeshFrames(int frameCount, int uniqueIndexCount, int indexCount, int materialCount, unsigned int framePerSecond, bool interpolation)
{
	FrameCount = frameCount;
	UniqueIndexCount = uniqueIndexCount;
	IndexCount = indexCount;
	MaterialCount = materialCount;
	FramePerSecond = framePerSecond;
	if (FramePerSecond == 0)
		FramePerSecond = 0;
	else
	 FrameTime = 1.0 / FramePerSecond;

	AnimationTime = FrameTime * FrameCount;
	Interpolation = interpolation;
	VertexCount = uniqueIndexCount * 3;// index have 3 vertex
	VerticesSize = sizeof(VertexMaterial) * VertexCount;

	FrameSize = sizeof(SSVertexMaterial) * uniqueIndexCount;
	FramesData = (char*)malloc(FrameSize * frameCount + sizeof(unsigned int) * IndexCount + sizeof(Material) * materialCount);
}

SSVertexMaterial* MeshFrames::GetFrameData(int frame) const
{
	if (frame < 1 || frame > FrameCount)
	{
		std::cout << "MeshFrames::GetFrameData frame number invalid ->" << frame << "<-" << std::endl;
		__debugbreak();
		return nullptr;
	}

	return (SSVertexMaterial*)&FramesData[FrameSize * (frame - 1)];
}
/*--*/

QMeshBaseComponent::QMeshBaseComponent()	
{	
	ForRender = true;
}


void QMeshBaseComponent::UpdateModelMatrix(bool onlyPosition)
{	// transform from x y z to -z x y
	if (onlyPosition)
	{
		ModelMatrix[3][0] = WorldPosition.X;
		ModelMatrix[3][1] = WorldPosition.Y;
		ModelMatrix[3][2] = WorldPosition.Z;
		return;
	}
		
	ModelMatrix = glm::mat4(RotationMatrix);
	
	ModelMatrix[3][0] = WorldPosition.X;
	ModelMatrix[3][1] = WorldPosition.Y;
	ModelMatrix[3][2] = WorldPosition.Z;

	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(AccumulateScale.Y * Scale.Y , AccumulateScale.Z * Scale.Z, AccumulateScale.X * Scale.X));
}
