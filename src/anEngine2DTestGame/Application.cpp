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
#include "Math/anMath.h"

#include <math.h>
#include <time.h>

struct Rect
{
	anFloat2 p0;
	anFloat2 p1;
	anFloat2 p2;
	anFloat2 p3;

	Rect()
	{
	}

	void SetTransform(const anFloat2& pos, const anFloat2& size, float rot)
	{
		anMatrix4 transform =
			anMatrix4::Translate({ pos.X, pos.Y, 0.0f }) *
			anMatrix4::Scale({ size.X, size.Y, 0.0f }) *
			anMatrix4::Rotate(rot, { 0.0f, 0.0f, 1.0f });

		p0 = anFloat2(-0.5f, -0.5f) * transform;
		p1 = anFloat2(0.5f, -0.5f) * transform;
		p2 = anFloat2(0.5f, 0.5f) * transform;
		p3 = anFloat2(-0.5f, 0.5f) * transform;
	}

	void SetTransform(const anFloat2& pos, const anFloat2& size)
	{
		p0 = { pos.X - size.X * 0.5f, pos.Y - size.Y * 0.5f };
		p1 = { pos.X + size.X * 0.5f, pos.Y - size.Y * 0.5f };
		p2 = { pos.X + size.X * 0.5f, pos.Y + size.Y * 0.5f };
		p3 = { pos.X - size.X * 0.5f, pos.Y + size.Y * 0.5f };
	}
};

static bool PointIntersects(const anFloat2& p0, const Rect& rect)
{
	return p0.X >= rect.p0.X && p0.X <= rect.p2.X && p0.Y >= rect.p0.Y && p0.Y <= rect.p2.Y;
}

class anEngine2DTestGameApplication : public anApplication
{
public:
	anEngine2DTestGameApplication()
		: anApplication({ "anEngine2D Test Game", 450, 650, true })
	{
	}

	~anEngine2DTestGameApplication()
	{
	}

	void Initialize() override
	{		
		srand(time(0));

		mRaleway.Load("fonts/Raleway-Regular.ttf", 24);

		mWorld = new anWorld();

		mRenderer.Initialize();

		mfWidth = (float)mApplicationDesc.Width;
		mfHeight = (float)mApplicationDesc.Height;

		mProjection = anMatrix4::Ortho(mfWidth * -0.5f, mfWidth * 0.5f, mfHeight * 0.5f, mfHeight * -0.5f, -1.0f, 1.0f);
		mRenderer.SetMatrix(mProjection);

		mMenuScene = new anScene();

		const anString descText = "Press Enter to Start the Game\n\n  Press Escape to Exit Game";
		mMenuText = new anTextObject("Menu Scene Desc Text");
		mMenuText->SetFont(mRaleway);
		mMenuText->SetText(descText);
		mMenuText->SetPosition({ (descText.size() * (float)mRaleway.GetSize()) * -0.125f, (float)mRaleway.GetSize() });

		mMenuScene->AddObject(mMenuText);

		mGameScene = new anScene();

		mPlayerPosition = { 0.0f, -90.0f };
		mPlayer = new anSpriteObject("Player");
		mPlayer->SetTexture(anTexture::GetWhiteTexture());
		mPlayer->SetColor({ 84, 18, 102 });
		mPlayer->SetSize({ 40.0f, 40.0f });
		mPlayer->SetPosition(mPlayerPosition);
		mPlayerRect.SetTransform(mPlayer->GetPosition(), mPlayer->GetSize(), mPlayer->GetRotation());

		mScoreText = new anTextObject("Score Text");
		mScoreText->SetFont(mRaleway);
		mScoreText->SetText(anToString(mGameScore));
		mScoreText->SetPosition({ (float)mRaleway.GetSize() * 0.25f, (float)mRaleway.GetSize() * -10.0f });

		mBlocks0X = mfWidth * 0.5f + mBlocksSize * 0.5f;
		mBlock0 = new anSpriteObject("Block 0");
		mBlock1 = new anSpriteObject("Block 1");
		mBlock0->SetTexture(anTexture::GetWhiteTexture());
		mBlock1->SetTexture(anTexture::GetWhiteTexture());
		mBlock0->SetPosition({ mBlocks0X, 0.0f });
		mBlock1->SetPosition({ mBlocks0X, 0.0f });
		mBlock0->SetColor(mBlocksColor);
		mBlock1->SetColor(mBlocksColor);
		mBlock0Rect.SetTransform(mBlock0->GetPosition(), mBlock0->GetSize());
		mBlock1Rect.SetTransform(mBlock1->GetPosition(), mBlock1->GetSize());
		
		mGameScene->AddObject(mBlock0);
		mGameScene->AddObject(mBlock1);
		mGameScene->AddObject(mPlayer);
		mGameScene->AddObject(mScoreText);

		mWorld->SetCurrentScene(mMenuScene);
		mWorld->Initialize();
	}

