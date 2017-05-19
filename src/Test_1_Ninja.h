// lataa skenen, asettaa randomilla ruohoa siihen, liittää ruohot 1 objektiin.
// ninjaa voi liikuttaa näppikseltä.
/*
1-14	Walk (normal)
15-30	Stealth Walk
32-44	Punch and swipe sword
45-59	Swipe and spin sword
60-68	Overhead twohanded downswipe
69-72	Up to block position (play backwards to lower sword if you want)
73-83	Forward kick
84-93	Pick up from floor (or down to crouch at frame 87)
94-102	Jump
103-111	Jump without height (for programmer controlled jumps)
112-125	High jump to Sword Kill (Finish em off move??)
126-133	Side Kick
134-145	Spinning Sword attack (might wanna speed this up in game)
146-158	Backflip
159-165	Climb wall
166-173	Death 1 - Fall back onto ground
174-182	Death 2 - Fall forward onto ground
184-205	Idle 1 - Breathe heavily
206-250	Idle 2
251-300	Idle 3
*/

#pragma once
#include "Program.h"

class Test_1 : public Program
{
protected:	
	Model ninja;
public:

	void createScene()
	{
		Model map;
		map.load("test1/map1.irr");

		IMetaTriangleSelector *meta = createTriangleSelectors();

		ninja.load("test1/ninja.b3d");
		ninja.getAnimatedNode()->setMaterialFlag(video::EMF_LIGHTING, false);
		ninja.getAnimatedNode()->setFrameLoop(206, 250); // idle
		ninja.getAnimatedNode()->setAnimationSpeed(15);
		ninja.getAnimatedNode()->setTransitionTime(0.2f);
		ninja.getAnimatedNode()->setScale(vector3df(0.05f, 0.05f, 0.05f));
		ninja.getAnimatedNode()->setPosition(vector3df(0,2,0));
		ninja.createCollisionResponseAnimator(meta);
		meta->drop();


		// laita ruohot maahan (randomilla x ja z,  y etitään ray-collision testillä)
		ISceneCollisionManager *collMan = sceneManager->getSceneCollisionManager();
		driver->setMinHardwareBufferVertexCount(0);

		IAnimatedMesh *grassMesh=sceneManager->getMesh(DATA_DIR + "test1/grass.irrmesh");
		grassMesh->setHardwareMappingHint(EHM_STATIC);

		array<IMeshSceneNode*> nodes;
		line3df ray;
		vector3df intersection;
		triangle3df hitTriangle;
		for(int q=0;q<10000;q++)
		{
			const float S=50;
			float x = ((float)rand()/(float)RAND_MAX) * S - (S/2.f);
			float z = ((float)rand()/(float)RAND_MAX) * S - (S/2.f);

			ray.start = vector3df(x,100,z);
			ray.end = vector3df(x,-100,z);
			ISceneNode *selectedSceneNode = collMan->getSceneNodeAndCollisionPointFromRay(ray, intersection, hitTriangle, 0, map.getNode());

			if(selectedSceneNode!=NULL) 
			{
				IMeshSceneNode *node = sceneManager->addMeshSceneNode(grassMesh, 0, 0);
				node->setPosition(intersection);
				node->setScale(vector3df(0.1f, 0.1f, 0.1f));

				nodes.push_back(node);
			}
		}

		/*
		CMeshCombiner* combiner = new CMeshCombiner(0.8f, ETPT_TILE);
		IMesh* combinedMesh = combiner->combineMeshes(sceneManager, nodes, DATA_DIR + "test1/grass.irrmesh");
		IMeshSceneNode* k = sceneManager->addMeshSceneNode(combinedMesh);
		k->setMaterialFlag(EMF_LIGHTING,false);
		*/

		setFPSCamera();
		//sceneManager->addCameraSceneNode(0, vector3df(0,30,-40), vector3df(0,5,0));
	}

	void clearScene()
	{
		sceneManager->clear();
		guienv->clear();
	}

	int mode, activeMode;
	void render(f32 frameDeltaTime)
	{
		bool pressed=false;
		float SPD=1;

		if(input.IsKeyDown(irr::KEY_ESCAPE))
			running=false; //device->closeDevice();

		if(input.IsKeyDown(irr::KEY_LEFT))
		{
			ninja.rotate(0, -frameDeltaTime*200, 0);
			pressed=true;
			mode=1;
			ninja.getAnimatedNode()->setAnimationSpeed(15);
		}

		if(input.IsKeyDown(irr::KEY_RIGHT))
		{
			ninja.rotate(0, frameDeltaTime*200, 0);
			pressed=true;
			mode=1;
			ninja.getAnimatedNode()->setAnimationSpeed(-15);
		}

		if(input.IsKeyDown(irr::KEY_UP))
		{
			ninja.move(frameDeltaTime*SPD);
			pressed=true;
			mode=2;
			ninja.getAnimatedNode()->setAnimationSpeed(15);
		}
		if(input.IsKeyDown(irr::KEY_DOWN))
		{
			ninja.move(-frameDeltaTime*SPD);
			pressed=true;
			mode=2;
			ninja.getAnimatedNode()->setAnimationSpeed(-15);
		}

		if(mode!=activeMode)
		{
			ninja.getAnimatedNode()->setCurrentFrame(0);
			if(mode==1)
				ninja.getAnimatedNode()->setFrameLoop(14, 29); // "turn"

			if(mode==2)
				ninja.getAnimatedNode()->setFrameLoop(0, 13);  // walk

			activeMode=mode;
		}

		if(!pressed)
		{
			if(mode!=0)
			{
				ninja.getAnimatedNode()->setFrameLoop(206, 250); // idle
				mode=0;
			}
		}

		ninja.getAnimatedNode()->animateJoints();

		sceneManager->drawAll();
		guienv->drawAll();

	}

};
