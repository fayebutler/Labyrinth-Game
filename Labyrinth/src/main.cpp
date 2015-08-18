//----------------------------------------------------------------------------------------------------------------------
/// @file main.cpp
/// @brief main function
//----------------------------------------------------------------------------------------------------------------------

#include <SDL.h>
#include <cstdlib>
#include <iostream>
#include "NGLDraw.h"
#include <ngl/NGLInit.h>
#include <stack>
#include <sstream>
#include <string>
#include <ngl/NGLInit.h>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <fstream>

//----------------------------------------------------------------------------------------------------------------------
/// @brief function to quit SDL with error message
/// @param[in] _msg the error message to send
void SDLErrorExit(const std::string &_msg);

//----------------------------------------------------------------------------------------------------------------------

/// @brief initialize SDL OpenGL context
SDL_GLContext createOpenGLContext( SDL_Window *window);

//----------------------------------------------------------------------------------------------------------------------

typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

//----------------------------------------------------------------------------------------------------------------------

int ParseScore(tokenizer::iterator &_firstWord)
{
  ++_firstWord;
  int outPut = boost::lexical_cast<int>(*_firstWord++);
  std::cout<<outPut<<std::endl;
  return outPut;
}

//----------------------------------------------------------------------------------------------------------------------

int ParseGravity(tokenizer::iterator &_firstWord)
{
  ++_firstWord;
  int outPut = boost::lexical_cast<int>(*_firstWord++);
  std::cout<<outPut<<std::endl;
  return outPut;
}

//----------------------------------------------------------------------------------------------------------------------

