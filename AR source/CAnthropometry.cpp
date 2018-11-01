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
#include "CAnthropometry.h"

CAnthropometry::CAnthropometry(std::vector<vertex> vert)
{
	vertices = vert;

	minX = 999.0;
	maxX = -999.0;
	minY = 999.0;
	maxY = -999.0;
	minZ = 999.0;
	maxZ = -999.0;

	sliceMesh();
	genBasicData();

	genCraniumPoint();
	genNeckPoint();
	genCollarPoint();
	genMSpinePoint();
	genLSpinePoint();
	genHipPoint();
	genThighPoints();
	genKneePoints();
	genAnklePoints();
	genFeetPoints();
	genShoulderPoints();
	genHandPoints();
	genArmPoints();
	genWristPoints();
}

CAnthropometry::~CAnthropometry()
{
	
}

void CAnthropometry::sliceMesh()
{
	for (int i = 0; i < 20; i++)
	{
		x.push_back(glm::vec2(-9999.0, 9999.0));
		y.push_back(glm::vec2(-9999.0, 9999.0));
		z.push_back(i * 10 + 5);
	}

	for (int i = 0; i < vertices.size(); i++)
	{
		if ((int)(vertices[i].pos.z * 200.0 / 20.0) < 20.0)
		{
			if (x[(int)(vertices[i].pos.z * 200.0 / 20.0)].x < vertices[i].pos.x)	x[(int)(vertices[i].pos.z * 200.0 / 20.0)].x = vertices[i].pos.x; //xmax
			if (x[(int)(vertices[i].pos.z * 200.0 / 20.0)].y > vertices[i].pos.x)	x[(int)(vertices[i].pos.z * 200.0 / 20.0)].y = vertices[i].pos.x; //xmin
			if (y[(int)(vertices[i].pos.z * 200.0 / 20.0)].x < vertices[i].pos.y)	y[(int)(vertices[i].pos.z * 200.0 / 20.0)].x = vertices[i].pos.y; //ymax
			if (y[(int)(vertices[i].pos.z * 200.0 / 20.0)].y > vertices[i].pos.y)	y[(int)(vertices[i].pos.z * 200.0 / 20.0)].y = vertices[i].pos.y; //ymin
			if (vertices[i].pos.x < minX)	minX = vertices[i].pos.x;
			if (vertices[i].pos.x > maxX)	maxX = vertices[i].pos.x;
			if (vertices[i].pos.y < minY)	minY = vertices[i].pos.y;
			if (vertices[i].pos.y > maxY)	maxY = vertices[i].pos.y;
			if (vertices[i].pos.z < minZ)	minZ = vertices[i].pos.z;
			if (vertices[i].pos.z > maxZ)	maxZ = vertices[i].pos.z;
		}
	}
}

void CAnthropometry::genBasicData()
{
	float lastLenght = glm::abs(y[19].x) + glm::abs(y[19].y);
	for (int i = 18; i > 15; i--)
	{		
		if (glm::abs(y[i].x) + glm::abs(y[i].y) < lastLenght * 1.4)
		{
			lastLenght = glm::abs(y[i].x) + glm::abs(y[i].y);
		}
		else
		{
			shoulder_H = i;
			break;
		}
	}

	lastLenght = glm::abs(y[shoulder_H].x) + glm::abs(y[shoulder_H].y);
	for (int i = shoulder_H-1; i > 5; i--)
	{
		if (glm::abs(y[i].x) + glm::abs(y[i].y) * 1.5 > lastLenght)
		{
			lastLenght = glm::abs(y[i].x) + glm::abs(y[i].y);
		}
		else
		{
			hand_H = i + 1;
			max_torso_width = 1.5 * glm::abs(y[i].x) + glm::abs(y[i].y);
			break;
		}
	}
	bonePos.resize(22);

	qDebug() << "Shoulder slice :" << shoulder_H << "\nHand slice :" << hand_H << "\nMax body width :" << max_torso_width * 100.0;
}

