#include "MeshBase.h"
#include <Object/Actor/Actor.h>
#include <Render/vk/LogicalDevice.h>
#include <Tools/VulkanSuff.h>
#include <Render/Buffer/VKBuffer.h>


#include <Render/Pipline/MeshShadowPipeline.h>


QMeshBaseComponent::SShaderDirLight QMeshBaseComponent::ShaderDL;

glm::mat4 QMeshBaseComponent::LightMatrix;

QFAVKMeshPipeline* QMeshBaseComponent::Pipeline;
QFAVKMeshShadowPipeline* QMeshBaseComponent::ShadowPipline;

VkCommandPool QMeshBaseComponent::commandPool;
VkRenderPass QMeshBaseComponent::RenderPass;

VkDescriptorPool QMeshBaseComponent::descriptorPool;
std::vector<VkDescriptorPool> QMeshBaseComponent::descriptorPoolsTwo;
std::vector<VkDescriptorPool> QMeshBaseComponent::descriptorPoolsShadow;


VkDescriptorSet QMeshBaseComponent::descriptorSetShadow;



std::vector<VkDescriptorSet> QMeshBaseComponent::DescriptorSets1;
std::vector<QMeshBaseComponent::SSet1Buffers> QMeshBaseComponent::Set1Buffers;
unsigned int QMeshBaseComponent::SetsInUse = 0;
unsigned int QMeshBaseComponent::ShadowSetsInUse = 0;
std::vector<VkDescriptorSet> QMeshBaseComponent::ShadowDescriptorSets;
std::vector<QFAVKBuffer*> QMeshBaseComponent::ShadowSetBuffers;


std::array<VkDescriptorSet, QFAWindow::MaxActiveViewPort> QMeshBaseComponent::ViewportSets;
std::array<QFAVKBuffer*, QFAWindow::MaxActiveViewPort> QMeshBaseComponent::BuffersVertex;


MeshData::MeshData(int uniqueIndexCount, int indexCount, int materialCount)
{
	
	UniqueIndexCount = uniqueIndexCount;
	IndexCount = indexCount;
	MaterialCount = materialCount;
	
	VertexCount = uniqueIndexCount * 3;// index have 3 vertex
	VerticesSize = sizeof(VertexMaterial) * VertexCount;

	FrameSize = sizeof(SSVertexMaterial) * uniqueIndexCount;
	FramesData = (char*)malloc(FrameSize + sizeof(unsigned int) * IndexCount + sizeof(Material) * materialCount);
}


/*

;kj-----------------------
*/

MeshData::MeshData(int vertexCount, int indexCount, int materialCount, int notNed)
{	
	MaterialCount = materialCount;
	IndexCount = indexCount;
	VertexCount = vertexCount;
	VerticesSize = sizeof(VertexMaterial) * vertexCount;

	FrameSize = sizeof(SSVertexMaterial) * vertexCount;
	FramesData = (char*)malloc(FrameSize + ( sizeof(unsigned int) * IndexCount) + ( sizeof(Material) * materialCount));
}

SSVertexMaterial* MeshData::GetFrameData(int frame) const
{
	return (SSVertexMaterial*)&FramesData[0];
}

/*--------------*/

QMeshBaseComponent::QMeshBaseComponent()	
{	
	NeedUpdateMatrix = true;
}




void QMeshBaseComponent::UpdateModelMatrix()
{	
	ModelMatrix = glm::mat4(RotationMatrix);
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(AccumulateScale.Y * Scale.Y , AccumulateScale.Z * Scale.Z, AccumulateScale.X * Scale.X));
}

void QMeshBaseComponent::StartShadowFrame()
{
	ShadowSetsInUse = 0;
}

void QMeshBaseComponent::StartShadowFrameViewport(glm::mat4& lmat)
{
	LightMatrix = lmat;
}

void QMeshBaseComponent::EndLife()
{
}


