#ifndef NGLDRAW_H__
#define NGLDRAW_H__
//----------------------------------------------------------------------------------------------------------------------
/// @file NGLDraw.h
/// @brief NGLDraw constructors, destructors and methods
//----------------------------------------------------------------------------------------------------------------------
#include <QtGui/QWindow>
#include <ngl/Camera.h>
#include <ngl/TransformStack.h>
#include <ngl/Light.h>
#include <SDL.h>
#include <btBulletDynamicsCommon.h>
#include <ngl/Obj.h>
#include <Text.h>

//----------------------------------------------------------------------------------------------------------------------
/// @class NGLDraw "include/NGLDraw.h"
/// @brief NGLDraw constructors, destructors and methods
/// @author Faye Butler
/// Modified from - Jon Macey
/// @date 10/04/2014
//----------------------------------------------------------------------------------------------------------------------

class PhysicsWorld;

class NGLDraw
{
public :
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief ctor this will have a valid OpenGL context so we can create gl stuff
    //----------------------------------------------------------------------------------------------------------------------
    NGLDraw();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief dtor used to remove any NGL stuff created
    //----------------------------------------------------------------------------------------------------------------------
    ~NGLDraw();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief resize our screen and set the camera aspect ratio
    /// @param[in] _w the new width
    /// @param[in] _h the new height
    //----------------------------------------------------------------------------------------------------------------------
    void resize(int _w, int _h);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief draw the scene
    //----------------------------------------------------------------------------------------------------------------------
    void draw();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called every time a mouse is moved
    /// @param _event the SDL mouse event structure containing all mouse info
    //----------------------------------------------------------------------------------------------------------------------
    void mouseMoveEvent (const SDL_MouseMotionEvent &_event);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called every time a mouse button is presses
    /// @param _event the SDL mouse event structure containing all mouse info
    //----------------------------------------------------------------------------------------------------------------------
    void mousePressEvent (const SDL_MouseButtonEvent &_event);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called everytime the mouse button is released
    /// @param _event the SDL mouse event structure containing all mouse info
    //----------------------------------------------------------------------------------------------------------------------
    void mouseReleaseEvent (const SDL_MouseButtonEvent &_event);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called everytime the mouse wheel is moved
    /// @param _event the SDL mouse event structure containing all mouse info
    //----------------------------------------------------------------------------------------------------------------------
    void wheelEvent(const SDL_MouseWheelEvent &_event);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief method to create the physics world
    /// @param _gravityY is the strength of gravity in the Y direction read from the config file
    /// @param _friction is the strength of the friction read from the config file
    //----------------------------------------------------------------------------------------------------------------------
    void setPhysics(int _gravityY, float _friction);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief method to rotate down in the x direction
    /// @param angleUpdate is the amount to increase the angle of rotation by each time
    //----------------------------------------------------------------------------------------------------------------------
    void rotateMazeXDOWN(float angleUpdate);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief method to rotate right in the z direction
    /// @param angleUpdate is the amount to increase the angle of rotation by each time
    //----------------------------------------------------------------------------------------------------------------------
    void rotateMazeZRIGHT(float angleUpdate);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief method to rotate up in the x direction
    /// @param angleUpdate is the amount to increase the angle of rotation by each time
    //----------------------------------------------------------------------------------------------------------------------
    void rotateMazeXUP(float angleUpdate);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief method to rotate left in the z direction
    /// @param angleUpdate is the amount to increase the angle of rotation by each time
    //----------------------------------------------------------------------------------------------------------------------
    void rotateMazeZLEFT(float angleUpdate);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief method to test if player has lost (returns true or false)
    /// @param _friction is the strength of the friction read from the config file
    //----------------------------------------------------------------------------------------------------------------------
    bool lose(float _friction);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief method to test if player has won (returns true or false)
    /// @param _friction is the strength of the friction read from the config file
    //----------------------------------------------------------------------------------------------------------------------
    bool win(float _friction);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief method to render text
    /// @param _text string of text to render
    //----------------------------------------------------------------------------------------------------------------------
    void text(std::string _text);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief method to create more balls
    /// @param _friction is the strength of the friction read from the config file
    //----------------------------------------------------------------------------------------------------------------------
    void createball(float _friction);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief set the game state (start/game/win/lose)
    /// @param _state what state game should be set to
    //----------------------------------------------------------------------------------------------------------------------
    inline void setGameState(int const _state) {m_state = _state;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief get what state the game is in (start/game/win/lose)
    /// @param _state what state the game is in
    //----------------------------------------------------------------------------------------------------------------------
    inline int getGameState() const {return m_state;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief return physics world
    //----------------------------------------------------------------------------------------------------------------------
    inline PhysicsWorld *getPhysicsWorld(){return m_physics;}

protected :
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief method to load transform data to the shaders
    //----------------------------------------------------------------------------------------------------------------------
    void loadMatricesToPhongShader();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief method to load transform data to the shaders
    //----------------------------------------------------------------------------------------------------------------------
    void loadMatricesToTextureShader();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief used to store the x rotation mouse value
    //----------------------------------------------------------------------------------------------------------------------
    int m_spinXFace;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief used to store the y rotation mouse value
    //----------------------------------------------------------------------------------------------------------------------
    int m_spinYFace;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief flag to indicate if the mouse button is pressed when dragging
    //----------------------------------------------------------------------------------------------------------------------
    bool m_rotate;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief flag to indicate if the Right mouse button is pressed when dragging
    //----------------------------------------------------------------------------------------------------------------------
    bool m_translate;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the previous x mouse value
    //----------------------------------------------------------------------------------------------------------------------
    int m_origX;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the previous y mouse value
    //----------------------------------------------------------------------------------------------------------------------
    int m_origY;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the previous z mouse value
    //----------------------------------------------------------------------------------------------------------------------
    int m_origZ;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the previous x mouse value for Position changes
    //----------------------------------------------------------------------------------------------------------------------
    int m_origXPos;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the previous y mouse value for Position changes
    //----------------------------------------------------------------------------------------------------------------------
    int m_origYPos;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the previous z mouse value for Position changes
    //----------------------------------------------------------------------------------------------------------------------
    int m_origZPos;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief used to store the global mouse transforms
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Mat4 m_mouseGlobalTX;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Our Camera
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Camera *m_cam;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief transformation stack for the gl transformations etc
    //----------------------------------------------------------------------------------------------------------------------
    ngl::TransformStack m_transformStack;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the model position for mouse movement
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Vec3 m_modelPos;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a simple light use to illuminate the screen
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Light *m_light;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief physics world
    //----------------------------------------------------------------------------------------------------------------------
    PhysicsWorld *m_physics;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief trasnform of body
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Mat4 m_bodyTransform;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief sphere obj mesh
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Obj *m_sphereMesh;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief maze obj mesh
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Obj *m_mazeMesh;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief cube obj mesh
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Obj *m_cube;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief larger text for titles
    //----------------------------------------------------------------------------------------------------------------------
    Text *m_text;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief smaller text for body
    //----------------------------------------------------------------------------------------------------------------------
    Text *m_bodyText;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief original angle for rotation
    //----------------------------------------------------------------------------------------------------------------------
    int m_angleOrig;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief what state the game is in
    //----------------------------------------------------------------------------------------------------------------------
    int m_state;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief if player has lost
    //----------------------------------------------------------------------------------------------------------------------
    bool m_lost;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief if player has won
    //----------------------------------------------------------------------------------------------------------------------
    bool m_win;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief texture for the maze
    //----------------------------------------------------------------------------------------------------------------------
    unsigned int m_mazeTexture;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief vec3 to set gravity
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Vec3 m_gravity;

};


#endif