//Generate antropometirc data of bones
void CAnthropometry::genCraniumPoint()
{
	glm::vec3 vSum = glm::vec3(0);;
	int iSum = 0;

	for (int i = 0; i < vertices.size(); i++)
	{
		if (vertices[i].pos.z > (z[19] - 5)/100.0)
		{
			vSum += vertices[i].pos;
			iSum++;
		}
	}
	bonePos[0] = glm::vec3(vSum.x / iSum, vSum.y / iSum, z[19]/100);

	qDebug() << "CAnthropometry: Cranium bone pos:" << bonePos[0].x * 100.0 << bonePos[0].y * 100.0 << bonePos[0].z * 100.0;
}

void CAnthropometry::genNeckPoint()
{
	float height = minZ + ((z[shoulder_H + 1] + 5)/ 100.0);

	bonePos[1] = glm::vec3(0, (minY + maxY) / 2.0, height);
	float min = 0.0;
	float max = 0.0;

	for (int i = 0; i < vertices.size(); i++)
	{
		if (glm::distance(bonePos[1], vertices[i].pos) < 0.2)
		{
			if (min > vertices[i].pos.x) min = vertices[i].pos.x;
			if (max < vertices[i].pos.x) max = vertices[i].pos.x;
		}
	}
	bonePos[1].x = min + (glm::distance(min, max) / 2.0);
	qDebug() << "CAnthropometry: Neck bone pos:" << bonePos[1].x * 100.0 << bonePos[1].y * 100.0 << bonePos[1].z * 100.0;
}

void CAnthropometry::genCollarPoint()
{
	float height = minZ + z[shoulder_H] / 100.0;
	bonePos[2] = glm::vec3(0, (minY + maxY) / 2.0, height);
	float min = 0.0;
	float max = 0.0;

	for (int i = 0; i < vertices.size(); i++)
	{
		if (glm::distance(bonePos[2], vertices[i].pos) < 0.3)
		{
			if (min > vertices[i].pos.x) min = vertices[i].pos.x;
			if (max < vertices[i].pos.x) max = vertices[i].pos.x;
		}
	}
	bonePos[2].x = min + (glm::distance(min, max) / 2.0);
	qDebug() << "CAnthropometry: Collar bone pos:" << bonePos[2].x * 100.0 << bonePos[2].y * 100.0 << bonePos[2].z * 100.0;
}

void CAnthropometry::genMSpinePoint()
{
	float height = minZ + (10.5 / 15.0) * 2.0;
	bonePos[3] = glm::vec3(0, (minY + maxY) / 2.0, height);
	float min = 0.0;
	float max = 0.0;

	for (int i = 0; i < vertices.size(); i++)
	{
		if (glm::distance(bonePos[3], vertices[i].pos) < 0.4)
		{
			if (min > vertices[i].pos.x) min = vertices[i].pos.x;
			if (max < vertices[i].pos.x) max = vertices[i].pos.x;
		}
	}
	bonePos[3].x = min + 0.6 * glm::distance(min, max);
	qDebug() << min << max << glm::distance(min, max) << 0.6 * glm::distance(min, max);
	qDebug() << "CAnthropometry: MSpine bone pos:" << bonePos[3].x * 100.0 << bonePos[3].y * 100.0 << bonePos[3].z * 100.0;
}

void CAnthropometry::genLSpinePoint()
{
	float height = minZ + (8.5 / 15.0) * 2.0;
	bonePos[4] = glm::vec3(0, (minY + maxY) / 2, height);
	float min = 0.0;
	float max = 0.0;

	for (int i = 0; i < vertices.size(); i++)
	{
		if (glm::distance(bonePos[4], vertices[i].pos) < 0.4)
		{
			if (min > vertices[i].pos.x) min = vertices[i].pos.x;
			if (max < vertices[i].pos.x) max = vertices[i].pos.x;
		}
	}
	bonePos[4].x = min + 0.6 * glm::distance(min, max);
	qDebug() << "CAnthropometry: LSpine bone pos:" << bonePos[4].x * 100.0 << bonePos[4].y * 100.0 << bonePos[4].z * 100.0;
}