VkVertexInputBindingDescription QMeshBaseComponent::getBindingDescription()
{
	VkVertexInputBindingDescription bindingDescription{};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(SSVertexMaterial);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 3> QMeshBaseComponent::getAttributeDescriptions()
{
	std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

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

	return attributeDescriptions;
}

void QMeshBaseComponent::CreateVertexIndexBuffers()
{
	VertexBufer = new QFAVKVertexBuffer(Mf->GetVerticesSize(), Mf->GetVerticesDate(), commandPool);
	IndexBuffer = new QFAVKIndexBuffer(Mf->GetIndexCount() * sizeof(int), Mf->GetIndexData(), commandPool);
}


void QMeshBaseComponent::StartFrameViewpoet(glm::mat4& viewPortProjection, glm::mat3& cameraRotationMatrix, glm::mat4& directionLightMatrix)
{
	QMeshBaseComponent::UBOVertex ubo{};

	ubo.projection = viewPortProjection;
	
	ubo.cameraR = glm::mat4(cameraRotationMatrix);
	ubo.directionLightMatrix = directionLightMatrix;
	
	memcpy(BuffersVertex[QFAWindow::GetMainWindow()->ViewportProcess]->MapData, &ubo.projection, sizeof(ubo) );
	SetsInUse = 0;	
}


void QMeshBaseComponent::createDescriptorPool0()
{
	std::array<VkDescriptorPoolSize, 2> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = 1;	

	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = 1;


	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = QFAWindow::MaxActiveViewPort;
	poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT; // if not need free descriptorSet not set it flag

	

	if (vkCreateDescriptorPool(QFAVKLogicalDevice::GetDevice(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
		stopExecute("failed to create descriptor pool!");

	createDescriptorSet0();
}

void QMeshBaseComponent::createDescriptorPool1()
{
	std::array<VkDescriptorPoolSize, 2> poolSizes2{};
	poolSizes2[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes2[0].descriptorCount = 1;
	poolSizes2[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes2[1].descriptorCount = 1;

	VkDescriptorPoolCreateInfo poolInfo2{};
	poolInfo2.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo2.poolSizeCount = static_cast<uint32_t>(poolSizes2.size());
	poolInfo2.pPoolSizes = poolSizes2.data();
	poolInfo2.maxSets = DescriptorSets1Amount;

	VkDescriptorPool pool;
	if (vkCreateDescriptorPool(QFAVKLogicalDevice::GetDevice(), &poolInfo2, nullptr, &pool) != VK_SUCCESS)
		stopExecute("failed to create descriptor pool!");

	descriptorPoolsTwo.push_back(pool);
	createDescriptorSet1();

}

void QMeshBaseComponent::createDescriptorPoolShadow()
{
	std::array<VkDescriptorPoolSize, 1> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = 1;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = DescriptorSets1Amount;	

	VkDescriptorPool pool;
	if (vkCreateDescriptorPool(QFAVKLogicalDevice::GetDevice(), &poolInfo, nullptr, &pool) != VK_SUCCESS)
		stopExecute("failed to create descriptor pool!");


	descriptorPoolsShadow.push_back(pool);
	createDescriptorSetShadow();
}

void QMeshBaseComponent::createDescriptorSet0()
{
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &Pipeline->descriptorSetLayout[0];

	QFAWindow* window = QFAWindow::GetMainWindow();
	VkDeviceSize bufferSize = sizeof(UBOVertex);
	for (size_t i = 0; i < QFAWindow::MaxActiveViewPort; i++)
	{
		if (vkAllocateDescriptorSets(QFAVKLogicalDevice::GetDevice(), &allocInfo, &ViewportSets[i]) != VK_SUCCESS)
			stopExecute("failed to allocate descriptor sets!");


		BuffersVertex[i] = new QFAVKBuffer(bufferSize, nullptr, true, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

		VkDescriptorBufferInfo bufferInfoVertex{};
		bufferInfoVertex.buffer = BuffersVertex[i]->Buffer;
		bufferInfoVertex.offset = 0;
		bufferInfoVertex.range = sizeof(UBOVertex);

		std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = ViewportSets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfoVertex;

		VkDescriptorImageInfo IF;// 
		IF.imageLayout = VK_IMAGE_LAYOUT_GENERAL;// VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		IF.imageView = window->ShadowImagesView->ImageView;
		IF.sampler = window->ShadowSampler->textureSampler;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = ViewportSets[i];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &IF;

		vkUpdateDescriptorSets(QFAVKLogicalDevice::GetDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}	
}

void QMeshBaseComponent::createDescriptorSet1()
{
	auto last = DescriptorSets1.size();
	DescriptorSets1.resize(last + DescriptorSets1Amount);
	Set1Buffers.resize(DescriptorSets1.size());

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPoolsTwo.back();// redo descriptorPool[1]
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &Pipeline->descriptorSetLayout[1];

	VkDeviceSize bufferSize = sizeof(glm::mat4);
	VkDeviceSize bufferSizeFragment = sizeof(UBOFragment);
	VkDescriptorBufferInfo bufferInfoVertexMatrix{};	
	bufferInfoVertexMatrix.offset = 0;
	bufferInfoVertexMatrix.range = bufferSize;
	
	for (size_t i = last; i < DescriptorSets1.size(); i++)
	{	
		if (vkAllocateDescriptorSets(QFAVKLogicalDevice::GetDevice(), &allocInfo, &DescriptorSets1[i]) != VK_SUCCESS)
			stopExecute("failed to allocate descriptor sets!");

		Set1Buffers[i].vertexBuffer = new QFAVKBuffer(bufferSize, nullptr, true, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
		Set1Buffers[i].fragmentBuffer = new QFAVKBuffer(bufferSizeFragment, nullptr, true, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

		bufferInfoVertexMatrix.buffer = Set1Buffers[i].vertexBuffer->Buffer;
		std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = DescriptorSets1[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfoVertexMatrix;

		VkDescriptorBufferInfo bufferInfoFragment{};
		bufferInfoFragment.buffer = Set1Buffers[i].fragmentBuffer->Buffer; 
		bufferInfoFragment.offset = 0;
		bufferInfoFragment.range = sizeof(UBOFragment);

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = DescriptorSets1[i];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pBufferInfo = &bufferInfoFragment;

		vkUpdateDescriptorSets(QFAVKLogicalDevice::GetDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}

void QMeshBaseComponent::createDescriptorSetShadow()
{
	auto last = ShadowDescriptorSets.size();
	
	ShadowDescriptorSets.resize(last + DescriptorSets1Amount);
	ShadowSetBuffers.resize(ShadowDescriptorSets.size());

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPoolsShadow.back();
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &ShadowPipline->descriptorSetLayout;
	VkDeviceSize bufferSize = sizeof(UBOShadowVertex);

	VkDescriptorBufferInfo bufferInfoVertex{};
	bufferInfoVertex.offset = 0;
	bufferInfoVertex.range = sizeof(UBOShadowVertex);

	for (size_t i = last; i < ShadowDescriptorSets.size(); i++)
	{
		if (vkAllocateDescriptorSets(QFAVKLogicalDevice::GetDevice(), &allocInfo, &ShadowDescriptorSets[i]) != VK_SUCCESS)
			stopExecute("failed to allocate descriptor sets!");
		
		ShadowSetBuffers[i] = new QFAVKBuffer(bufferSize, nullptr, true, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);		

		bufferInfoVertex.buffer = ShadowSetBuffers[i]->Buffer;

		std::array<VkWriteDescriptorSet, 1> descriptorWrites{};

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = ShadowDescriptorSets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfoVertex;
		vkUpdateDescriptorSets(QFAVKLogicalDevice::GetDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);		
	}
}

void QMeshBaseComponent::Init(VkRenderPass renderPass, VkRenderPass shadowRenderPass, VkCommandPool commandPool_)
{
	commandPool = commandPool_;
	RenderPass = renderPass;

	Pipeline = new QFAVKMeshPipeline(renderPass, "Engine/Shaders/MeshVertex.spv", "Engine/Shaders/MeshFragment.spv");	
	ShadowPipline = new QFAVKMeshShadowPipeline(shadowRenderPass, "Engine/Shaders/ShadowVertex.spv");
	createDescriptorPool0();
	createDescriptorPool1();
	createDescriptorPoolShadow();
}
