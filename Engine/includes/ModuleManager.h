#pragma once

#include <vector>
#include "Modules/Module.h"

/**
 * @brief Classe ModuleManager.
 *
 * Cette classe repr�sente le gestionnaire de modules dans le syst�me.
 * Le ModuleManager est responsable de la cr�ation, de la gestion et de la communication entre les diff�rents modules de l'application.
 */
class ModuleManager
{
	public:
		ModuleManager() = default;
		~ModuleManager();

		/**
		 * @brief Cr�e les modules par d�faut.
		 */
		void CreateDefaultModules();

#pragma region Event
		/**
		 * @brief Initialise le module.
		 */
		void Init()
		{
		}

		/**
		 * @brief D�marre le module.
		 */
		void Start()
		{
		}

		/**
		 * @brief Effectue une mise � jour fixe du module.
		 */
		void FixedUpdate()
		{
		}

		/**
		 * @brief Met � jour le module.
		 */
		void Update()
		{
		}

		/**
		 * @brief Fonction pr�-rendu du module.
		 */
		void PreRender()
		{
		}

		/**
		 * @brief Rendu du module.
		 */
		void Render()
		{
		}

		/**
		 * @brief Rendu de l'interface graphique du module.
		 */
		void RenderGui()
		{
		}

		/**
		 * @brief Fonction post-rendu du module.
		 */
		void PostRender()
		{
		}

		/**
		 * @brief Lib�re les ressources utilis�es par le module.
		 */
		void Release()
		{
		}

		/**
		 * @brief Finalise le module.
		 */
		void Finalize()
		{
		}

#pragma endregion

		/**
		 * @brief Cr�e une instance du module de type T.
		 * @return Un pointeur vers le module cr��.
		 */
		template <class T>
		T* CreateModule();

		/**
		 * @brief Obtient une instance du module de type T.
		 * @return Un pointeur vers le module obtenu, ou nullptr si le module n'existe pas.
		 */
		template <class T>
		T* GetModule();

	private:
		std::vector<Module*> modules; /**< Vecteur contenant des pointeurs vers les modules. */
};

template <class T>
T* ModuleManager::CreateModule()
{
	T* module            = new T();
	module->moduleManager = this;
	modules.push_back(module);
	return module;
}

template <class T>
T* ModuleManager::GetModule()
{
	for (Module*& module : modules)
	{
		if (T* module_cast = dynamic_cast<T*>(module)) return module_cast;
	}
	return nullptr;
}
