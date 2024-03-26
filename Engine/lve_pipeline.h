#pragma once

#include "lve_device.h"

#include <string>
#include <vector>

namespace lve {

	struct PipelineConfigInfo {
		//PipelineConfigInfo(const PipelineConfigInfo&) = delete;
		//PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
		std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		std::vector<VkDynamicState> dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};



	class LvePipeline
	{
	public:

		/**
		 * @brief Constructeur de la classe LvePipeline.
		 *
		 * Initialise un pipeline graphique en cr�ant les shaders � partir des fichiers sp�cifi�s.
		 *
		 * @param _vertFilepath Le chemin d'acc�s au fichier contenant le code source du shader de vertex.
		 * @param _fragFilepath Le chemin d'acc�s au fichier contenant le code source du shader de fragment.
		 */
		LvePipeline(LveDevice & _device, const std::string& _vertFilepath, const std::string& _fragFilepath, const PipelineConfigInfo& _configInfo);

		~LvePipeline();

		LvePipeline(const LvePipeline&) = delete;
		LvePipeline &operator=(const LvePipeline&) = delete;


		void Bind(VkCommandBuffer commandBuffer);
		static void DefaultPipelineConfigInfo(PipelineConfigInfo& _configInfo);

	private:

		/**
		* @brief Lit le contenu d'un fichier binaire et le retourne sous forme de vecteur de caract�res.
		 *
		 * Cette fonction ouvre un fichier binaire, lit son contenu dans un vecteur de caract�res et le retourne.
		 *
		* @param _filepath Le chemin d'acc�s au fichier � lire.
		* @return Un vecteur de caract�res contenant le contenu du fichier.
		* @throws std::runtime_error si le fichier ne peut pas �tre ouvert.
		*/
		static std::vector<char> ReadFile(const std::string& _filepath);

		/**
		* @brief Cr�e un pipeline graphique en chargeant les shaders depuis les fichiers sp�cifi�s.
		*
		* Cette fonction charge le code source des shaders de vertex et de fragment depuis les fichiers sp�cifi�s,
		* puis affiche la taille du code source des shaders.
		*
		* @param _vertFilepath Le chemin d'acc�s au fichier contenant le code source du shader de vertex.
		* @param _fragFilepath Le chemin d'acc�s au fichier contenant le code source du shader de fragment.
		*/
		void CreateGraphicsPipeline(const std::string& _vertFilepath, const std::string& _fragFilepath, const PipelineConfigInfo& _configInfo);

		void CreateShaderModule(const std::vector<char>& _code, VkShaderModule* _shaderModule);

		LveDevice& lveDevice;
		VkPipeline graphicsPipeline;
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;
	};

} //namespace lve

