#include "anApplication.h"
#include "Device/anShaders.h"
#include "Device/anTexture.h"
#include "anFont.h"
#include "State/anStateManager.h"

anApplication::anApplication(const anApplicationCreationDescription& desc)
	: mFramesPerSecond(0)
{
	memset(&mApplicationDesc, 0, sizeof(anApplicationCreationDescription));
	mApplicationDesc.Title = desc.Title;
	mApplicationDesc.Width = desc.Width;
	mApplicationDesc.Height = desc.Height;
}

anApplication::~anApplication()
{
}

void anApplication::Start()
{
	mStateManager = new anStateManager(this);

	auto onEvent = [this](const anEvent& event) { AOnEvent(event); };

	mWindow = anCreateWindow(mApplicationDesc.Title, mApplicationDesc.Width, mApplicationDesc.Height, onEvent, mApplicationDesc.WindowResizable);
	anTexture::Initialize();
	anFont::Initialize();
	anInitializeShaders();
	mStateManager->Initialize();
	Initialize();

	int fps = 0;
	float elapsedTime = 0.0f;
	while (mWindow->IsRunning())
	{
		const float dt = mTimer.Tick();
		elapsedTime += dt;
		if (elapsedTime >= 1.0f)
		{
			mFramesPerSecond = fps;
			elapsedTime = 0.0f;
			fps = 0;
		}
		++fps;

		mStateManager->Update(dt);
		Update(dt);
		mWindow->Present();
	}
}

void anApplication::AOnEvent(const anEvent& event)
{
	mStateManager->OnEvent(event);
	OnEvent(event);
}

void anApplication::Render(anRenderer& renderer)
{
	mStateManager->Render(renderer);
}

void anApplication::SetCurrentState(anState* state)
{
	mStateManager->SetCurrentState(state);
}

anState* anApplication::GetCurrentState()
{
	return mStateManager->GetCurrentState();
}

anWindow* anApplication::GetWindow()
{
	return mWindow;
}
