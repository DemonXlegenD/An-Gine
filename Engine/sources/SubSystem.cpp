#include "SubSystem.h"

// premi�re option pour instancier le manager //


// deuxi�me option pour instancier le manager //
std::unordered_map<std::type_index, void*> SubSystem::instances;

template <class T>
void SubSystem::Set(T* instance)
{
	instances[typeid(T)] = instance;
}

template <class T>
T* SubSystem::Get()
{
	auto it = instances.find(typeid(T));
	if (it != instances.end()) return static_cast<T*>(it->second);
	return nullptr;
}

void startUp()
{
}
