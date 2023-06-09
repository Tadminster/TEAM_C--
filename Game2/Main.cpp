﻿#include "stdafx.h"
#include "Wall.h"
#include "Ball.h"
#include "Ball_guideLine.h"
#include "Ball_trail.h"
#include "Obstacle.h"
#include "MapObject.h"
#include "Ending.h"
#include "Title.h"
#include "Main.h"

Main::Main()
{
	title = new Title();
	mapObj = new MapObject();
	ending = new Ending();
	golfBall = new Ball();
	for (int i = 0; i < 5; i++)
	{
		ball_guideLine[i] = new Ball_guideLine();
		ball_guideLine[i]->SetParentT(*golfBall);
		ball_guideLine[i]->color = Color(0.0f + i * 0.2f, 0.8f - i * 0.1f, 0.1f, 0.5f);
	}
	goal = new ObRect();
	goal_skin = new ObImage(L"goal_skin.png");
	goal_skin->SetParentRT(*goal);
}

Main::~Main()
{
	for (auto& guideLine : ball_guideLine)
		delete guideLine;
	delete golfBall;
	delete goal;
	
	delete ending;
	delete mapObj;
}

void Main::Init()
{
	title->Init();

	// 공
	golfBall->SetWorldPos(Vector2(-310, -240));

	// 공의 유도선
	for (int i = 0; i < 5; i++)
		ball_guideLine[i]->SetLocalPosX(30 + i * 40);

	// 맵의 오브젝트
	mapObj->Init();

	// 엔딩
	ending->Init();

	//골대
	goal->SetWorldPos(Vector2(0.0f, 2240.0f));
	goal->scale = Vector2(25.0f, 25.0f);
	goal->color = Vector4(0, 0, 0, 1);

	goal_skin->scale.x = goal->scale.x * 3.5f;
	goal_skin->scale.y = goal->scale.y * 3.0f;
}

void Main::Release()
{
}

void Main::Update()
{
	// Debug 출력 창
	{
		ImGui::Text("CAMERA_X: %f\n", CAM->position.x);
		ImGui::Text("CAMERA_Y: %f\n", CAM->position.y);
		ImGui::Text("\n");

		ImGui::Text("BALL_X: %f\n", golfBall->GetWorldPos().x);
		ImGui::Text("BALL_Y: %f\n", golfBall->GetWorldPos().y);
		ImGui::Text("BALL_RIGHT: %f\n", golfBall->GetRight());
		ImGui::Text("\n");

		ImGui::Text("MOUSE_X: %f\n", INPUT->GetWorldMousePos().x);
		ImGui::Text("MOUSE_Y: %f\n", INPUT->GetWorldMousePos().y);
	}

	if (!gameStart)
	{
		title->Update();

		int type = 9;  

		if (INPUT->KeyDown(VK_LBUTTON))
		{
			type = title->onClick();
		}

		if (type == 1)
		{
			gameStart = true;
			delete title;

			// 공
			golfBall->SetWorldPos(Vector2(-310, -50));
			golfBall->fire(Vector2(-200.0f, -100.0f));
		}
		if (type == 0)
			exit(1);
	}
	else
	{
		// Debug 출력 창
		/*{
			ImGui::Text("CAMERA_X: %f\n", CAM->position.x);
			ImGui::Text("CAMERA_Y: %f\n", CAM->position.y);
			ImGui::Text("\n");

			ImGui::Text("BALL_X: %f\n", golfBall->GetWorldPos().x);
			ImGui::Text("BALL_Y: %f\n", golfBall->GetWorldPos().y);
			ImGui::Text("BALL_RIGHT: %f\n", golfBall->GetRight());
			ImGui::Text("\n");

			ImGui::Text("MOUSE_X: %f\n", INPUT->GetWorldMousePos().x);
			ImGui::Text("MOUSE_Y: %f\n", INPUT->GetWorldMousePos().y);
		}*/

		// 카메라가 공에 따라가게
		CAM->position.y = golfBall->GetWorldPos().y + 100.0f;
		CAM->position.x = golfBall->GetWorldPos().x;
		CAM->position.x = Utility::Saturate(CAM->position.x, -100.0f, 100.0f);
		CAM->position.y = Utility::Saturate(CAM->position.y, -200.0f, 2000.0f);
		
		// 맵의 오브젝트
		mapObj->Update();

		// 엔딩
		ending->Update(golfBall);

		// 관리자 모드
		{
			if (INPUT->KeyPress('Z'))
			{
				golfBall->hasAxis = true;
				ADMIN_MODE = true;
			}
			if (INPUT->KeyPress('X'))
			{
				golfBall->hasAxis = false;
				ADMIN_MODE = false;
			}

			if (ADMIN_MODE)
			{
				//공이동			
				if (INPUT->KeyPress('W'))
				{
					golfBall->MoveWorldPos(UP * 500.0f * DELTA);
				}
				if (INPUT->KeyPress('S'))
				{
					golfBall->MoveWorldPos(DOWN * 500.0f * DELTA);
				}
				if (INPUT->KeyPress('A'))
				{
					golfBall->MoveWorldPos(LEFT * 500.0f * DELTA);
				}
				if (INPUT->KeyPress('D'))
				{
					golfBall->MoveWorldPos(RIGHT * 500.0f * DELTA);
				}
				golfBall->gravityForce = 0.0f;
				golfBall->pressPower = 0.0f;
				golfBall->Update();
			}
		}





		// 궤적 관련
		{
			//ImGui::Text("TRAIL COUNT: %d\n", trail.size());
			// 궤적 생성
			if (!golfBall->isStop)
			{
				trail.push_back(Ball_trail(golfBall->GetWorldPos()));
				for (auto& trail : this->trail)
					trail.Update();
			}

			// 궤적의 지속시간이 경과되면 삭제
			trail.erase
			(
				std::remove_if
				(
					trail.begin(),
					trail.end(),
					[](Ball_trail& t) { return t.timeOut(); }
				),
				trail.end()
			);
		}
		goal->Update();
		goal_skin->Update();

	}
	
	// 공의 클릭 관련
	{
		// 공이 멈췄을 때
		if (golfBall->stopcheck())
		{
			golfBall->color = Color(1.f, 1.f, 1.f, 1.f);	// 색상을 검정으로

			// 마우스가 공위에 있으면
			if (golfBall->Intersect(INPUT->GetWorldMousePos()))
			{
				// 좌클을 누르면
				if (INPUT->KeyDown(VK_LBUTTON))
				{
					onClick = true;						// 클릭 신호를 주고
					point = golfBall->GetWorldPos();	// 공의 위치를 저장
				}
			}
			// 공을 클릭했으면
			if (onClick)
			{
				// 유도선을 그려줌
				for (int i = 0; i < 5; i++)
				{
					Vector2 direction;
					if (INPUT->GetWorldMousePos().x > 2000 || INPUT->GetWorldMousePos().x < -2000)
						direction = Vector2(-2000 - point.x, INPUT->GetWorldMousePos().y - point.y);
					else if (INPUT->GetWorldMousePos().y > 2000 || INPUT->GetWorldMousePos().y < -2000)
						direction = Vector2(INPUT->GetWorldMousePos().x - point.x, 2000 - point.y);
					else
						direction = INPUT->GetWorldMousePos() - point;

					ball_guideLine[i]->rotation2.z = atan2f(direction.y, direction.x) + PI;		// 유도선 방향 계산
					ball_guideLine[i]->Update();
				}
				// 클릭을 떼면 발사
				if (INPUT->KeyUp(VK_LBUTTON))
				{
					firepower = point - INPUT->GetWorldMousePos();
					golfBall->fire(firepower);
					golfBall->Update();
					onClick = false;
				}
			}
		}
		else
		{
			if (gameStart)
			{
				golfBall->color = Color(0.5f, 0.5f, 0.5f, 1.0f);
				golfBall->Update();
			}
		}

		if (starting == 0 && gameStart) golfBall->Update();
		}
}

