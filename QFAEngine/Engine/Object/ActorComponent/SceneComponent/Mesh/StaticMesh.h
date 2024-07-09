#pragma once

#include <Object/ActorComponent/SceneComponent/Mesh/MeshBase.h>

class QFAEXPORT QStaticMesh : public QMeshBaseComponent
{
public:

    QStaticMesh(){}

    QStaticMesh(QFAMeshData* meshData);
    
    void SetMesh(QFAMeshData* meshData) override;

    // remove startFrameTime
    void UpdateBuffers(VkCommandBuffer commandBuffer, uint64_t startFrameTime, bool isShadow, const FVector& cameraPos) override;

    int GetIndexCount() override;
};


