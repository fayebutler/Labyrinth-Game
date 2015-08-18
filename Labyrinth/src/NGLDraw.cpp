//----------------------------------------------------------------------------------------------------------------------
/// @file NGLDraw.cpp
/// @brief NGLDraw constructors, destructors and methods
//----------------------------------------------------------------------------------------------------------------------

#include <QMouseEvent>
#include <QGuiApplication>
#include "NGLDraw.h"
#include <ngl/ShaderLib.h>
#include <ngl/NGLInit.h>
#include <ngl/Material.h>
#include <ngl/VAOPrimitives.h>
#include "PhysicsWorld.h"
#include "CollisionShape.h"
#include <SDL.h>
#include <sstream>
#include <string>

//----------------------------------------------------------------------------------------------------------------------

const static float INCREMENT=0.01;
const static float ZOOM=1.0;
NGLDraw::NGLDraw()
{
  m_rotate=false;
  m_spinXFace=0;
  m_spinYFace=0;

  glClearColor(0.98f, 0.98f, 0.98f, 1.0f);
  glEnable(GL_DEPTH_TEST);

  ngl::ShaderLib *shader=ngl::ShaderLib::instance();

  shader->createShaderProgram("Phong");
  shader->attachShader("PhongVertex",ngl::VERTEX);
  shader->attachShader("PhongFragment",ngl::FRAGMENT);
  shader->loadShaderSource("PhongVertex","shaders/PhongVertex.glsl");
  shader->loadShaderSource("PhongFragment","shaders/PhongFragment.glsl");
  shader->compileShader("PhongVertex");
  shader->compileShader("PhongFragment");
  shader->attachShaderToProgram("Phong","PhongVertex");
  shader->attachShaderToProgram("Phong","PhongFragment");
  shader->bindAttribute("Phong",0,"inVert");
  shader->bindAttribute("Phong",1,"inUV");
  shader->bindAttribute("Phong",2,"inNormal");
  shader->linkProgramObject("Phong");
  (*shader)["Phong"]->use();

  ngl::Vec3 from(0,100,0);
  ngl::Vec3 to(0,10,0);
  ngl::Vec3 up(0,0,1);
  m_cam= new ngl::Camera(from,to,up);
  m_cam->setShape(45,(float)720.0/576.0,0.05,350);
  shader->setShaderParam3f("viewerPos",m_cam->getEye().m_x,m_cam->getEye().m_y,m_cam->getEye().m_z);

  m_light = new ngl::Light(ngl::Vec3(0,100,0),ngl::Colour(1,1,1,1),ngl::Colour(1,1,1,1),ngl::POINTLIGHT );
  m_light->loadToShader("light");

  ngl::ShaderLib *shader2=ngl::ShaderLib::instance();

  shader2->createShaderProgram("TextureShader");
  shader2->attachShader("TextureVertex",ngl::VERTEX);
  shader2->attachShader("TextureFragment",ngl::FRAGMENT);
  shader2->loadShaderSource("TextureVertex","shaders/TextureVert.glsl");
  shader2->loadShaderSource("TextureFragment","shaders/TextureFrag.glsl");
  shader2->compileShader("TextureVertex");
  shader2->compileShader("TextureFragment");
  shader2->attachShaderToProgram("TextureShader","TextureVertex");
  shader2->attachShaderToProgram("TextureShader","TextureFragment");
  shader2->linkProgramObject("TextureShader");
  shader2->use("TextureShader");
  shader2->registerUniform("TextureShader","MVP");

  ngl::Texture texture("textures/wood.tif");
  m_mazeTexture=texture.setTextureGL();

  m_text = new Text("font/Raleway Thin.ttf",100);
  m_text->setColour(0.0,0.0,0.0);

  m_bodyText = new Text("font/Raleway Thin.ttf",60);
  m_bodyText->setColour(0.0,0.0,0.0);

  m_sphereMesh = new ngl::Obj("obj/sphere.obj");
  m_sphereMesh->createVAO();

  m_mazeMesh = new ngl::Obj("obj/mazev3.obj", "textures/wood.tif");
  m_mazeMesh->createVAO();

  m_cube = new ngl::Obj("obj/cubev2.obj");
  m_cube->createVAO();

  CollisionShape *shapes=CollisionShape::instance();
  shapes->addSphere("ball", "obj/sphere.obj");
  shapes->addMaze("maze", "obj/mazev3.obj");
  shapes->addBox("cube", "obj/cubev2.obj");
}

//----------------------------------------------------------------------------------------------------------------------

