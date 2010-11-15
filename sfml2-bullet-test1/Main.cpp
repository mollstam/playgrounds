#include "btBulletDynamicsCommon.h"
#include "DebugDraw.hpp"
#include "Definitions.hpp"
#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <boost/shared_ptr.hpp>

int main(int argc, char** argv) {
        sf::RenderWindow* RenderWin = new sf::RenderWindow(sf::VideoMode(WIDTH, HEIGHT, 32), "lol test");
        RenderWin->UseVerticalSync(true);

        // Collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
		boost::shared_ptr<btDefaultCollisionConfiguration> collisionConfiguration(new btDefaultCollisionConfiguration());

        // Use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded).
		boost::shared_ptr<btCollisionDispatcher> dispatcher(new btCollisionDispatcher(collisionConfiguration.get()));

        // btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
		boost::shared_ptr<btBroadphaseInterface> overlappingPairCache(new btDbvtBroadphase());

        // The default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded).
		boost::shared_ptr<btSequentialImpulseConstraintSolver> solver(new btSequentialImpulseConstraintSolver());

		boost::shared_ptr<btDiscreteDynamicsWorld> dynamicsWorld(new btDiscreteDynamicsWorld(dispatcher.get(), overlappingPairCache.get(), solver.get(), collisionConfiguration.get()));

        // Set gravity to 9.8m/s² along y-axis.
        dynamicsWorld->setGravity(btVector3(0, 1, 0));

        // Get us some debug output. Without this, we'd see nothing at all.
		boost::shared_ptr<DebugDraw> debugDraw(new DebugDraw(RenderWin));
        debugDraw->setDebugMode(btIDebugDraw::DBG_DrawWireframe);

        dynamicsWorld->setDebugDrawer(debugDraw.get());

        // Keep track of the shapes, we release memory at exit.
        // Make sure to re-use collision shapes among rigid bodies whenever possible!
        btAlignedObjectArray<btCollisionShape*> collisionShapes;

        // Create a ground body.
        btScalar thickness(0.2);
		boost::shared_ptr<btCollisionShape> groundShape(new btBoxShape(btVector3(btScalar(WIDTH / 2 * METERS_PER_PIXEL), thickness, btScalar(0))));
        collisionShapes.push_back(groundShape.get());
        btTransform groundTransform(btQuaternion(0, 0, 0, 1), btVector3(WIDTH / 2 * METERS_PER_PIXEL, HEIGHT * METERS_PER_PIXEL, 0));
        // Using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects.
		boost::shared_ptr<btDefaultMotionState> groundMotionState(new btDefaultMotionState(groundTransform));
        btRigidBody::btRigidBodyConstructionInfo ground_rbInfo(0, groundMotionState.get(), groundShape.get(), btVector3(0, 0, 0));
		boost::shared_ptr<btRigidBody> ground_body(new btRigidBody(ground_rbInfo));
        // Add the body to the dynamics world.
        dynamicsWorld->addRigidBody(ground_body.get());

        // Create left wall.
        btTransform leftWallTransform(btQuaternion(0, 0, 1, 1), btVector3(0, HEIGHT / 2 * METERS_PER_PIXEL, 0));
		boost::shared_ptr<btDefaultMotionState> leftWallMotionState(new btDefaultMotionState(leftWallTransform));
        btRigidBody::btRigidBodyConstructionInfo leftWall_rbInfo(0, leftWallMotionState.get(), groundShape.get(), btVector3(0, 0, 0));
		boost::shared_ptr<btRigidBody> leftwall_body(new btRigidBody(leftWall_rbInfo));
        // Add the body to the dynamics world.
        dynamicsWorld->addRigidBody(leftwall_body.get());

        // Create right wall.
        btTransform rightWallTransform(btQuaternion(0, 0, 1, 1), btVector3(WIDTH * METERS_PER_PIXEL, HEIGHT / 2 * METERS_PER_PIXEL, 0));
		boost::shared_ptr<btDefaultMotionState> rightWallMotionState(new btDefaultMotionState(rightWallTransform));
        btRigidBody::btRigidBodyConstructionInfo rightWall_rbInfo(0, rightWallMotionState.get(), groundShape.get(), btVector3(0, 0, 0));
		boost::shared_ptr<btRigidBody> rightwall_body(new btRigidBody(rightWall_rbInfo));
        // Add the body to the dynamics world.
        dynamicsWorld->addRigidBody(rightwall_body.get());

        // Create ceiling
        btTransform topWallTransform(btQuaternion(0, 0, 0, 1), btVector3(WIDTH / 2 * METERS_PER_PIXEL, 0, 0));
		boost::shared_ptr<btDefaultMotionState> topWallMotionState(new btDefaultMotionState(topWallTransform));
        btRigidBody::btRigidBodyConstructionInfo topWall_rbInfo(0, topWallMotionState.get(), groundShape.get(), btVector3(0, 0, 0));
		boost::shared_ptr<btRigidBody> topwall_body(new btRigidBody(topWall_rbInfo));
        // Add the body to the dynamics world.
        dynamicsWorld->addRigidBody(topwall_body.get());


        // Create dynamic rigid body.

        //btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
		boost::shared_ptr<btCollisionShape> colShape(new btSphereShape(btScalar(1.)));
        collisionShapes.push_back(colShape.get());

        /// Create Dynamic Objects
        btTransform startTransform;
        startTransform.setIdentity();

        btScalar mass(1.f);

        //rigidbody is dynamic if and only if mass is non zero, otherwise static
        bool isDynamic = (mass != 0.f);

        btVector3 localInertia(0, 0, 0);
        if (isDynamic)
                colShape->calculateLocalInertia(mass,localInertia);

        startTransform.setOrigin(btVector3(5, 5, 0));

        //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		boost::shared_ptr<btDefaultMotionState> myMotionState(new btDefaultMotionState(startTransform));
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState.get(),colShape.get(),localInertia);
		boost::shared_ptr<btRigidBody> body(new btRigidBody(rbInfo));

        dynamicsWorld->addRigidBody(body.get());

