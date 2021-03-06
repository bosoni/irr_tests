﻿#pragma once
#include "Program.h"

extern E_DRIVER_TYPE driverType;
extern IrrlichtDevice *device;
extern IVideoDriver *driver;
extern ISceneManager *sceneManager;
extern IFileSystem *fileSystem;
extern const stringc DATA_DIR;

class Model
{
private:
	IAnimatedMesh *mesh;
	ISceneNode *node;
	bool scene; // jos true, ollaan ladattu .irr filu. 

public:
	Model() : mesh(0), node(0), scene(false) {}

	void clear()
	{
		if(node!=0) 
		{
			node->removeAll();	// remove children
			node->remove();		// remove this
		}
		node=0;
		mesh=0;
	}

	void createEmpty()
	{
		node = sceneManager->addEmptySceneNode();
	}

	bool load(stringc fileName)
	{
		fileName = DATA_DIR + fileName;

		// tsekkaa onko nimessä .irr
		if(fileName.find(".irr")!=-1) // löyty
		{
			scene=true;
			node = sceneManager->addEmptySceneNode();
			bool b=sceneManager->loadScene(fileName.c_str(), 0, node);
			if(b==false) 
				throw "Load failed!";
		}
		else
		{
			mesh=sceneManager->getMesh(fileName.c_str());
			if(mesh==0) 
				throw "Load failed!";
			else
				node = sceneManager->addAnimatedMeshSceneNode(mesh);
		}

		return true;
	}

	void move(float speed, float Y_FIX=-90)
	{
		vector3df v = node->getPosition();
		vector3df r = node->getRotation();
		r.Y += Y_FIX; // tässä korjataan noden asento että obu liikkuu oikeaan suuntaan
		v.X = v.X + (float)((cos(r.Y * PI / 180) * speed));
		v.Z = v.Z - (float)(sin(r.Y * PI / 180) * speed);
		node->setPosition(v); 
	}

	void rotate(float x, float y, float z)
	{
		vector3df r=node->getRotation();
		r += vector3df(x,y,z);
		node->setRotation(r);
	}

	ISceneNode *getNode()
	{
		return node;
	}

	IAnimatedMeshSceneNode *getAnimatedNode()
	{
		return (IAnimatedMeshSceneNode*)node;
	}

	void createCollisionResponseAnimator(IMetaTriangleSelector *meta)
	{
		vector3df s = node->getBoundingBox().getExtent() * node->getScale();

		ISceneNodeAnimator* anim = sceneManager->createCollisionResponseAnimator(meta, node, 
			vector3df(s.X/3.f, s.Y/2.f, s.Z/3.f), vector3df(0,-0.01f,0), vector3df(0,-s.Y/2.f, 0), 0.0007f);

		node->addAnimator(anim);
		anim->drop();
	}

	s32 loadShader(stringc name, IShaderConstantSetCallBack *shaderCallback = 0, s32 userdata = 0)
	{
		IGPUProgrammingServices* gpu = driver->getGPUProgrammingServices();
		if(!gpu) return -1;

		stringc file = DATA_DIR + "Shaders/" + name;

		if(driverType == EDT_OPENGL)
			return gpu->addHighLevelShaderMaterialFromFiles(
				(file + ".vert").c_str(), "main_v", EVST_VS_2_0,
				(file + ".frag").c_str(), "main_f", EPST_PS_2_0,
				shaderCallback, video::EMT_SOLID, userdata, EGSL_DEFAULT);
		else
			return gpu->addHighLevelShaderMaterialFromFiles(
				(file + ".hlsl").c_str(), "main_v", EVST_VS_2_0,
				(file + ".hlsl").c_str(), "main_f", EPST_PS_2_0,
				shaderCallback, video::EMT_SOLID, userdata, EGSL_DEFAULT);
	}

	void useShader(s32 shader)
	{
		if(scene)
		{
			core::array<scene::ISceneNode *> nodes;
			sceneManager->getSceneNodesFromType(ESNT_MESH, nodes); 
			for (u32 i=0; i < nodes.size(); ++i)
				nodes[i]->setMaterialType((E_MATERIAL_TYPE)shader);
		}
		else
			node->setMaterialType((E_MATERIAL_TYPE)shader);
	}

	void setMaterialFlag(E_MATERIAL_FLAG flag, bool newvalue)
	{
		if(scene)
		{
			core::array<scene::ISceneNode *> nodes;
			sceneManager->getSceneNodesFromType(ESNT_MESH, nodes); 
			for (u32 i=0; i < nodes.size(); ++i)
				nodes[i]->setMaterialFlag(flag, newvalue);

		}
		else
			node->setMaterialFlag(flag, newvalue);
	}



};
