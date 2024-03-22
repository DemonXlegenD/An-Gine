#pragma once
#include <cassert>

class CollisionManager
{
public:
	static CollisionManager& get() {
		static CollisionManager* gpSingleton = NULL;
		if (gpSingleton == NULL) {
			gpSingleton = new CollisionManager;
		}
		assert(gpSingleton);
		return *gpSingleton;
	}

	CollisionManager()
	{
		// nothing
	}

	~CollisionManager()
	{
		// nothing
	}

	void Awake() {}

	void Create() {}

	void OnCollisionEnter() {}

	void Update() {}

	void start() {}

	void stop() {}
};