void CAnthropometry::genHipPoint()
{
	float height = minZ + (7.5 / 15.0) * 2.0;
	bonePos[5] = glm::vec3(0, (minY + maxY) / 2, height);
	float min = 0.0;
	float max = 0.0;

	for (int i = 0; i < vertices.size(); i++)
	{
		if (glm::distance(bonePos[5], vertices[i].pos) < 0.4)
		{
			if (min > vertices[i].pos.x) min = vertices[i].pos.x;
			if (max < vertices[i].pos.x) max = vertices[i].pos.x;
		}
	}
	bonePos[5].x = min + 0.5 * glm::distance(min, max);
	qDebug() << "CAnthropometry: Hip bone pos:" << bonePos[5].x * 100 << bonePos[5].y * 100 << bonePos[5].z * 100;
}

void CAnthropometry::genThighPoints()
{
	float height = minZ + (7.0 / 15.0) * 2.0;
	bonePos[6] = glm::vec3(0, -max_torso_width / 4, height);
	bonePos[7] = glm::vec3(0, max_torso_width / 4, height);
	float rightX = 0.0;
	int rightXnum = 0;
	float leftX = 0.0;
	int leftXnum = 0;

	for (int i = 0; i < vertices.size(); i++)
	{
		if (glm::distance(bonePos[6], vertices[i].pos) < 0.2)
		{
			leftX = leftX + vertices[i].pos.x;
			leftXnum++;
		}
		if (glm::distance(bonePos[7], vertices[i].pos) < 0.2)
		{
			rightX = rightX + vertices[i].pos.x;
			rightXnum++;
		}
	}
	bonePos[6].x = leftX / leftXnum;
	bonePos[7].x = rightX / rightXnum;

	qDebug() << "CAnthropometry: left thigh bone pos:" << bonePos[6].x * 100 << bonePos[6].y * 100 << bonePos[6].z * 100;
	qDebug() << "CAnthropometry: right thigh bone pos:" << bonePos[7].x * 100 << bonePos[7].y * 100 << bonePos[7].z * 100;
}

void CAnthropometry::genKneePoints()
{
	float height = minZ + (4.0 / 15.0) * 2.0;
	bonePos[8] = glm::vec3(0, 0, height);
	bonePos[9] = glm::vec3(0, 0, height);

	float lMinX = 99.0;
	float lMinY = 99.0;
	float lMaxX = -99.0;
	float lMaxY = -99.0;

	float rMinX = 99.0;
	float rMinY = 99.0;
	float rMaxX = -99.0;
	float rMaxY = -99.0;

	for (int i = 0; i < vertices.size(); i++)
	{
		if (vertices[i].pos.z > (height - 0.05) &&
			vertices[i].pos.z < (height + 0.05))
		{
			if (vertices[i].pos.y < 0 )
			{
				if (lMinX > vertices[i].pos.x) lMinX = vertices[i].pos.x;
				if (lMinY > vertices[i].pos.y) lMinY = vertices[i].pos.y;
				if (lMaxX < vertices[i].pos.x) lMaxX = vertices[i].pos.x;
				if (lMaxY < vertices[i].pos.y) lMaxY = vertices[i].pos.y;
			}
			else
			{
				if (rMinX > vertices[i].pos.x) rMinX = vertices[i].pos.x;
				if (rMinY > vertices[i].pos.y) rMinY = vertices[i].pos.y;
				if (rMaxX < vertices[i].pos.x) rMaxX = vertices[i].pos.x;
				if (rMaxY < vertices[i].pos.y) rMaxY = vertices[i].pos.y;
			}
		}
	}

	bonePos[8].x = (lMinX + lMaxX) / 2;
	bonePos[8].y = (lMinY + lMaxY) / 2;
	bonePos[9].x = (rMinX + rMaxX) / 2;
	bonePos[9].y = (rMinY + rMaxY) / 2;
	qDebug() << "CAnthropometry: left knee bone pos:" << bonePos[8].x * 100 << bonePos[8].y * 100 << bonePos[8].z * 100;
	qDebug() << "CAnthropometry: right knee bone pos:" << bonePos[9].x * 100 << bonePos[9].y * 100 << bonePos[9].z * 100;
}