void Main::LateUpdate()
{
	if (!gameStart)
	{
		title->LateUpdate();
	}
	else
	{
		// 임시벽
		if (golfBall->GetWorldPos().y < -300.0f + 10.0f)
		{
			golfBall->SetWorldPosY(-300.0f + 10.0f);
			golfBall->ReflectionX();
			golfBall->Update();
		}

		if (golfBall->GetWorldPos().x < -400.0f + 10.0f)
		{
			golfBall->SetWorldPosX(-400.0f + 10.0f);
			golfBall->ReflectionY();
			golfBall->Update();
			starting = true;
		}
		else if (golfBall->GetWorldPos().x > 400.0f - 10.0f)
		{
			golfBall->SetWorldPosX(400.0f - 10.0f);
			golfBall->ReflectionY();
			golfBall->Update();
		}


		//벽충돌
		for (int i = 0; i < MAPMAX; i++)
		{
			mapObj->map[i]->Collision(golfBall);
		}
		//방해물충돌
		for (int i = 0; i < FLBMAX; i++)
		{
			if (mapObj->floatingBall[i]->Intersect(golfBall)) golfBall->ReflectionBall(mapObj->floatingBall[i]);
		}
		//골대충돌
		if (goal->Intersect(golfBall) && !isGameOver)
		{
			ADMIN_MODE = false;
			golfBall->hasAxis = false;
			golfBall->stop = true;
			isGameOver = true;
			ending->stage[0] = true;
		}
	}
}

void Main::Render()
{
	if (!gameStart)
	{
		title->Render();
	}
	else
	{
		mapObj->Render();
		ending->Render();
		for (int i = 0; i < MAPMAX; i++)
		{
			golfBall->Render();

			if (onClick)
				for (int i = 0; i < 5; i++)
				{
					Vector2 distance = (golfBall->GetWorldPos() - INPUT->GetWorldMousePos());
					//ImGui::Text("d: %f\n", distance.Length());
					//ImGui::Text("d: %f\n", distance.Length() / 60.f);
					if (distance.Length() / 100.f > i)
					{
						ball_guideLine[i]->Render();
					}
				}
		}

		for (auto& trail : this->trail)
			trail.Render();

		//goal->Render();
		goal_skin->Render();
	}
}

void Main::ResizeScreen()
{
	
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prevInstance, LPWSTR param, int command)
{
	app.SetAppName(L"Game2");
	app.SetInstance(instance);
	app.InitWidthHeight(800.0f, 600.0f);
	WIN->Create();
	Main* main = new Main();
	int wParam = (int)WIN->Run(main);
	SafeDelete(main);
	WIN->Destroy();
	WIN->DeleteSingleton();
	
	return wParam;
}