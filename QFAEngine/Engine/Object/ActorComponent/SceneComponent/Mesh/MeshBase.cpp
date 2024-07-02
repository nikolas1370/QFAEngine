#include "pch.h"
#include "MeshBase.h"
#include <Object/Actor/Actor.h>
#include <Render/vk/LogicalDevice.h>
#include <Tools/VulkanSuff.h>
#include <Render/Buffer/VKBuffer.h>
#include <Render/Pipline/Pipline.h>
#include <Render/Buffer/VertexBuffer.h>

QMeshBaseComponent::SShaderDirLight QMeshBaseComponent::ShaderDL;

glm::mat4 QMeshBaseComponent::LightMatrix;

QFAVKPipeline* QMeshBaseComponent::Pipeline;
QFAVKPipeline* QMeshBaseComponent::ShadowPipline;

VkCommandPool QMeshBaseComponent::commandPool;
VkRenderPass QMeshBaseComponent::RenderPass;
VkRenderPass QMeshBaseComponent::ShadowRenderPass;

VkDescriptorPool QMeshBaseComponent::descriptorPool;
std::vector<VkDescriptorPool> QMeshBaseComponent::descriptorPoolsTwo;
std::vector<VkDescriptorPool> QMeshBaseComponent::descriptorPoolsShadow;

VkDescriptorSet QMeshBaseComponent::descriptorSetShadow;

std::vector<QMeshBaseComponent::SSet1Buffers> QMeshBaseComponent::Set1Buffers;
unsigned int QMeshBaseComponent::SetsInUse = 0;
std::vector<VkDescriptorSet> QMeshBaseComponent::ShadowDescriptorSets;


std::vector<QMeshBaseComponent*> QMeshBaseComponent::MeshIdList;
unsigned int QMeshBaseComponent::MaxMeshId;

MeshData::MeshData(int uniqueIndexCount, int indexCount, int materialCount)
{
	
	Mi.UniqueIndexCount = uniqueIndexCount;
	Mi.IndexCount = indexCount;
	Mi.MaterialCount = materialCount;
	
	Mi.VertexCount = uniqueIndexCount * 3;// index have 3 vertex
	Mi.VerticesSize = sizeof(VertexMaterial) * Mi.VertexCount;

	Mi.FrameSize = sizeof(SSVertexMaterial) * uniqueIndexCount;
	Mi.AmountData = Mi.FrameSize + sizeof(unsigned int) * Mi.IndexCount + sizeof(Material) * materialCount;
	FramesData = (char*)malloc(Mi.AmountData);
}

MeshData::MeshData(int vertexCount, int indexCount, int materialCount, int notNed)
{	
	Mi.MaterialCount = materialCount;
	Mi.IndexCount = indexCount;
	Mi.VertexCount = vertexCount;
	Mi.VerticesSize = sizeof(VertexMaterial) * vertexCount;

	Mi.FrameSize = sizeof(SSVertexMaterial) * vertexCount;
	Mi.AmountData = Mi.FrameSize + (sizeof(unsigned int) * Mi.IndexCount) + (sizeof(Material) * materialCount);
	FramesData = (char*)malloc(Mi.AmountData);
}

MeshData::MeshData(SMeshInfo* mi, void* framesData)
{
	FramesData = (char*)framesData;
	Mi = *mi;
}

void MeshData::CreateVertextIndexBuffer()
{
	if (VertexBufer)
		return;
	
	
	VertexBufer = new QFAVKVertexBuffer(GetVerticesSize(), GetVerticesDate(), QFAWindow::commandPool);
	IndexBuffer = new QFAVKIndexBuffer(GetIndexCount() * sizeof(int), GetIndexData(), QFAWindow::commandPool);
}

SSVertexMaterial* MeshData::GetFrameData() const
{
	return (SSVertexMaterial*)&FramesData[0];
}
/*

	SMeshInfo GetMeshInfo() const;

*/
MeshData::SMeshInfo MeshData::GetMeshInfo() const
{
	return Mi;
}

/*--------------*/

QMeshBaseComponent::QMeshBaseComponent()	
{	
	NeedUpdateMatrix = true;
}

QMeshBaseComponent::~QMeshBaseComponent()
{

		/*	
		delete all
	*/
}

void* QMeshBaseComponent::GetModelBuffer()
{
	if (SetsInUse >= Set1Buffers.size())
		createDescriptorSet1();

	return Set1Buffers[SetsInUse].vertexBuffer->MapData;
}

void* QMeshBaseComponent::GetFragmentBuffer()
{
	if (SetsInUse >= Set1Buffers.size())
		createDescriptorSet1();

	return Set1Buffers[SetsInUse].fragmentBuffer->MapData;
}

