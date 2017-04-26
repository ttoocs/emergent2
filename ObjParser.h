#include <iostream>
#include <fstream>
#include <cstdlib>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>
#include <unistd.h>
#include <sstream>

#ifdef found_this_in_duplicate_of_this_file
  #include "CustomOperators.h"   //Seems to make it not compile. /shrug.
#endif

using namespace std;

void loadObjFile(	string fileName, vector<vec3> &vertices,
									vector<vec3> &normals, vector<vec2> &uv,
									vector<uint> &indices)
{
	vertices.clear();
	normals.clear();
	uv.clear();
	indices.clear();
	string input;
	ifstream inFile(fileName, ios::in);
	vector<vector<uint>> vertexNormals;
	while(getline(inFile, input))
	{
		string parsedInfo;
		istringstream iss(input);
		iss >> parsedInfo;
		//cout << parsedInfo << endl;
		if(parsedInfo == "v")
		{
			iss>>parsedInfo;
			float x = stof(parsedInfo);
			iss>>parsedInfo;
			float y= stof(parsedInfo);
			iss>>parsedInfo;
			float z= stof(parsedInfo);
			vertices.push_back(vec3(x,y,z));
			vector<uint> vNormals;
			vertexNormals.push_back(vNormals);
		}

		else if(parsedInfo == "vn")
		{
			iss>>parsedInfo;
			float x = stof(parsedInfo);
			iss>>parsedInfo;
			float y= stof(parsedInfo);
			iss>>parsedInfo;
			float z= stof(parsedInfo);
			normals.push_back(vec3(x,y,z));
		}

		else if(parsedInfo == "f")
		{
			while(iss>>parsedInfo)
			{
				string currentNum;
				uint values[3];
				uint index =0;
				for(char c: parsedInfo)
				{
					if(c=='/')
					{
						if(currentNum=="")
							values[index] = -1;
						else
							values[index] = stoi(currentNum);

						currentNum.clear();
						index++;
					}
					else
					{
						currentNum+=c;
					}
				}
				values[index] = stoi(currentNum);
				vertexNormals[values[0]-1].push_back(values[2]-1);
				indices.push_back(values[0]-1);
				//cout << indices[indices.size()-1] << endl;
			}
		}
	}
	inFile.close();

	vector<vec3> finalNormals;
	for(vector<uint> normalIndexes: vertexNormals)
	{
		vec3 finalNormal=vec3(0);
		for(uint index: normalIndexes)
		{
			finalNormal += normals[index];
			//cout << normals[index] << endl;
		}
		//cout << endl;
		finalNormal = finalNormal/(float)normalIndexes.size();
		finalNormals.push_back(finalNormal);
	}
	//cout << endl;
	/*for(vec3 v: finalNormals)
		//cout << v << endl;*/
	normals = finalNormals;
}