NGLDraw::~NGLDraw()
{
  ngl::NGLInit *Init = ngl::NGLInit::instance();
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
  delete m_light;
  delete m_cam;
  delete m_physics;
  delete m_sphereMesh;
  delete m_mazeMesh;
//  glDeleteFramebuffers(1, &m_fboID);
  Init->NGLQuit();
}

//----------------------------------------------------------------------------------------------------------------------

void NGLDraw::setPhysics(int _gravityY, float _friction)
{
  m_gravity = ngl::Vec3(0, _gravityY, 0);
  m_physics = new PhysicsWorld();
  m_physics->setGravity(m_gravity);
  m_physics->addGroundPlane(ngl::Vec3(0,0,0), ngl::Vec3(50,0.01,50));

  m_physics->addSphere("ball",ngl::Vec3(-15,25,-15), _friction);
  m_physics->addMaze("maze", ngl::Vec3(0,20,0), _friction);
  m_physics->addCube("cube",ngl::Vec3(0,17,0));
}

//----------------------------------------------------------------------------------------------------------------------

void NGLDraw::resize(int _w, int _h)
{
  glViewport(0,0,_w,_h);
  m_cam->setShape(45,(float)_w/_h,0.05,350);

  m_text->setScreenSize(_w,_h);
  SDL_Rect s;
  SDL_GetDisplayBounds(0,&s);
  float x,y;
  x=1.0-float(s.w-_w)/s.w;
  y=1.0-float(s.h-_h)/s.h;
  std::cout<<s.w-_w<<" "<<s.h-_h<<"\n";
  std::cout<<x<<" "<<y<<"\n";

  m_text->setTransform(x,y);
  draw();

}

//----------------------------------------------------------------------------------------------------------------------

void NGLDraw::draw()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  (*shader)["Phong"]->use();

  ngl::Transformation trans;
  ngl::Mat4 rotX;
  ngl::Mat4 rotY;

  rotX.rotateX(m_spinXFace);
  rotY.rotateY(m_spinYFace);

  m_mouseGlobalTX=rotY*rotX;

  m_mouseGlobalTX.m_m[3][0] = m_modelPos.m_x;
  m_mouseGlobalTX.m_m[3][1] = m_modelPos.m_y;
  m_mouseGlobalTX.m_m[3][2] = m_modelPos.m_z;
  loadMatricesToPhongShader();

  unsigned int bodies=m_physics->getNumCollisionObjects();
  for(unsigned int i=1; i<bodies; ++i)
  {
    m_bodyTransform=m_physics->getTransformMatrix(i);

    if(m_physics->getBodyNameAtIndex(i)=="ball")
    {
      loadMatricesToPhongShader();
      ngl::Material m(ngl::SILVER);
      m.loadToShader("material");
      m_physics->getCollisionShape(i);
      m_sphereMesh->draw();
    }
    else if(m_physics->getBodyNameAtIndex(i)=="maze")
    {
      loadMatricesToTextureShader();
      glBindTexture(GL_TEXTURE_2D, m_mazeTexture);
      m_physics->getCollisionShape(i);
      m_mazeMesh->draw();
    }
    else if(m_physics->getBodyNameAtIndex(i)=="cube")
    {
      loadMatricesToPhongShader();
      ngl::Material m(ngl::BLACKPLASTIC);
      m.loadToShader("material");
      m_physics->getCollisionShape(i);
      m_cube->draw();
    }
  }

  loadMatricesToPhongShader();

  m_bodyTransform.identity();

  loadMatricesToPhongShader();

  if(getGameState()==0)//main menu
  {
    m_text->renderText(300,200, "Labyrinth");
    m_bodyText->renderText(300, 400, "Get the Ball to the Black Square");
    m_bodyText->renderText(300,500, "Press A To Play");
    m_bodyText->renderText(300,600, "Use Arrow Keys");
    m_bodyText->renderText(300,700, "Esc To Quit");
    m_bodyText->renderText(300,800, "Press B To Add More Balls");
  }
  else if(getGameState()==1)//game
  {
   m_physics->step(1.0f/60.0f, 10);
  }
  else if(getGameState()==2)//lost menu
  {
    m_text->renderText(300,300, "You Lose");
    m_text->renderText(200,500, "Press A To Try Again");
  }
  else if(getGameState()==3)//win menu
  {
    m_text->renderText(300,300, "You Win");
    m_text->renderText(200,500, "Press A To Play Again");
  }

}

//----------------------------------------------------------------------------------------------------------------------