	void Update(float dt) override
	{
		mWorld->Update(dt);

		if (mWorld->GetCurrentScene() == mGameScene && !mGameFinished)
		{
			if (mBlocks0X + mBlocksSize * 0.5f <= mfWidth * -0.5f)
			{
				float x = GenerateRandomFloat(230.0f, 280.0f);
				float y = GenerateRandomFloat(230.0f, 280.0f);
				
				mBlock0->SetSize({ mBlocksSize, x });
				mBlock1->SetSize({ mBlocksSize, y });
			}

			if (mBlocks0X == mfWidth * 0.5f + mBlocksSize * 0.5f)
			{
				float x = GenerateRandomFloat(230.0f, 280.0f);
				float y = GenerateRandomFloat(230.0f, 280.0f);

				mBlock0->SetSize({ mBlocksSize, x });
				mBlock1->SetSize({ mBlocksSize, y });
			}

			if (mBlocks0X + mBlocksSize * 0.5f <= mfWidth * -0.5f)
				mBlocks0X = mfWidth * 0.5f + mBlocksSize * 0.5f;

			mBlocks0X -= 2.5f;

			mBlock0->SetPosition({ mBlocks0X, mfHeight * -0.5f + mBlock0->GetSize().Y * 0.5f});
			mBlock1->SetPosition({ mBlocks0X, mfHeight * 0.5f + mBlock1->GetSize().Y * -0.5f});

			mBlock0Rect.SetTransform(mBlock0->GetPosition(), mBlock0->GetSize());
			mBlock1Rect.SetTransform(mBlock1->GetPosition(), mBlock1->GetSize());

			mPointRect.p0 = mBlock0Rect.p3;
			mPointRect.p1 = mBlock0Rect.p2;
			mPointRect.p2 = mBlock1Rect.p1;
			mPointRect.p3 = mBlock1Rect.p0;
			if (PointIntersects(mPlayerRect.p0, mPointRect) ||
				PointIntersects(mPlayerRect.p1, mPointRect) ||
				PointIntersects(mPlayerRect.p2, mPointRect) ||
				PointIntersects(mPlayerRect.p3, mPointRect))
			{
				if (mIncreaseScore)
					IncreaseGameScore();

				mIncreaseScore = false;
			}
			else
				mIncreaseScore = true;

			mPlayerPositionDeltaY += mPlayerFallPower;
			mPlayerPosition.Y += mPlayerPositionDeltaY;

			if (mPlayerPositionDeltaY > 0.0f)
				mPlayerDeltaRotation = -2.0f;

			if (mPlayerRotation > -25.0f)
				mPlayerRotation += mPlayerDeltaRotation;
			
			if (mPlayerRect.p0.Y <= mfHeight * -0.5f || mPlayerRect.p1.Y <= mfHeight * -0.5f ||
				mPlayerRect.p2.Y <= mfHeight * -0.5f || mPlayerRect.p3.Y <= mfHeight * -0.5f ||
				mPlayerRect.p0.Y >= mfHeight * 0.5f || mPlayerRect.p1.Y >= mfHeight * 0.5f ||
				mPlayerRect.p2.Y >= mfHeight * 0.5f || mPlayerRect.p3.Y >= mfHeight * 0.5f ||
				PointIntersects(mPlayerRect.p0, mBlock0Rect) ||
				PointIntersects(mPlayerRect.p0, mBlock1Rect) ||
				PointIntersects(mPlayerRect.p1, mBlock0Rect) ||
				PointIntersects(mPlayerRect.p1, mBlock1Rect) ||
				PointIntersects(mPlayerRect.p2, mBlock0Rect) ||
				PointIntersects(mPlayerRect.p2, mBlock1Rect) ||
				PointIntersects(mPlayerRect.p3, mBlock0Rect) ||
				PointIntersects(mPlayerRect.p3, mBlock1Rect))
			{
				mGameFinished = true;
			
				mScoreText->SetText("\t\t\t\tThe End\n\n\n\t\t\tYour Score: " + anToString(mGameScore) + "\nPress Enter to Return Menu");
				mScoreText->SetPosition({ -6.0f * (float)mRaleway.GetSize(), 0.0f});
			}

			mPlayer->SetPosition(mPlayerPosition);
			mPlayer->SetRotation(mPlayerRotation);

			mPlayerRect.SetTransform(mPlayer->GetPosition(), mPlayer->GetSize(), mPlayer->GetRotation());
		}

		anClearColor(mBackgroundColor);
		anEnableBlend();
		
		mRenderer.Start();

		mWorld->Render(mRenderer);

		mRenderer.DrawString(mRaleway, { mfWidth * -0.5f, mfHeight * -0.5f + (float)mRaleway.GetSize() }, "FPS: " + anToString(mFramesPerSecond), { 255, 255, 255 });

		mRenderer.End();
	}
	