/// Do some simulation
		sf::Event sfmlEvent;

        while(RenderWin->IsOpened()) {
		while(RenderWin->GetEvent(sfmlEvent)) {
			if(sfmlEvent.Type == sf::Event::Closed) 
				RenderWin->Close();
			if(sfmlEvent.Type == sf::Event::KeyPressed) {
				if(sfmlEvent.Key.Code == sf::Key::Escape) 
					RenderWin->Close();
			}
		}
			// SFML access class for real-time input
			const sf::Input& input = RenderWin->GetInput();
			// get the current "frame time" (seconds elapsed since the previous frame, hopefully close to 1/60 since vsync is enabled)
			float frameTime = RenderWin->GetFrameTime();

			//step the Box2D physics world, with a constant time step
			//note that this is kind of a bad way to do time steps, as it can get out of sync if the framerate drops (see http://gafferongames.wordpress.com/game-physics/fix-your-timestep/ for more information)
			dynamicsWorld->stepSimulation(1/60.f, 10);
			dynamicsWorld->clearForces();

			//check for user keyboard input to control Bullet forces/torques/etc
			float mag = 500.0f;
			if(input.IsKeyDown(sf::Key::Left))
				body->applyForce(btVector3(-mag*frameTime, 0.0f, 0), body->getCenterOfMassPosition());
			if(input.IsKeyDown(sf::Key::Right)) 
				body->applyForce(btVector3(mag*frameTime, 0.0f, 0), body->getCenterOfMassPosition());
			if(input.IsKeyDown(sf::Key::Up)) 
				body->applyForce(btVector3(0.0f, -mag*frameTime, 0), body->getCenterOfMassPosition());
			if(input.IsKeyDown(sf::Key::Down)) 
				body->applyForce(btVector3(0.0f, mag*frameTime, 0), body->getCenterOfMassPosition());
			if(input.IsKeyDown(sf::Key::Q)) 
				body->applyTorque(btVector3(mag/2*frameTime, 0, 0));
			if(input.IsKeyDown(sf::Key::E)) 
				body->applyTorque(btVector3(-mag/2*frameTime, 0, 0));

			/*// apply random forces to all the small bodies created earlier
			if(input.IsKeyDown(sf::Key::Space)) {
				for (sf::Uint32 i=0; i<smallBodyList.size(); i++) {
					smallBodyList[i]->applyForce(btVector3(mag/100*(10-rand()%20), mag/100*(10-rand()%20), 0), smallBodyList[i]->getCenterOfMassPosition());
				}
			}*/

			if(input.IsKeyDown(sf::Key::W)) {
				// make a new gravity vector
				btVector3 gravity(0.0f, -2.0f, 0);
				// pass it to Box2D
				dynamicsWorld->setGravity(gravity);
				// wake up all sleeping bodies so they will be affected by the gravity change
				//BOOST_FOREACH(btRigidBody* body, smallBodyList)
				//	body->SetAwake(true);
			}
			if(input.IsKeyDown(sf::Key::S)) {
				btVector3 gravity(0.0f, 2.0f, 0);
				dynamicsWorld->setGravity(gravity);
				//BOOST_FOREACH(btRigidBody* body, smallBodyList)
				//	body->SetAwake(true);
			}
			if(input.IsKeyDown(sf::Key::A)) {
				btVector3 gravity(-2.0f, 0.0f, 0);
				dynamicsWorld->setGravity(gravity);
				//BOOST_FOREACH(btRigidBody* body, smallBodyList)
				//	body->SetAwake(true);
			}
			if(input.IsKeyDown(sf::Key::D)) {
				btVector3 gravity(2.0f, 0.0f, 0);
				dynamicsWorld->setGravity(gravity);
				//BOOST_FOREACH(btRigidBody* body, smallBodyList)
				//	body->SetAwake(true);
			}
                RenderWin->Clear(sf::Color(50,50,50));


                //print positions of all objects
                for(int j=dynamicsWorld->getNumCollisionObjects()-1; j>=0 ;j--) {
                        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[j];
                        btRigidBody* body = btRigidBody::upcast(obj);
                        if (body && body->getMotionState()) {
                                btTransform trans;
                                body->getMotionState()->getWorldTransform(trans);
                                //printf("world pos = %f,%f,%f\n", float(trans.getOrigin().getX()),float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
                        }
                }

                //have Bullet use our debug drawing class to render the world
                dynamicsWorld->debugDrawWorld();

                RenderWin->Display();
        }

        return 0;
}
