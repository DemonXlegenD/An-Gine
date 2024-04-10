#include "Systems/simple_render_system.h"

#include "lve_descriptors.h"

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>
#include <iostream>

// std
#include <cassert>
#include <stdexcept>

namespace lve
{
	SimpleRenderSystem::SimpleRenderSystem(LveDevice&              _device, vk::RenderPass _renderPass,
	                                       vk::DescriptorSetLayout _globalSetLayout) : lveDevice(_device)
	{
		CreatePipelineLayout(_globalSetLayout);
		CreatePipeline(_renderPass);
	}

	SimpleRenderSystem::~SimpleRenderSystem() { lveDevice.Device().destroyPipelineLayout(pipelineLayout, nullptr); }

	void SimpleRenderSystem::CreatePipelineLayout(vk::DescriptorSetLayout _globalSetLayout)
	{
		vk::PushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;
		pushConstantRange.offset     = 0;
		pushConstantRange.size       = sizeof(SimplePushConstantData);

		std::vector<vk::DescriptorSetLayout> descriptorSetLayouts{_globalSetLayout};

		vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType                  = vk::StructureType::ePipelineLayoutCreateInfo;
		pipelineLayoutInfo.setLayoutCount         = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts            = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges    = &pushConstantRange;
		if (lveDevice.Device().createPipelineLayout(&pipelineLayoutInfo, nullptr, &pipelineLayout) !=
		    vk::Result::eSuccess)
			throw std::runtime_error("failed to create pipeline layout!");
	}


	void SimpleRenderSystem::CreatePipeline(vk::RenderPass _renderPass)
	{
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		LvePipeline::DefaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass     = _renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		lvePipeline                   = std::make_unique<LvePipeline>(
			lveDevice,
			"Shaders/simple_shader.vert.spv",
			"Shaders/simple_shader.frag.spv",
			pipelineConfig);
	}

	void SimpleRenderSystem::RenderGameObjects(FrameInfo& _frameInfo)
	{
		// Liaison du pipeline
		//lvePipeline->Bind(_frameInfo.commandBuffer);

		// Liaison de l'ensemble de descripteurs global
		//_frameInfo.commandBuffer.bindDescriptorSets(
		//	vk::PipelineBindPoint::eGraphics,
		//	pipelineLayout,
		//	0,
		//	_frameInfo.globalDescriptorSet,
		//	nullptr);

		for (auto& kv : _frameInfo.gameObjects)
		{
			auto& obj = kv.second;
			if (obj.model == nullptr) continue;
			if (obj.texture != nullptr) {

				// Si pas de texture, render avec un autre shader ?

				vk::DescriptorSetAllocateInfo allocInfo{};
				allocInfo.sType = vk::StructureType::eDescriptorSetAllocateInfo;
				allocInfo.descriptorPool = descriptorPool;


				//VkDescriptorSetAllocateInfo allocInfo{};
				//allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
				//allocInfo.descriptorPool = descriptorPool;
				//allocInfo.descriptorSetCount = 1;
				//allocInfo.pSetLayouts = &textureDescriptorSetLayout;

				//if (vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet) != VK_SUCCESS) {
				//	throw std::runtime_error("failed to allocate descriptor sets!");
				//}



				vk::DescriptorImageInfo imageInfo{};
				imageInfo.sampler = obj.texture->getSampler();
				imageInfo.imageView = obj.texture->getImageView();
				imageInfo.imageLayout = obj.texture->getImageLayout();

				std::cout << "1\n";

				if (_frameInfo.globalDescriptorSet == VK_NULL_HANDLE) {
					std::cout << "Descriptor is null\n";
				}

				vk::WriteDescriptorSet descriptorWrite = {};
				descriptorWrite.sType = vk::StructureType::eWriteDescriptorSet;
				descriptorWrite.dstSet = _frameInfo.globalDescriptorSet; // The descriptor set to update
				descriptorWrite.dstBinding = 1; // Binding 0
				descriptorWrite.dstArrayElement = 0;
				descriptorWrite.descriptorType = vk::DescriptorType::eCombinedImageSampler;
				descriptorWrite.descriptorCount = 1;
				descriptorWrite.pImageInfo = &imageInfo;

				//_frameInfo.commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 1, 1, )

				std::cout << "2\n";

				// Le command buffer record avant et ducoup �a fait l'erreur
				// "If the dstSet member of any element of pDescriptorWrites or pDescriptorCopies is bound, accessed, 
				// or modified by any command that was recorded to a command buffer which is currently in the recording or executable state, 
				// and any of the descriptor bindings that are updated were not created with the VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT or 
				// VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT bits set, that command buffer becomes invalid.

				lveDevice.Device().updateDescriptorSets(1, &descriptorWrite, 0, nullptr);
				std::cout << "3\n";
			}


			_frameInfo.commandBuffer.bindDescriptorSets(
				vk::PipelineBindPoint::eGraphics,
				pipelineLayout,
				0,
				_frameInfo.globalDescriptorSet,
				nullptr);

			SimplePushConstantData push{};
			push.modelMatrix  = obj.transform.Mat4();
			push.normalMatrix = obj.transform.NormalMatrix();

			// Mise � jour des push constants
			_frameInfo.commandBuffer.pushConstants<SimplePushConstantData>(
				pipelineLayout,
				vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
				0,
				push);

			// Liaison du mod�le et dessin
			lvePipeline->Bind(_frameInfo.commandBuffer);
			obj.model->Bind(_frameInfo.commandBuffer);
			obj.model->Draw(_frameInfo.commandBuffer);
		}
	}
} // namespace lve
