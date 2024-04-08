#include "Modules/SoundSystemModule.h"
#include <fmod_errors.h>
#include <string>

SoundSystemModule::SoundSystemModule()
{
	if (System_Create(&system) != FMOD_OK)
	{
		std::cout << "une erreur s'est produit lors de la cr�ation du Syst�me de FMOD" << std::endl;
		return;
	}

	int driverCount = 0;
	system->getNumDrivers(&driverCount);

	if (driverCount == 0)
	{
		std::cout << "FMOD n'a pas trouv� de sortie de son" << std::endl;
		return;
	}

	// Permet d'init le nombre de musique jou� simultan�ment
	system->init(36, FMOD_LOOP_OFF, nullptr);
}

// Permet la cr�ation du son avec son chemin d'acc�s //
void SoundSystemModule::CreateSound(SoundClass* p_Sound, const char* p_pathAudio) const
{
	const FMOD_RESULT result = system->createSound(p_pathAudio, FMOD_LOOP_OFF, nullptr, p_Sound);

	if (result != FMOD_OK) std::cout << "le son n'a pas charge : " << FMOD_ErrorString(result) << std::endl;
}

// Permet la cr�ation d'un groupe de son. //
void SoundSystemModule::CreateSoundGroup(SoundGroup* p_pSoundGroup, const char* p_pathAudio) const
{
	if (const FMOD_RESULT result = system->createSoundGroup(p_pathAudio, p_pSoundGroup);
		result != FMOD_OK) std::cout << "le groupe n'a pas charge : " << FMOD_ErrorString(result) << std::endl;
}

// Permet la r�cup�ration d'un groupe de son. //
void SoundSystemModule::GetMasterSoundGroup(SoundGroup* p_pSound) const
{
	if (const FMOD_RESULT result = system->getMasterSoundGroup(p_pSound);
		result != FMOD_OK) std::cout << "le groupe n'a pas charge : " << FMOD_ErrorString(result) << std::endl;
}

// Permet de jou� un son sp�cifique avec ces propres param�tres. //
void SoundSystemModule::PlaySound(const SoundClass _pSound, const bool _isPlay, const int _loopCount,
                                  const float      _volume, Channel*   p_channelPtr) const
{
	FMOD::Channel* channel = nullptr;

	if (!_isPlay)
	{
		_pSound->setMode(FMOD_LOOP_OFF);
	}
	else
	{
		_pSound->setMode(FMOD_LOOP_NORMAL);
		_pSound->setLoopCount(_loopCount);
	}

	system->playSound(_pSound, channelGroup, false, &channel);

	channel->setVolume(_volume);

	// Affecter le canal cr�� au pointeur de pointeur pass� en param�tre
	*p_channelPtr = channel;
}

// Permet de lib�rer la m�moire et de couper le son. //
void SoundSystemModule::ReleaseSound(const SoundClass _pSound)
{
	_pSound->release();
}

// Permet d'obtenir le nombre total de tour sur le son //
int SoundSystemModule::GetLoopCount(const SoundClass _pSound, int* p_loopCount)
{
	return _pSound->getLoopCount(p_loopCount);
}

// Permet la cr�ation d'un channel de groupe. //
void SoundSystemModule::CreateChannelGroup(FMOD::ChannelGroup** channelGroup) const
{
	system->createChannelGroup(nullptr, channelGroup);
}

// Permet d'ajouter un son � un channel de groupe pour pouvoir modifier les param�tres du son du tout les sons du channel. //
void SoundSystemModule::AddSoundToGroup(const SoundClass _pSound, FMOD::ChannelGroup* p_channelGroup) const
{
	system->playSound(_pSound, p_channelGroup, false, nullptr);
}

// Impl�mentation des fonctions pour le routing vers les ports et la gestion de la r�verb�ration //
FMOD_RESULT SoundSystemModule::AttachChannelGroupToPort(const FMOD_PORT_TYPE _portType,
                                                        const FMOD_PORT_INDEX _portIndex,
                                                        FMOD::ChannelGroup* p_channelGroup, const bool _passThru) const
{
	return system->attachChannelGroupToPort(_portType, _portIndex, p_channelGroup, _passThru);
}

FMOD_RESULT SoundSystemModule::DetachChannelGroupFromPort(FMOD::ChannelGroup* p_channelGroup) const
{
	return system->detachChannelGroupFromPort(p_channelGroup);
}

FMOD_RESULT SoundSystemModule::SetReverbProperties(const int _instance, const FMOD_REVERB_PROPERTIES* p_prop) const
{
	return system->setReverbProperties(_instance, p_prop);
}

FMOD_RESULT SoundSystemModule::GetReverbProperties(const int _instance, FMOD_REVERB_PROPERTIES* p_prop) const
{
	return system->getReverbProperties(_instance, p_prop);
}

void SoundSystemModule::Init()
{
	Module::Init();
}

void SoundSystemModule::Start()
{
	Module::Start();
}

void SoundSystemModule::FixedUpdate()
{
	Module::FixedUpdate();
}

void SoundSystemModule::Update()
{
	Module::Update();
}

void SoundSystemModule::PreRender()
{
	Module::PreRender();
}

void SoundSystemModule::Render()
{
	Module::Render();
}

void SoundSystemModule::RenderGui()
{
	Module::RenderGui();
}

void SoundSystemModule::PostRender()
{
	Module::PostRender();
}

void SoundSystemModule::Release()
{
	Module::Release();
}

void SoundSystemModule::Finalize()
{
	Module::Finalize();
}