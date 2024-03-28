#pragma once

#include <Object/ActorComponent/SceneComponent/Mesh/MeshBase.h> // remove to this file

class QStaticMesh :  public QMeshBaseComponent
{
public:

    QStaticMesh(){}

    QStaticMesh(MeshData* meshData);
    
    void SetMesh(MeshData* meshData);

    // remove startFrameTime
    void UpdateBuffers(VkCommandBuffer commandBuffer, uint64_t startFrameTime, bool isShadow, const FVector& cameraPos) override;

    int GetIndexCount() override;

private:
    
};


