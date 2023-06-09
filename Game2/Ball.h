﻿#pragma once
class Ball : public ObCircle
{
public:
	Vector2		fireDir;
	float		pressPower;
	Vector2		velocity;
	float		gravityForce;
	Vector2		gravityDir;
	
	bool stop;
	bool isStop;

public:
	Ball();
	virtual ~Ball();
	void Update() override;
	void Render() override;

	float dotProduct(Vector2 v1, Vector2 v2) { return v1.x * v2.x + v1.y * v2.y; }

	void fire(Vector2 dir);

	bool stopcheck( ) ;
	void SetgravityForve(float GF) { gravityForce = GF; };

	void Reflection(Vector2 dis);
	void ReflectionY()
	{
		fireDir.x = -fireDir.x;
		
		pressPower *= 0.75f;
		gravityForce *= 0.75f;
	}
	void ReflectionX()
	{
		fireDir.y = -fireDir.y;
		gravityForce = -gravityForce;
		
		pressPower *= 0.75f;
		gravityForce *= 0.75f;
	}
	void ReflectionXY()
	{
		fireDir = -fireDir;		
		pressPower *= 0.5f;
		gravityForce *= 0.5f;
	}

	void ReflectionBall(GameObject* ob)
	{
		Vector2 dir;
		dir = this->GetWorldPos() - ob->GetWorldPos();
		dir.Normalize();
		fireDir = dir;
		
	}
};

