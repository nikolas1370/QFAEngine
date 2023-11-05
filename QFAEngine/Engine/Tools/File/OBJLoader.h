#pragma once
#include <Object/ActorComponent/SceneComponent/Mesh/StaticMesh.h>
#include <Object/ActorComponent/SceneComponent/Mesh/Mesh.h>
#include "FileLoader.h"
#include "Parser.h"
#include <Math/Math.h>

class OBJLoader
{
	struct materialName
	{
		FVector color = FVector(1);
		float specular = 0;
//		int length = 0;
		std::string name;// startName
	};

	struct rawIndex
	{
		unsigned int vertex;
		unsigned int normal;
		unsigned int material;
	};
	/*
		true frame
		false not frame
	*/
//	static bool SomeFun(const FVector& previousFrameVertex, const FVector& curentFrameVertex, const FVector& nextFrameVertex);


	static MeshFrames* LoadModel(std::string path, unsigned int framePerSecond);
public:
	/* 
		load only triangulate mesh
		model Up vecto		  Y  ( up Y and forwart -Z because blender defauld setings)
		model Forward vector -Z
	*/
	static QStaticMesh* LoadModel(std::string path);
	/*
		load only triangulate mesh
		model Up vector		  Y
		model Forward vector -Z

		path == "nameModel_1.obj" after name always be '_' and start from first(1) frame
			in name only one '_' and '.'

	*/
	static QMesh* LoadModelWithAnimate(std::string path, unsigned int framePerSecond);

	
};