float ParseFriction(tokenizer::iterator &_firstWord)
{
  ++_firstWord;
  float outPut = boost::lexical_cast<float>(*_firstWord++);
  std::cout<<outPut<<std::endl;
  return outPut;
}

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
  //initialize variables
  float rotateUp =0.0;
  float rotateDown =0.0;
  float rotateLeft =0.0;
  float rotateRight=0.0;
  int score=0;
  int highScore=1000;
  int lastTime=0;
  int currentTime=0;
  int diffTime=0;
  int accumulator=0;
  int second=0;
  int pauseTime=0;
  int gravityY=0;
  float friction =0.0;

  //read in config file
  if (argc <=1)
  {
    std::cout <<"Usage FileRead [filename] \n";
    exit(EXIT_FAILURE);
  }
  std::fstream fileIn;

  fileIn.open(argv[1],std::ios::in);

  if (!fileIn.is_open())
  {
    std::cout <<"File : "<<argv[1]<<" Not found Exiting \n";
    exit(EXIT_FAILURE);
  }
  std::string lineBuffer;
  boost::char_separator<char> sep(" \t\r\n");

  while(!fileIn.eof())
  {
    getline(fileIn, lineBuffer, '\n');

    if(lineBuffer.size() >0)
    {
      tokenizer tokens(lineBuffer, sep);
      tokenizer::iterator firstWord = tokens.begin();

      if(*firstWord == "HighScore")
      {
        highScore = ParseScore(firstWord);
      }
      else if(*firstWord == "Gravity")
      {
        gravityY = ParseGravity(firstWord);
      }
      else if(*firstWord == "Friction")
      {
        friction = ParseFriction(firstWord);
      }
      else
      {
        std::cerr<<"unknown token"<<*firstWord<<std::endl;
      }
     }
  }
  fileIn.close();

  // Initialize SDL's Video subsystem
  if (SDL_Init(SDL_INIT_VIDEO) < 0 )
  {
    // Or die on error
    SDLErrorExit("Unable to initialize SDL");
  }

  // now get the size of the display and create a window we need to init the video
  SDL_Rect rect;
  SDL_GetDisplayBounds(0,&rect);
  // now create our window
  SDL_Window *window=SDL_CreateWindow("Labyrinth",
                                      SDL_WINDOWPOS_CENTERED,
                                      SDL_WINDOWPOS_CENTERED,
                                      rect.w/2,
                                      rect.h/2,
                                      SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
                                     );
  // check to see if that worked or exit
  if (!window)
  {
    SDLErrorExit("Unable to create window");
  }
  // Create our opengl context and attach it to our window
   SDL_GLContext glContext=createOpenGLContext(window);
   if(!glContext)
   {
     SDLErrorExit("Problem creating OpenGL context");
   }
   // make this our current GL context (we can have more than one window but in this case not)
   SDL_GL_MakeCurrent(window, glContext);
  /* This makes our buffer swap syncronized with the monitor's vertical refresh */
  SDL_GL_SetSwapInterval(1);
  // we need to initialise the NGL lib which will load all of the OpenGL functions, this must
  // be done once we have a valid GL context but before we call any GL commands. If we dont do
  // this everything will crash
  ngl::NGLInit::instance();
  // now clear the screen and swap whilst NGL inits (which may take time)
  glClear(GL_COLOR_BUFFER_BIT);
  SDL_GL_SwapWindow(window);

  bool quit=false;

  SDL_Event event;

  NGLDraw ngld;
  ngld.setPhysics(gravityY, friction);
  ngld.resize(rect.w,rect.h);
  ngld.setGameState(0);
  while(!quit)
  {
    while ( SDL_PollEvent(&event) )
    {
      switch (event.type)
      {
        // this is the window x being clicked.
        case SDL_QUIT : quit = true; break;
        // process the mouse data by passing it to ngl class
        case SDL_MOUSEMOTION : ngld.mouseMoveEvent(event.motion); break;
        case SDL_MOUSEBUTTONDOWN : ngld.mousePressEvent(event.button); break;
        case SDL_MOUSEBUTTONUP : ngld.mouseReleaseEvent(event.button); break;
        case SDL_MOUSEWHEEL : ngld.wheelEvent(event.wheel);
        // if the window is re-sized pass it to the ngl class to change gl viewport
        // note this is slow as the context is re-create by SDL each time
        case SDL_WINDOWEVENT :
          int w,h;
          // get the new window size
          SDL_GetWindowSize(window,&w,&h);
          ngld.resize(w,h);
        break;

        // now we look for a keydown event
        case SDL_KEYDOWN:
        {       
          switch( event.key.keysym.sym )
          {
            //set keys for game states
            case SDLK_ESCAPE :  quit = true; break;
            case SDLK_w : glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); break;
            case SDLK_s : glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); break;
            case SDLK_b :
            if(ngld.getGameState()==1)
            {
              ngld.createball(friction);
            }
            break;
            case SDLK_UP :
            if(ngld.getGameState()==1)
            {
              rotateUp= 0.003;
            }
            break;
            case SDLK_RIGHT :
            if(ngld.getGameState()==1)
            {
              rotateRight=0.003;
            }
            break;
            case SDLK_DOWN :
            if(ngld.getGameState()==1)
            {
              rotateDown= 0.003;
            }
            break;
            case SDLK_LEFT :
            if(ngld.getGameState()==1)
            {
              rotateLeft=0.003;
            }
            break;

            case SDLK_a :
            {
              if(ngld.getGameState()==0)
              {
                ngld.setGameState(1);
                break;
              }
              else if(ngld.getGameState()==1)
              {
                break;
              }
              else if(ngld.getGameState()==2)
              {
                ngld.setGameState(1);
                break;
              }
              else if(ngld.getGameState()==3)
              {
                ngld.setGameState(1);
                break;
              }
              break;
            }
            break;

            SDL_SetWindowFullscreen(window,SDL_FALSE);
            glViewport(0,0,rect.w,rect.h);
            break;

            case SDLK_g : SDL_SetWindowFullscreen(window,SDL_FALSE); break;
            default : break;

          }
          break;
        } // end of keydown

      //keyup event
      case SDL_KEYUP:
      {
        switch( event.key.keysym.sym )
        {

          case SDLK_UP : rotateUp=0.0; break;
          case SDLK_RIGHT : rotateRight=0.0; break;
          case SDLK_DOWN : rotateDown=0.0; break;
          case SDLK_LEFT : rotateLeft=0.0; break;
          break;
          default : break;
        }
        break;
      } // end of keyup

        default : break;
      } // end of event switch
    } // end of poll events

    //movement for maze rotation
    ngld.rotateMazeXUP(rotateUp);
    ngld.rotateMazeXDOWN(rotateDown);
    ngld.rotateMazeZLEFT(rotateLeft);
    ngld.rotateMazeZRIGHT(rotateRight);

    ngld.draw();

    //timer
    if(ngld.getGameState()==1)
    {
      if(ngld.lose(friction) ==1)
      {
        score = score ;
      }
      if(ngld.win(friction) ==1)
      {
        score = score ;
        if(score < highScore)
        {
          highScore = score;
        }
      }
      currentTime = SDL_GetTicks();
      diffTime = currentTime - lastTime;

      accumulator += diffTime;
      if(accumulator > 1)
      {
        accumulator -= 1;
        second = currentTime;
      }

      lastTime = currentTime;

      score = (second - pauseTime - score)/1000;

      std::stringstream ss1;
      ss1<<score;
      std::string s1= "Time = " + ss1.str();
      ngld.text(s1);
      lastTime = currentTime;

    }

    if(ngld.getGameState()==0 || ngld.getGameState()==2 || ngld.getGameState()==3)
    {
      pauseTime = SDL_GetTicks();

    }
    // swap the buffers
    SDL_GL_SwapWindow(window);

  }

  //write back into config file highscore etc
  if (argc <=1)
  {
    std::cerr <<"Usage FileWrite [filename] "<<std::endl;
    exit(EXIT_FAILURE);
  }
  std::ofstream fileOut;
  fileOut.open(argv[1],std::ios::out);
  // check to see if we can open the file
  if (!fileOut.is_open())
  {
    std::cerr <<"Could not open File : "<<argv[1]<<" for writing \n";
    exit(EXIT_FAILURE);
  }

  fileOut<<"HighScore "<<highScore<<std::endl;
  fileOut<<"Gravity "<<gravityY<<std::endl;
  fileOut<<"Friction "<<friction<<std::endl;

  fileOut.close();

 SDL_Quit();
}

//----------------------------------------------------------------------------------------------------------------------

SDL_GLContext createOpenGLContext(SDL_Window *window)
{
  // Request an opengl 3.2 context first we setup our attributes, if you need any
  // more just add them here before the call to create the context
  // SDL doesn't have the ability to choose which profile at this time of writing,
  // but it should default to the core profile
  // for some reason we need this for mac but linux crashes on the latest nvidia drivers
  // under centos
  #ifdef DARWIN
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
  #endif
  // set multi sampling else we get really bad graphics that alias
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,4);
  // Turn on double buffering with a 24bit Z buffer.
  // You may need to change this to 16 or 32 for your system
  // on mac up to 32 will work but under linux centos build only 16
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  // enable double buffering (should be on by default)
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  //
  return SDL_GL_CreateContext(window);

}

//----------------------------------------------------------------------------------------------------------------------

void SDLErrorExit(const std::string &_msg)
{
  std::cerr<<_msg<<"\n";
  std::cerr<<SDL_GetError()<<"\n";
  SDL_Quit();
  exit(EXIT_FAILURE);
}
