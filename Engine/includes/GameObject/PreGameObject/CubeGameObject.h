#pragma once
#include "lve_constants.h"
#include "Transform.h"
#include "GameObject/GameObject.h"

namespace lve
{
	/**
	 * @brief Repr�sente un objet cube dans le jeu.
	 *
	 * Cette classe repr�sente un objet cube dans le jeu. Un objet cube est un type d'objet g�om�trique tridimensionnel
	 * d�fini par ses dimensions et sa position dans l'espace tridimensionnel.
	 */
	class CubeGameObject
	{
		public:
			/**
			 * @brief Cr�e un nouvel objet cube.
			 *
			 * Cette m�thode statique cr�e un nouvel objet cube avec les param�tres sp�cifi�s.
			 *
			 * @param _lveDevice Une r�f�rence vers l'objet LveDevice utilis� pour cr�er l'objet cube.
			 * @param _position La position initiale de l'objet cube dans l'espace tridimensionnel (par d�faut : glm::vec3(0, 0, 0)).
			 * @param _scale L'�chelle initiale de l'objet cube (par d�faut : glm::vec3(1, 1, 1)).
			 * @param _rotation La rotation initiale de l'objet cube (par d�faut : glm::vec3(0, 0, 0)).
			 * @return Un pointeur vers le nouvel objet cube cr��.
			 */
			static GameObject* Create(LveDevice& _lveDevice, glm::vec3 _position = VEC3_ZERO,
			                          glm::vec3  _scale = VEC3_ONE, glm::vec3 _rotation = VEC3_ZERO)
				{
		const std::shared_ptr<LveModel> lve_model = LveModel::CreateModelFromFile(_lveDevice, "Models\\cube.obj");

		const auto game_object = GameObject::CreatePGameObject();
		game_object->SetModel(lve_model);
		game_object->GetTransform()->SetPosition(_position);
		game_object->GetTransform()->SetScale(_scale);
		game_object->GetTransform()->SetRotation(_rotation);

		return game_object;
	}


			/**
			 * @brief Cr�e un nouvel objet cube avec une couleur al�atoire.
			 *
			 * Cette m�thode statique cr�e un nouvel objet cube avec une couleur al�atoire pour chaque face.
			 *
			 * @param _lveDevice Une r�f�rence vers l'objet LveDevice utilis� pour cr�er l'objet cube.
			 * @param _position La position initiale de l'objet cube dans l'espace tridimensionnel (par d�faut : glm::vec3(0, 0, 0)).
			 * @param _scale L'�chelle initiale de l'objet cube (par d�faut : glm::vec3(1, 1, 1)).
			 * @param _rotation La rotation initiale de l'objet cube (par d�faut : glm::vec3(0, 0, 0)).
			 * @return Un pointeur vers le nouvel objet cube cr�� avec des couleurs al�atoires pour chaque face.
			 */
			static GameObject* CreateColor(LveDevice& _lveDevice, glm::vec3 _position = VEC3_ZERO,
			                               glm::vec3  _scale = VEC3_ONE, glm::vec3 _rotation = VEC3_ZERO)
				{
					const std::shared_ptr<LveModel> lve_model = LveModel::CreateModelFromFile(
					_lveDevice, "Models\\colored_cube.obj");

					const auto game_object = GameObject::CreatePGameObject();
					game_object->SetModel(lve_model);
					game_object->GetTransform()->SetPosition(_position);
					game_object->GetTransform()->SetScale(_scale);
					game_object->GetTransform()->SetRotation(_rotation);

		return game_object;
	}
	};
}