void* QMeshBaseComponent::GetShadowBuffer()
{
	return QFAWindow::ViewportStuff[QFAWindow::ViewportProcess].buffers.shadowBuffer->MapData;
}

std::array<VkDescriptorSet, 2> QMeshBaseComponent::GetNextSets()
{
	return std::array<VkDescriptorSet, 2>
	{
		Pipeline->GetSet(0, QFAWindow::CurentProcessWindow->ViewportProcess),
			Pipeline->GetSet(1, SetsInUse++)
	};
}

VkDescriptorSet QMeshBaseComponent::GetShadowNextSet()
{
	return ShadowPipline->GetSet(0, QFAWindow::CurentProcessWindow->ViewportProcess);
}

void QMeshBaseComponent::EndLife()
{
}

void QMeshBaseComponent::UpdateModelMatrix()
{	
	ModelMatrix = glm::mat4(RotationMatrix);
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(AccumulateScale.Y * Scale.Y , AccumulateScale.Z * Scale.Z, AccumulateScale.X * Scale.X));
}



void QMeshBaseComponent::StartFrame()
{
	SetsInUse = 0;
	MaxMeshId = 0;
	MeshIdList.clear();
}

void QMeshBaseComponent::StartShadowFrameViewport(glm::mat4& lmat)
{
	LightMatrix = lmat;
}

void QMeshBaseComponent::Render(VkCommandBuffer commandBuffer, bool shadow, FVector cameraPosition)
{
	UpdateBuffers(commandBuffer, 0, shadow, cameraPosition);


	VkDeviceSize offsets[] = { 0 };
	if (shadow)
	{		
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &Mf->VertexBufer->GpuSideBuffer->Buffer, offsets);
		vkCmdBindIndexBuffer(commandBuffer, Mf->IndexBuffer->GpuSideBuffer->Buffer, 0, VK_INDEX_TYPE_UINT32);
		auto nextSet = GetShadowNextSet();
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			ShadowPipline->GetPipelineLayout(), 0, 1, &nextSet, 0, nullptr);

		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(GetIndexCount()), 1, 0, 0, 0);
	}
	else
	{
		MeshIdList.push_back(this);
		PickId.meshId = ++MaxMeshId;
		vkCmdPushConstants(commandBuffer, Pipeline->GetPipelineLayout(), VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PickId), &PickId);

		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &Mf->VertexBufer->GpuSideBuffer->Buffer, offsets);
		vkCmdBindIndexBuffer(commandBuffer, Mf->IndexBuffer->GpuSideBuffer->Buffer, 0, VK_INDEX_TYPE_UINT32);

		auto nextSets = GetNextSets();
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			Pipeline->GetPipelineLayout(), 0, (uint32_t)nextSets.size(), nextSets.data(), 0, nullptr);

		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(GetIndexCount()), 1, 0, 0, 0);
	}
}



void QMeshBaseComponent::StartFrameViewpoet(glm::mat4& viewPortProjection, glm::mat3& cameraRotationMatrix, glm::mat4& directionLightMatrix, int viewportIndex)
{
	QMeshBaseComponent::UBOVertex ubo{};

	ubo.projection = viewPortProjection;
	
	ubo.cameraR = glm::mat4(cameraRotationMatrix);
	ubo.directionLightMatrix = directionLightMatrix;	
	memcpy(QFAWindow::ViewportStuff[QFAWindow::ViewportProcess].buffers.worldProjectionBuffer->MapData, &ubo.projection, sizeof(ubo) );
}

void QMeshBaseComponent::createDescriptorSet0(VkBuffer buffer, VkBuffer shadeowBuffer)
{
	VkDescriptorBufferInfo bufferInfoVertex{};
	bufferInfoVertex.buffer = buffer;
	bufferInfoVertex.offset = 0;
	bufferInfoVertex.range = sizeof(UBOVertex);

	std::array< QFAVKPipeline::QFADescriptorSetInfo, 2 > SetsInfo;
	SetsInfo[0].dstBinding = 0;
	SetsInfo[0].DescriptorBufferInfos = &bufferInfoVertex;


	VkDescriptorImageInfo IF;// 
	IF.imageLayout = VK_IMAGE_LAYOUT_GENERAL;// VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	IF.imageView = QFAWindow::ShadowImagesView->ImageView;
	IF.sampler = QFAWindow::ShadowSampler->textureSampler;

	SetsInfo[1].dstBinding = 1;
	SetsInfo[1].DescriptorImageInfos = &IF;

	Pipeline->CreateSet(0, SetsInfo.data());
	/*---*/
	VkDescriptorBufferInfo bufferInfo;
	bufferInfo.buffer = shadeowBuffer;
	bufferInfo.offset = 0;
	bufferInfo.range = sizeof(UBOShadowVertex);

	QFAVKPipeline::QFADescriptorSetInfo setInfos;
	setInfos.dstBinding = 0;
	setInfos.DescriptorBufferInfos = &bufferInfo;

	ShadowPipline->CreateSet(0, &setInfos);
}

