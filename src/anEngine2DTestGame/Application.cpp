#include "Core/anApplication.h"
#include "Core/anEntryPoint.h"
#include "Renderer/anRenderer.h"
#include "Device/anGPUCommands.h"
#include "World/anWorld.h"
#include "World/anObject.h"
#include "World/anSpriteObject.h"
#include "World/anLineObject.h"
#include "World/anTextObject.h"
#include "Core/anMessage.h"
#include "Core/anKeyCodes.h"

class anEngine2DTestGameApplication : public anApplication
{
public:
	anEngine2DTestGameApplication()
		: anApplication({ "anEngine2D Test Game", 700, 650, false })
	{
	}

	~anEngine2DTestGameApplication()
	{
	}

	void Initialize() override
	{		
		mRaleway.Load("fonts/Raleway-Regular.ttf", 18);

		mWorld = new anWorld();

		mRenderer.Initialize();

		mfWidth = (float)mApplicationDesc.Width;
		mfHeight = (float)mApplicationDesc.Height;

		mProjection = anMatrix4::Ortho(mfWidth * -0.5f, mfWidth * 0.5f, mfHeight * 0.5f, mfHeight * -0.5f, -1.0f, 1.0f);
		mRenderer.SetMatrix(mProjection);

		mWorld->Initialize();
	}

	void Update(float dt) override
	{
		mWorld->Update(dt);

		anClear();
		anEnableBlend();
		
		mRenderer.Start();

		mWorld->Render(mRenderer);

		mRenderer.DrawString(mRaleway, { mfWidth * -0.5f, mfHeight * -0.5f + (float)mRaleway.GetSize() }, "FPS: " + anToString(mFramesPerSecond), { 255, 255, 255 });

		mRenderer.End();
	}
	
	void OnEvent(const anEvent& event) override
	{
	}

private:
	anRenderer mRenderer;

	anMatrix4 mProjection;

	anFont mRaleway;

	anWorld* mWorld = nullptr;

	float mfWidth = 0.0f;
	float mfHeight = 0.0f;
};

int anStartApplication(char** args, int argc)
{
	anEngine2DTestGameApplication* app = new anEngine2DTestGameApplication();
	app->Start();

	return 0;
}
