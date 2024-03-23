#include "lve_pipeline.h"

#include "lve_model.h"

// std
#include <cassert>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace lve {

    LvePipeline::LvePipeline(
        LveDevice& device,
        const std::string& vertFilepath,
        const std::string& fragFilepath,
        const PipelineConfigInfo& configInfo)
        : lveDevice{ device } {
        CreateGraphicsPipeline(vertFilepath, fragFilepath, configInfo);
    }

    LvePipeline::~LvePipeline() {
        vkDestroyShaderModule(lveDevice.device(), vertShaderModule, nullptr);
        vkDestroyShaderModule(lveDevice.device(), fragShaderModule, nullptr);
        vkDestroyPipeline(lveDevice.device(), graphicsPipeline, nullptr);
    }

    std::vector<char> LvePipeline::ReadFile(const std::string& _filepath) {
        std::ifstream file{ _filepath, std::ios::ate | std::ios::binary };

        if (!file.is_open()) {
            throw std::runtime_error("failed to open file: " + _filepath);
        }

        size_t fileSize = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();
        return buffer;
    }

    void LvePipeline::CreateGraphicsPipeline(
        const std::string& _vertFilepath,
        const std::string& _fragFilepath,
        const PipelineConfigInfo& _configInfo) {
        assert(
            _configInfo.pipelineLayout != VK_NULL_HANDLE &&
            "Cannot create graphics pipeline: no pipelineLayout provided in configInfo");
        assert(
            _configInfo.renderPass != VK_NULL_HANDLE &&
            "Cannot create graphics pipeline: no renderPass provided in configInfo");

        auto vertCode = ReadFile(_vertFilepath);
        auto fragCode = ReadFile(_fragFilepath);

        CreateShaderModule(vertCode, &vertShaderModule);
        CreateShaderModule(fragCode, &fragShaderModule);

        VkPipelineShaderStageCreateInfo shaderStages[2];
        shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].module = vertShaderModule;
        shaderStages[0].pName = "main";
        shaderStages[0].flags = 0;
        shaderStages[0].pNext = nullptr;
        shaderStages[0].pSpecializationInfo = nullptr;
        shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].module = fragShaderModule;
        shaderStages[1].pName = "main";
        shaderStages[1].flags = 0;
        shaderStages[1].pNext = nullptr;
        shaderStages[1].pSpecializationInfo = nullptr;

        auto bindingDescriptions = LveModel::Vertex::GetBindingDescriptions();
        auto attributeDescriptions = LveModel::Vertex::GetAttributeDescriptions();
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexAttributeDescriptionCount =
            static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
        vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &_configInfo.inputAssemblyInfo;
        pipelineInfo.pViewportState = &_configInfo.viewportInfo;
        pipelineInfo.pRasterizationState = &_configInfo.rasterizationInfo;
        pipelineInfo.pMultisampleState = &_configInfo.multisampleInfo;
        pipelineInfo.pColorBlendState = &_configInfo.colorBlendInfo;
        pipelineInfo.pDepthStencilState = &_configInfo.depthStencilInfo;
        pipelineInfo.pDynamicState = &_configInfo.dynamicStateInfo;

        pipelineInfo.layout = _configInfo.pipelineLayout;
        pipelineInfo.renderPass = _configInfo.renderPass;
        pipelineInfo.subpass = _configInfo.subpass;

        pipelineInfo.basePipelineIndex = -1;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(
            lveDevice.device(),
            VK_NULL_HANDLE,
            1,
            &pipelineInfo,
            nullptr,
            &graphicsPipeline) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline");
        }
    }

    void LvePipeline::CreateShaderModule(const std::vector<char>& _code, VkShaderModule* _shaderModule) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = _code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(_code.data());

        if (vkCreateShaderModule(lveDevice.device(), &createInfo, nullptr , _shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module");
        }
    }

    void LvePipeline::Bind(VkCommandBuffer _commandBuffer) {
        vkCmdBindPipeline(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
    }

    void LvePipeline::DefaultPipelineConfigInfo(PipelineConfigInfo& _configInfo) {
        _configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        _configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        _configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        _configInfo.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        _configInfo.viewportInfo.viewportCount = 1;
        _configInfo.viewportInfo.pViewports = nullptr;
        _configInfo.viewportInfo.scissorCount = 1;
        _configInfo.viewportInfo.pScissors = nullptr;

        _configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        _configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
        _configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
        _configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
        _configInfo.rasterizationInfo.lineWidth = 1.0f;
        _configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
        _configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        _configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
        _configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
        _configInfo.rasterizationInfo.depthBiasClamp = 0.0f;           // Optional
        _configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional

        _configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        _configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
        _configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        _configInfo.multisampleInfo.minSampleShading = 1.0f;           // Optional
        _configInfo.multisampleInfo.pSampleMask = nullptr;             // Optional
        _configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
        _configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional

        _configInfo.colorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;
        _configInfo.colorBlendAttachment.blendEnable = VK_FALSE;
        _configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
        _configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
        _configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
        _configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
        _configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
        _configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

        _configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        _configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
        _configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
        _configInfo.colorBlendInfo.attachmentCount = 1;
        _configInfo.colorBlendInfo.pAttachments = &_configInfo.colorBlendAttachment;
        _configInfo.colorBlendInfo.blendConstants[0] = 0.0f;  // Optional
        _configInfo.colorBlendInfo.blendConstants[1] = 0.0f;  // Optional
        _configInfo.colorBlendInfo.blendConstants[2] = 0.0f;  // Optional
        _configInfo.colorBlendInfo.blendConstants[3] = 0.0f;  // Optional

        _configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        _configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
        _configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
        _configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
        _configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
        _configInfo.depthStencilInfo.minDepthBounds = 0.0f;  // Optional
        _configInfo.depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
        _configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
        _configInfo.depthStencilInfo.front = {};  // Optional
        _configInfo.depthStencilInfo.back = {};   // Optional

        _configInfo.dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        _configInfo.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        _configInfo.dynamicStateInfo.pDynamicStates = _configInfo.dynamicStateEnables.data();
        _configInfo.dynamicStateInfo.dynamicStateCount =
            static_cast<uint32_t>(_configInfo.dynamicStateEnables.size());
        _configInfo.dynamicStateInfo.flags = 0;
    }

}  // namespace lve