void CAnthropometry::genAnklePoints()
{
	float height = minZ + 0.14;
	bonePos[10] = glm::vec3(0, 0, height);
	bonePos[11] = glm::vec3(0, 0, height);

	float lMinX = 0.0;
	float lMinY = 0.0;
	float lMaxX = 0.0;
	float lMaxY = -99.0;
	float rMinX = 0.0;
	float rMinY = 99.0;
	float rMaxX = 0.0;
	float rMaxY = 0.0;

	for (int i = 0; i < vertices.size(); i++)
	{
		if (vertices[i].pos.z < 0.2)
		{
			if (vertices[i].pos.y < 0)
			{
				if (lMinX > vertices[i].pos.x) lMinX = vertices[i].pos.x;
				if (lMinY > vertices[i].pos.y) lMinY = vertices[i].pos.y;
				if (lMaxX < vertices[i].pos.x) lMaxX = vertices[i].pos.x;
				if (lMaxY < vertices[i].pos.y) lMaxY = vertices[i].pos.y;
			}
			else
			{
				if (rMinX > vertices[i].pos.x) rMinX = vertices[i].pos.x;
				if (rMinY > vertices[i].pos.y) rMinY = vertices[i].pos.y;
				if (rMaxX < vertices[i].pos.x) rMaxX = vertices[i].pos.x;
				if (rMaxY < vertices[i].pos.y) rMaxY = vertices[i].pos.y;
			}
		}
	}

	bonePos[10].x = lMinX + 0.7 * glm::distance(lMinX, lMaxX);
	bonePos[10].y = (lMinY + lMaxY) / 2;
	bonePos[11].x = rMinX + 0.7 * glm::distance(rMinX, rMaxX);
	bonePos[11].y = (rMinY + rMaxY) / 2;

	qDebug() << "CAnthropometry: left Ankle bone pos:" << bonePos[10].x * 100 << bonePos[10].y * 100 << bonePos[10].z * 100;
	qDebug() << "CAnthropometry: right Ankle bone pos:" << bonePos[11].x * 100 << bonePos[11].y * 100 << bonePos[11].z * 100;
}

void CAnthropometry::genFeetPoints()
{
	bonePos[12] = glm::vec3(x[0].x, 0.85 * y[0].y, minZ);
	bonePos[13] = glm::vec3(x[0].x, 0.85 * y[0].x, minZ);

	qDebug() << "CAnthropometry: left foot bone pos:" << bonePos[12].x * 100 << bonePos[12].y * 100 << bonePos[12].z * 100;
	qDebug() << "CAnthropometry: right foot bone pos:" << bonePos[13].x * 100 << bonePos[13].y * 100 << bonePos[13].z * 100;
}

void CAnthropometry::genShoulderPoints()
{
	float height = minZ + (z[shoulder_H] - 5) / 100;
	bonePos[14] = glm::vec3(0, 0, height);
	bonePos[15] = glm::vec3(0, 0, height);
	float tminY = 100.0;
	float tmaxY = -100.0;
	float tminX = 100.0;
	float tmaxX = -100.0;

	for (int i = 0; i < vertices.size(); i++)
	{
		if (vertices[i].pos.z < height * 1.1 &&
			vertices[i].pos.z > height * 0.9)
		{
			if (tminX > vertices[i].pos.x) tminX = vertices[i].pos.x;
			if (tmaxX < vertices[i].pos.x) tmaxX = vertices[i].pos.x;
			if (tminY > vertices[i].pos.y) tminY = vertices[i].pos.y;
			if (tmaxY < vertices[i].pos.y) tmaxY = vertices[i].pos.y;
		}
	}
	bonePos[14].x = tminX + 0.6 * glm::distance(tminX, tmaxX);
	bonePos[14].y = tminY + 0.22 * glm::distance(tminY, tmaxY);

	bonePos[15].x = tminX + 0.6 * glm::distance(tminX, tmaxX);
	bonePos[15].y = tminY + 0.78 * glm::distance(tminY, tmaxY);

	qDebug() << "CAnthropometry: left shoulder bone pos:" << bonePos[14].x * 100 << bonePos[14].y * 100 << bonePos[14].z * 100;
	qDebug() << "CAnthropometry: right shoulder bone pos:" << bonePos[15].x * 100 << bonePos[15].y * 100 << bonePos[15].z * 100;
}