void NGLDraw::loadMatricesToTextureShader()
{
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  (*shader)["TextureShader"]->use();
  ngl::Mat4 MV;
  ngl::Mat4 MVP;
  ngl::Mat3 normalMatrix;
  ngl::Mat4 M;
  M=m_bodyTransform*m_transformStack.getCurrentTransform().getMatrix()*m_mouseGlobalTX;
  MV=  M*m_cam->getViewMatrix();
  MVP= M*m_cam->getVPMatrix();
  normalMatrix=MV;
  normalMatrix.inverse();
  shader->setShaderParamFromMat4("MVP",MVP);
}

//----------------------------------------------------------------------------------------------------------------------

void NGLDraw::loadMatricesToPhongShader()
{
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  (*shader)["Phong"]->use();
  ngl::Mat4 MV;
  ngl::Mat4 MVP;
  ngl::Mat3 normalMatrix;
  ngl::Mat4 M;
  M=m_bodyTransform*m_transformStack.getCurrentTransform().getMatrix()*m_mouseGlobalTX;
  MV=  M*m_cam->getViewMatrix();
  MVP= M*m_cam->getVPMatrix();
  normalMatrix=MV;
  normalMatrix.inverse();
  shader->setShaderParamFromMat4("MV",MV);
  shader->setShaderParamFromMat4("MVP",MVP);
  shader->setShaderParamFromMat3("normalMatrix",normalMatrix);
  shader->setShaderParamFromMat4("M",M);

}

//----------------------------------------------------------------------------------------------------------------------

void NGLDraw::mouseMoveEvent (const SDL_MouseMotionEvent &_event)
{
  if(m_rotate && _event.state &SDL_BUTTON_LMASK)
  {
    int diffx=_event.x-m_origX;
    int diffy=_event.y-m_origY;
    m_spinXFace += (float) 0.5f * diffy;
    m_spinYFace += (float) 0.5f * diffx;
    m_origX = _event.x;
    m_origY = _event.y;
    this->draw();

  }

  else if(m_translate && _event.state &SDL_BUTTON_RMASK)
  {
    int diffX = (int)(_event.x - m_origXPos);
    int diffZ = (int)(_event.y - m_origZPos);
    m_origXPos=_event.x;
    m_origZPos = _event.y;
    m_modelPos.m_x -= INCREMENT * diffX;
    m_modelPos.m_z -= INCREMENT * diffZ;
    this->draw();
  }
}


