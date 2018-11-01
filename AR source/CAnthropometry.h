/*  Autorigger, tool that allows to create a skeleton and
	attach it to a three-dimensional model of a human body.
    Copyright (C) 2018  Michał Wiśniewski

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once

#include "stdafx.h"
#include "SContainers.h"


class CAnthropometry
{
public:
	CAnthropometry(std::vector<vertex>);
	~CAnthropometry();
	std::vector<glm::vec3> getBonePositions();
private:
	std::vector<vertex> vertices;
	std::vector<glm::vec2> x;
	std::vector<glm::vec2> y;
	std::vector<float> z;

	void sliceMesh();
	void genBasicData();

	void genCraniumPoint();
	void genNeckPoint();
	void genCollarPoint();
	void genMSpinePoint();
	void genLSpinePoint();
	void genHipPoint();
	void genThighPoints();
	void genKneePoints();
	void genAnklePoints();
	void genFeetPoints();
	void genShoulderPoints();
	void genArmPoints();
	void genWristPoints();
	void genHandPoints();

	int shoulder_H;
	int hand_H;
	float max_torso_width;
	float minX;
	float maxX;
	float minY;
	float maxY;
	float minZ;
	float maxZ;

	/*point IDs in bonePos vector
	craniumBonePos			0
	neckBonePos				1
	collarBonePos			2
	MSpineBonePos			3
	lSpineBonePos			4
	HipBonePos				5
	leftThighBonePos		6
	rightThighBonePos		7
	leftKneeBonePos			8
	rightKneeBonePos		9
	leftAnkleBonePos		10
	rightAnkleBonePos		11
	leftFootBonePos			12
	rightFootBonePos		13
	leftShoulderBonePos		14
	rightShoulderBonePos	15
	leftArmBonePos			16
	rightArmBonePos			17
	leftWristBonePos		18
	rightWristBonePos		19
	leftHandBonePos			20
	rightHandBonePos		21
	*/
	std::vector<glm::vec3> bonePos;
};