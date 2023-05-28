﻿#pragma once
#define MAPMAX 36
#define FLBMAX 4
class Main : public Scene
{
private:
	class Wall* map[MAPMAX];
	class Ball* Golfball;
	class Obstacle* floatingBall[FLBMAX];
	class ObRect* Goal;

	Vector2 mousepoint;
	Vector2 ballpoint;
	Vector2 point;
	Vector2 firepower;
	
	bool isStop= false;
	bool onClick = false;
	bool starting = false;
	bool gameover = false;
	float pow;

	bool ADMIN_MODE=false;
	

	
public:
	Main();
	~Main();
	virtual void Init() override;
	virtual void Release() override; //해제
	virtual void Update() override;
	virtual void LateUpdate() override;//갱신
	virtual void Render() override;
	virtual void ResizeScreen() override;
};