//----------------------------------------------------------------------------------------------------------------------
void NGLDraw::mousePressEvent (const SDL_MouseButtonEvent &_event)
{
  if(_event.button == SDL_BUTTON_LEFT)
  {
    m_origX = _event.x;
    m_origY = _event.y;
    m_rotate =true;
  }

  else if(_event.button == SDL_BUTTON_RIGHT)
  {
    m_origXPos = _event.x;
    m_origZPos = _event.y;
    m_translate=true;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLDraw::mouseReleaseEvent (const SDL_MouseButtonEvent &_event)
{
  if (_event.button == SDL_BUTTON_LEFT)
  {
    m_rotate=false;
  }

  if (_event.button == SDL_BUTTON_RIGHT)
  {
    m_translate=false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLDraw::wheelEvent(const SDL_MouseWheelEvent &_event)
{
  if(_event.y > 0)
  {
    m_modelPos.m_y+=ZOOM;
    this->draw();
  }
  else if(_event.y <0 )
  {
    m_modelPos.m_y-=ZOOM;
    this->draw();
  }

  if(_event.x > 0)
  {
    m_modelPos.m_x-=ZOOM;
    this->draw();
  }
  else if(_event.x <0 )
  {
    m_modelPos.m_x+=ZOOM;
    this->draw();
  }
}

//----------------------------------------------------------------------------------------------------------------------

void NGLDraw::rotateMazeXUP(float angleUpdate)
{
  unsigned int bodies=m_physics->getNumCollisionObjects();
  for(unsigned int i=1; i<bodies; ++i)
  {
    if(m_physics->getBodyNameAtIndex(i) == "maze")
    {
      btQuaternion total;
      total = m_physics->getRotation(i);
      btQuaternion local;
      local.setW(cos(angleUpdate/2));
      local.setX(sin(angleUpdate/2));
      total = local * total;
      m_physics->setRot(i, total);
    }
    if(m_physics->getBodyNameAtIndex(i) == "cube")
    {
      btQuaternion total;
      total = m_physics->getRotation(i);
      btQuaternion local;
      local.setW(cos(angleUpdate/2));
      local.setX(sin(angleUpdate/2));
      total = local * total;
      m_physics->setRotAboutOrigin(i, total);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------

void NGLDraw::rotateMazeZRIGHT(float angleUpdate)
{
  unsigned int bodies=m_physics->getNumCollisionObjects();
  for(unsigned int i=1; i<bodies; ++i)
  {
    if(m_physics->getBodyNameAtIndex(i) =="maze")
    {
      btQuaternion total;
      total = m_physics->getRotation(i);
      btQuaternion local;
      local.setW(cos(angleUpdate/2));
      local.setZ(sin(angleUpdate/2));
      total = local * total;
      m_physics->setRot(i, total);
    }
    if(m_physics->getBodyNameAtIndex(i) =="cube")
    {
      btQuaternion total;
      total = m_physics->getRotation(i);
      btQuaternion local;
      local.setW(cos(angleUpdate/2));
      local.setZ(sin(angleUpdate/2));
      total = local * total;
      m_physics->setRotAboutOrigin(i, total);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------

void NGLDraw::rotateMazeXDOWN(float angleUpdate)
{
  unsigned int bodies=m_physics->getNumCollisionObjects();
  for(unsigned int i=1; i<bodies; ++i)
  {
    if(m_physics->getBodyNameAtIndex(i) == "maze")
    {
      btQuaternion total;
      total = m_physics->getRotation(i);
      btQuaternion local;
      local.setW(cos(-angleUpdate/2));
      local.setX(sin(-angleUpdate/2));
      total = local * total;
      m_physics->setRot(i, total);
    }
    if(m_physics->getBodyNameAtIndex(i) == "cube")
    {
      btQuaternion total;
      total = m_physics->getRotation(i);
      btQuaternion local;
      local.setW(cos(-angleUpdate/2));
      local.setX(sin(-angleUpdate/2));
      total = local * total;
      m_physics->setRotAboutOrigin(i, total);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------

void NGLDraw::rotateMazeZLEFT(float angleUpdate)
{
  unsigned int bodies=m_physics->getNumCollisionObjects();
  for(unsigned int i=1; i<bodies; ++i)
  {
    if(m_physics->getBodyNameAtIndex(i)=="maze")
    {
      btQuaternion total;
      total = m_physics->getRotation(i);
      btQuaternion local;
      local.setW(cos(-angleUpdate/2));
      local.setZ(sin(-angleUpdate/2));
      total = local * total;
      m_physics->setRot(i, total);
    }

    if(m_physics->getBodyNameAtIndex(i)=="cube")
    {
      btQuaternion total;
      total = m_physics->getRotation(i);
      btQuaternion local;
      local.setW(cos(-angleUpdate/2));
      local.setZ(sin(-angleUpdate/2));
      total = local * total;
      m_physics->setRotAboutOrigin(i, total);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------

bool NGLDraw::lose(float _friction)
{
  unsigned int bodies=m_physics->getNumCollisionObjects();
  for(unsigned int i=1; i<bodies; ++i)
  {
    if(m_physics->getBodyNameAtIndex(i)=="ball")
    {
      ngl::Vec3 pos = m_physics->getPosition(i);
      if(pos.m_y < 3)
      {
        m_physics->reset();
        m_physics->addMaze("maze",ngl::Vec3(0,20,0), _friction);
        m_physics->addSphere("ball", ngl::Vec3(-15,25,-15), _friction);
        m_physics->addCube("cube",ngl::Vec3(0,17,0));
        setGameState(2);
        m_lost=1;
        break;
      }
      else
      {
        m_lost =0;
        break;
      }
    }
  }

  return m_lost;
}

//----------------------------------------------------------------------------------------------------------------------

bool NGLDraw::win(float _friction)
{
  unsigned int bodies=m_physics->getNumCollisionObjects();
  for(unsigned int i=1; i<bodies; ++i)
  {
    for(unsigned int j=1; j<bodies; ++j)
    {
      if(m_physics->getBodyNameAtIndex(i)=="ball" && m_physics->getBodyNameAtIndex(j)=="cube")
      {
        if(m_physics->contactTest(i,j)==true)
        {
          m_physics->reset();
          m_physics->addMaze("maze",ngl::Vec3(0,20,0), _friction);
          m_physics->addSphere("ball", ngl::Vec3(-15,25,-15), _friction);
          m_physics->addCube("cube",ngl::Vec3(0,17,0));
          setGameState(3);
          m_win=1;
          break;
        }
        else if(m_physics->contactTest(i,j)==false)
        {
          m_win=0;
          break;
        }
      }
    }
  }
  return m_win;
}

//----------------------------------------------------------------------------------------------------------------------

void NGLDraw::createball(float _friction)
{
  m_physics->addSphere("ball", ngl::Vec3(-15,25,-15), _friction);
}

void NGLDraw::text(std::string _text)
{
  m_text->setColour(0,0,0);
  m_text->renderText(10, 50, _text);
}

//----------------------------------------------------------------------------------------------------------------------