void CAnthropometry::genArmPoints()
{
	bonePos[16] = (bonePos[20] + bonePos[14]) * glm::vec3(0.51);
	bonePos[17] = (bonePos[21] + bonePos[15]) * glm::vec3(0.51);

	int posSum = 0;
	int negSum = 0;
	glm::vec3 vPosSum = glm::vec3(0);
	glm::vec3 vNegSum = glm::vec3(0);

	for (int i = 0; i < vertices.size(); i++)
	{
		if (vertices[i].pos.y < -max_torso_width / 2)
		{
			if (glm::distance(vertices[i].pos, bonePos[16]) < 0.16)
			{
				vNegSum += vertices[i].pos;
				negSum++;
			}
		}
		else if (vertices[i].pos.y > max_torso_width / 2)
		{
			if (glm::distance(vertices[i].pos, bonePos[17]) < 0.16)
			{
				vPosSum += vertices[i].pos;;
				posSum++;
			}
		}
	}

	bonePos[16] = vNegSum / glm::vec3(negSum);
	bonePos[17] = vPosSum / glm::vec3(posSum);

	qDebug() << "CAnthropometry: left arm bone pos:" << bonePos[16].x * 100 << bonePos[16].y * 100 << bonePos[16].z * 100;
	qDebug() << "CAnthropometry: right arm bone pos:" << bonePos[17].x * 100 << bonePos[17].y * 100 << bonePos[17].z * 100;
}

void CAnthropometry::genWristPoints()
{
	bonePos[18] = (bonePos[20] + bonePos[16]) * glm::vec3(0.465);
	bonePos[19] = (bonePos[21] + bonePos[17]) * glm::vec3(0.465);

	int posSum = 0;
	int negSum = 0;
	glm::vec3 vPosSum = glm::vec3(0);
	glm::vec3 vNegSum = glm::vec3(0);

	for (int i = 0; i < vertices.size(); i++)
	{
		if (vertices[i].pos.y < -max_torso_width / 2)
		{
			if (glm::distance(vertices[i].pos, bonePos[18]) < 0.11)
			{
				vNegSum += vertices[i].pos;
				negSum++;
			}
		}
		else if (vertices[i].pos.y > max_torso_width / 2)
		{
			if (glm::distance(vertices[i].pos, bonePos[19]) < 0.11)
			{
				vPosSum += vertices[i].pos;;
				posSum++;
			}
		}
	}
	bonePos[18] = vNegSum / glm::vec3(negSum);
	bonePos[19] = vPosSum / glm::vec3(posSum);

	qDebug() << "CAnthropometry: left wrist bone pos:" << bonePos[18].x * 100 << bonePos[18].y * 100 << bonePos[18].z * 100;
	qDebug() << "CAnthropometry: right wrist bone pos:" << bonePos[19].x * 100 << bonePos[19].y * 100 << bonePos[19].z * 100;
}

void CAnthropometry::genHandPoints()
{
	int posSum = 0;
	int negSum = 0;
	glm::vec3 vPosSum = glm::vec3(0);
	glm::vec3 vNegSum = glm::vec3(0);

	for (int i = 0; i < vertices.size(); i++)
	{
		if (vertices[i].pos.z > (z[hand_H] - 5) / 100 && vertices[i].pos.z < (z[hand_H] + 5) / 100)
		{
			if (vertices[i].pos.y < -max_torso_width / 2)
			{
				vNegSum += vertices[i].pos;
				negSum++;
			}
			else if (vertices[i].pos.y > max_torso_width / 2)
			{
				vPosSum += vertices[i].pos;;
				posSum++;
			}
		}
	}
	bonePos[20] = vNegSum / glm::vec3(negSum);
	bonePos[21] = vPosSum / glm::vec3(posSum);

	qDebug() << "CAnthropometry: left hand bone pos:" << bonePos[20].x * 100 << bonePos[20].y * 100 << bonePos[20].z * 100;
	qDebug() << "CAnthropometry: right hand bone pos:" << bonePos[21].x * 100 << bonePos[21].y * 100 << bonePos[21].z * 100;
}

std::vector<glm::vec3> CAnthropometry::getBonePositions()
{
	return bonePos;
}