void QMeshBaseComponent::createDescriptorSet1()
{
	VkDeviceSize bufferSize = sizeof(glm::mat4);
	VkDeviceSize bufferSizeFragment = sizeof(UBOFragment);
	SSet1Buffers Buffers;
	Buffers.vertexBuffer = new QFAVKBuffer(bufferSize, nullptr, true, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
	Buffers.fragmentBuffer = new QFAVKBuffer(bufferSizeFragment, nullptr, true, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
	Set1Buffers.push_back(Buffers);


	VkDescriptorBufferInfo bufferInfoVertexMatrix{};	
	bufferInfoVertexMatrix.buffer = Set1Buffers.back().vertexBuffer->Buffer;
	bufferInfoVertexMatrix.offset = 0;
	bufferInfoVertexMatrix.range = bufferSize;

	VkDescriptorBufferInfo bufferInfoFragment{};
	bufferInfoFragment.buffer = Set1Buffers.back().fragmentBuffer->Buffer;
	bufferInfoFragment.offset = 0;
	bufferInfoFragment.range = sizeof(UBOFragment);


	
	std::array< QFAVKPipeline::QFADescriptorSetInfo, 2> SetsInfo;
	SetsInfo[0].dstBinding = 0;
	SetsInfo[0].DescriptorBufferInfos = &bufferInfoVertexMatrix;
	
	SetsInfo[1].dstBinding = 1;
	SetsInfo[1].DescriptorBufferInfos = &bufferInfoFragment;



	Pipeline->CreateSet(1, SetsInfo.data());
}

void QMeshBaseComponent::Init(VkRenderPass renderPass, VkRenderPass shadowRenderPass, VkCommandPool commandPool_)
{
	commandPool = commandPool_;
	RenderPass = renderPass;
	ShadowRenderPass = shadowRenderPass;

	CreatePipeline();
	
	CreateShadowPipline();
	MeshIdList.reserve(1000);
}

void QMeshBaseComponent::CreatePipeline()
{
	QFAVKPipeline::QFAPipelineCreateInfo PipelineInfo;
	PipelineInfo.RenderPass = RenderPass;
	PipelineInfo.PipelineShaderStages.VertexShaderName = U"Mesh.spvv";
	PipelineInfo.PipelineShaderStages.FragmentShaderName = U"Mesh.spvf";

	PipelineInfo.VertexInputInfo.VertexBindingDescriptionCount = 1;
	VkVertexInputBindingDescription bindingDescription{};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(SSVertexMaterial);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	PipelineInfo.VertexInputInfo.VertexBindingDescriptions = &bindingDescription;


	std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
	PipelineInfo.VertexInputInfo.VertexAttributeDescriptionCount = (uint32_t)attributeDescriptions.size();
	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(SSVertexMaterial, Position);

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(SSVertexMaterial, Normal);

	attributeDescriptions[2].binding = 0;
	attributeDescriptions[2].location = 2;
	attributeDescriptions[2].format = VK_FORMAT_R32_SINT;
	attributeDescriptions[2].offset = offsetof(SSVertexMaterial, materialIndex);
	PipelineInfo.VertexInputInfo.VertexAttributeDescriptions = attributeDescriptions.data();


	PipelineInfo.Rasterization.CullMode = VK_CULL_MODE_BACK_BIT;
	std::array< QFAVKPipeline::QFAPipelineColorBlendAttachment, 2> blendAttachments;
	blendAttachments[0].BlendEnable = VK_FALSE;
	blendAttachments[1].BlendEnable = VK_FALSE;


	PipelineInfo.ColorBlendState.attachmentCount = (uint32_t)blendAttachments.size();
	PipelineInfo.ColorBlendState.pAttachments = blendAttachments.data();

	std::array<VkDynamicState, 2> dynamicStates =
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};
	PipelineInfo.DynamicStates = dynamicStates.data();
	PipelineInfo.DynamicStateCount = (uint32_t)dynamicStates.size();


	PipelineInfo.DepthStencil.DepthTestEnable = VK_TRUE;
	PipelineInfo.DepthStencil.DepthWriteEnable = VK_TRUE;
	PipelineInfo.DepthStencil.DepthCompareOp = VK_COMPARE_OP_LESS;

	std::array< QFAVKPipeline::QFADescriptorSetLayout, 2> DescriptorSetLayouts;
	std::array<VkDescriptorSetLayoutBinding, 2> firsLayout;
	firsLayout[0].binding = 0;
	firsLayout[0].descriptorCount = 1;
	firsLayout[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	firsLayout[0].pImmutableSamplers = nullptr;
	firsLayout[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	firsLayout[1].binding = 1;
	firsLayout[1].descriptorCount = 1;
	firsLayout[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	firsLayout[1].pImmutableSamplers = nullptr;
	firsLayout[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::array<VkDescriptorSetLayoutBinding, 2> secondLayout;
	secondLayout[0].binding = 0;
	secondLayout[0].descriptorCount = 1;
	secondLayout[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	secondLayout[0].pImmutableSamplers = nullptr;
	secondLayout[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	secondLayout[1].binding = 1;
	secondLayout[1].descriptorCount = 1;
	secondLayout[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	secondLayout[1].pImmutableSamplers = nullptr;
	secondLayout[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	DescriptorSetLayouts[0].BindingCount = (uint32_t)firsLayout.size();
	DescriptorSetLayouts[1].BindingCount = (uint32_t)secondLayout.size();
	DescriptorSetLayouts[0].Bindings = firsLayout.data();
	DescriptorSetLayouts[1].Bindings = secondLayout.data();

	PipelineInfo.DescriptorSetLayoutCount = (uint32_t)DescriptorSetLayouts.size();
	PipelineInfo.DescriptorSetLayouts = DescriptorSetLayouts.data();


	std::array< uint32_t, 2> MaxSets;
	MaxSets[0] = DescriptorSets0Amount;
	MaxSets[1] = DescriptorSets0Amount;
	PipelineInfo.MaxSets = MaxSets.data();


	VkPushConstantRange range = {};
	range.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	range.offset = 0;
	range.size = sizeof(SPushConstantPickId); // use for pick mesh
	PipelineInfo.PushConstant.pushConstantRangeCount = 1;
	PipelineInfo.PushConstant.PushConstantRanges = &range;

	Pipeline = new QFAVKPipeline(PipelineInfo);
}

void QMeshBaseComponent::CreateShadowPipline()
{
	QFAVKPipeline::QFAPipelineCreateInfo PipelineInfo;
	PipelineInfo.RenderPass = ShadowRenderPass;
	PipelineInfo.PipelineShaderStages.VertexShaderName = U"Shadow.spvv";

	PipelineInfo.VertexInputInfo.VertexBindingDescriptionCount = 1;
	VkVertexInputBindingDescription bindingDescription{};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(SSVertexMaterial);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	PipelineInfo.VertexInputInfo.VertexBindingDescriptions = &bindingDescription;


	PipelineInfo.VertexInputInfo.VertexAttributeDescriptionCount = 1;
	std::array<VkVertexInputAttributeDescription, 1> attributeDescriptions{};
	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0; 
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = 0;
	PipelineInfo.VertexInputInfo.VertexAttributeDescriptions = attributeDescriptions.data();

	PipelineInfo.Rasterization.FrontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	PipelineInfo.Rasterization.CullMode = VK_CULL_MODE_NONE;
	PipelineInfo.Rasterization.DepthBiasEnable = VK_TRUE;

	std::array<VkDynamicState, 3> dynamicStates =
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR,
		VK_DYNAMIC_STATE_DEPTH_BIAS
	};
	PipelineInfo.DynamicStates = dynamicStates.data();
	PipelineInfo.DynamicStateCount = (uint32_t)dynamicStates.size();


	std::array< QFAVKPipeline::QFADescriptorSetLayout, 1> DescriptorSetLayouts;
	std::array<VkDescriptorSetLayoutBinding, 1> bindingsFragment;
	bindingsFragment[0].binding = 0;
	bindingsFragment[0].descriptorCount = 1;
	bindingsFragment[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	bindingsFragment[0].pImmutableSamplers = nullptr;
	bindingsFragment[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	DescriptorSetLayouts[0].BindingCount = (uint32_t)bindingsFragment.size();
	DescriptorSetLayouts[0].Bindings = bindingsFragment.data();

	PipelineInfo.DescriptorSetLayoutCount = (uint32_t)DescriptorSetLayouts.size();
	PipelineInfo.DescriptorSetLayouts = DescriptorSetLayouts.data();
	
	PipelineInfo.DepthStencil.DepthTestEnable = VK_TRUE;
	PipelineInfo.DepthStencil.DepthWriteEnable = VK_TRUE;
	PipelineInfo.DepthStencil.DepthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;

	std::array< uint32_t, 1> MaxSets;
	MaxSets[0] = DescriptorSets0Amount;
	PipelineInfo.MaxSets = MaxSets.data();
	ShadowPipline = new QFAVKPipeline(PipelineInfo);
	VkDeviceSize bufferSize = sizeof(UBOShadowVertex);
	

}
