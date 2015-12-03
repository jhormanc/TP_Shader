#pragma once
#include "Shapes.h"
#include "Triangle.h"
#include <vector>
#include <string>
#include <fstream>
class Mesh : public Shapes
{
public:
	std::vector <Triangle> faces;
	Mesh();
	bool intersect(const Ray &ray, float * tHit) const;
	BBox getBound() const;
	Normals getNormal(Point p) const;
	static Mesh readFromObj(std::string filename, Vector pos)
	{
		std::ifstream file(filename);
		std::vector<Point> points;
		std::vector<Triangle> faces;
		int currentPoint = 0;
		Mesh res;
		char current;

		while (file >> current && current != 'v');
		while (current == 'v')
		{
			Point tmp;
			file >> tmp.x;
			file >> tmp.y;
			file >> tmp.z;
			points.resize(currentPoint + 1);
			points[currentPoint] = tmp + pos;
			++currentPoint;
			file >> current;
		}
		while (current == 'f')
		{
			int p1, p2, p3;
			int skip;
			char skipc;
			file >> p1;
			//file >> skipc;
			//file >> skip;
			file >> p2;
		//	file >> skipc;
			//file >> skip;
			file >> p3;
		//	file >> skipc;
		//	file >> skip;
			faces.push_back(Triangle(points[p1 - 1], points[p2 - 1], points[p3 - 1]));
			if (!(file >> current)) break;
			
		}
		res.faces = faces;
		file.close();
		return res;
	}
	~Mesh();
};