	void OnEvent(const anEvent& event) override
	{
		if (event.Type == anEvent::KeyDown)
		{
			if (event.KeyCode == anKeyEscape)
				mWindow->Close();
		}

		if (mWorld->GetCurrentScene() == mMenuScene)
		{
			if (event.Type == anEvent::KeyDown)
			{
				if (event.KeyCode == anKeyEnter)
				{
					// reset all data
					mWorld->SetCurrentScene(mGameScene);
					mGameFinished = false;
					mPlayerRotation = 0.0f;
					mPlayerDeltaRotation = 0.0f;
					mPlayerPosition = { 0.0f, 0.0f };
					mPlayerPositionDeltaY = 0.0f;
					mPlayer->SetPosition(mPlayerPosition);
					mPlayer->SetRotation(mPlayerRotation);
					mBlocks0X = mfWidth * 0.5f + mBlocksSize * 0.5f;
					mBlock0->SetPosition({ mBlocks0X, 0.0f });
					mBlock1->SetPosition({ mBlocks0X, 0.0f });
					mGameScore = 0;
					mScoreText->SetText(anToString(mGameScore));
					mScoreText->SetPosition({ (float)mRaleway.GetSize() * 0.25f, (float)mRaleway.GetSize() * -10.0f });
					mPlayerRect.SetTransform(mPlayer->GetPosition(), mPlayer->GetSize(), mPlayer->GetRotation());
					mBlock0Rect.SetTransform(mBlock0->GetPosition(), mBlock0->GetSize());
					mBlock1Rect.SetTransform(mBlock1->GetPosition(), mBlock1->GetSize());
				}
			}
		}

		if (mWorld->GetCurrentScene() == mGameScene)
		{
			if (event.Type == anEvent::KeyDown)
			{
				if (mGameFinished)
				{
					if (event.KeyCode == anKeyEnter)
						mWorld->SetCurrentScene(mMenuScene);
				}

				if (event.KeyCode == anKeySpace && !mGameFinished)
				{
					mPlayerPositionDeltaY = -4.0f;
					mPlayerRotation = 20.0f;
				}
			}
		}
	}

	void IncreaseGameScore()
	{
		++mGameScore;
		mScoreText->SetText(anToString(mGameScore));
	}

	float GenerateRandomFloat(float min, float max)
	{
		int iRand = rand();

		return min + ((float)iRand / (float)RAND_MAX) * (max - min);
	}

private:
	anRenderer mRenderer;

	anMatrix4 mProjection;

	anFont mRaleway;

	anWorld* mWorld = nullptr;

	anScene* mMenuScene = nullptr;
	anTextObject* mMenuText = nullptr;

	anScene* mGameScene = nullptr;
	bool mGameFinished = false;
	int mGameScore = 0;
	anTextObject* mScoreText = nullptr;
	bool mIncreaseScore = false;

	// Player
	anSpriteObject* mPlayer = nullptr;
	anFloat2 mPlayerPosition;
	float mPlayerRotation = 0.0f;
	float mPlayerPositionDeltaY = 0.0f;
	float mPlayerFallPower = 0.09f;
	float mPlayerDeltaRotation = 0.0f;
	Rect mPlayerRect;
	// ------------

	// Blocks
	anSpriteObject* mBlock0 = nullptr;
	anSpriteObject* mBlock1 = nullptr;
	Rect mBlock0Rect;
	Rect mBlock1Rect;
	float mBlocks0X = 0.0f;
	float mBlocksSize = 50.0f;
	anColor mBlocksColor = { 192, 58, 72 };
	Rect mPointRect;
	// ------------

	float mfWidth = 0.0f;
	float mfHeight = 0.0f;

	anColor mBackgroundColor = { 57, 137, 86 };
};

int anStartApplication(char** args, int argc)
{
	anEngine2DTestGameApplication* app = new anEngine2DTestGameApplication();
	app->Start();

	return 0;
}
