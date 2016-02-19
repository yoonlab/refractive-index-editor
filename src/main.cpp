#define SDL_MAIN_HANDLED

#include "Common.h"
#include "SceneNode.h"
#include "Renderer.h"

void infoMsg(const char* msg)
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Info", msg, NULL);
}

void errorMsg(const char* title)
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, SDL_GetError(), NULL);
}

class MyGLApp
{
public:
    enum Status { STATUS_AIM, STATUS_POINT, STATUS_MOVE };

    SDL_Window* window;
    Renderer renderer;
    Scene *scene;
    PhysicsInterface *physInterface;
    Camera* camera;
    SDL_GLContext glContext;
    SDL_Event event;

    int windowWidth = 1024;
    int windowHeight = 768;

    double speed;
    double mouseSpeed;
    double deltaTime;
    int runLevel;
    double lastTime;
    Status currentStatus;

    std::vector<PosColorVertex> *points;

    MyGLApp()
    {
        speed = .008f;
        mouseSpeed = 0.002f;
        runLevel = 1;
        lastTime = SDL_GetTicks();
        deltaTime = 0.0;
        window = 0;
        camera = 0;
        currentStatus = STATUS_AIM;
        points = new std::vector<PosColorVertex>();

        if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
        {
            std::cerr << "Unable to initialize SDL: " << SDL_GetError() << std::endl;
            runLevel = 0;
        }
        else
        {
            SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
            SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
            SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
            SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

            SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

            //Uncomment to enable antiailiasing/multisampling
            //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 4);

            Uint32 flags = SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
            window = SDL_CreateWindow("", 10, 30, windowWidth, windowHeight, flags);
            if (window == NULL)
            {
                fprintf(stderr, "Unable to create window: %s\n", SDL_GetError());
                errorMsg("Unable to create window");
                runLevel = 0;
                return;
            }
            else
            {
                glContext = SDL_GL_CreateContext(window);
                if (glContext == NULL)
                {
                    errorMsg("Unable to create OpenGL context");
                    errorMsg(SDL_GetError());
                    runLevel = 0;
                    return;
                }
                /* This makes our buffer swap syncronized with the monitor's vertical refresh */
                SDL_GL_SetSwapInterval(0);

                //GLEW
                glewExperimental = GL_TRUE;
                GLenum err = glewInit();
                checkForGLError();
                if (GLEW_OK != err)
                {
                    // Problem: glewInit failed, something is seriously wrong.
                    const char* errorStr = (char*)glewGetErrorString(err);
                    errorMsg(errorStr);

                }

                //std::cout << "Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
                SDL_SetWindowTitle(window, (const char*)glGetString(GL_VERSION));

                int flags = IMG_INIT_JPG|IMG_INIT_PNG|IMG_INIT_TIF;
                int initted = IMG_Init(flags);
                if(initted&flags != flags)
                {
                    std::cerr << IMG_GetError() << std::endl;
                }
            }

            SDL_SetWindowGrab(window, SDL_TRUE);
            if(SDL_ShowCursor(SDL_DISABLE) < 0)
            {
                std::cerr << "Unable to hide the cursor" << std::endl;
            }

            /*
            if(SDL_SetRelativeMouseMode(SDL_TRUE) < 0) {
                errorMsg(SDL_GetError());
            } */

            checkForGLError();

            // Get largest anisotropic filtering level
            GLfloat fLargest;
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);

            checkForGLError();

            // Enable depth test
            glEnable(GL_DEPTH_TEST);
            checkForGLError();

            // Accept fragment if it closer to the camera than the former one
            glDepthFunc(GL_LESS);

            // Cull triangles which normal is not towards the camera
            // Enable only if faces all faces are drawn counter-clockwise
            //glEnable(GL_CULL_FACE);

            checkForGLError();
            glClearColor(0.8, 0.8, 0.8, 1.0);
            checkForGLError();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            checkForGLError();

            camera = new Camera();

            scene = new Scene();
            scene->addWavefront("portland.obj", glm::translate(glm::mat4(1.f), glm::vec3(0.0, 0.0, 0.0)));

            renderer.glInitFromScene(scene);

            physInterface = new PhysicsInterface();
            physInterface->addCollisionObjectsFromScene(scene);

            GLint viewport[4];
            glGetIntegerv(GL_VIEWPORT, viewport);

            double width = (double) viewport[2];
            double height = (double) viewport[3];

            glViewport(0, 0, viewport[2], viewport[3]);

            SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
            SDL_WarpMouseInWindow(window, (int) (width / 2.0), (int) (height / 2.0));
            SDL_EventState(SDL_MOUSEMOTION, SDL_ENABLE);
        }
    }

    ~MyGLApp()
    {
        delete camera;
        delete scene;
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void keyDown(SDL_Keycode& key)
    {
        switch(key)
        {
        case SDLK_ESCAPE:
            runLevel = 0;
            break;
        }
    }

    void keyUp(SDL_Keycode& key)
    {

    }

    void start()
    {
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);

        double width = (double) viewport[2];
        double height = (double) viewport[3];

        // Get mouse position
        double xpos, ypos;
        int x, y;

        /* Get mouse position */
        while (runLevel > 0)
        {
            SDL_PollEvent(&event);

            if(event.type == SDL_QUIT)
            {
                runLevel = 0;
                break;
            }
            else if(event.type == SDL_KEYDOWN)
            {
                keyDown(event.key.keysym.sym);
                if(runLevel < 1)
                {
                    break;
                }
            }
            else if(event.type == SDL_KEYUP)
            {
                keyUp(event.key.keysym.sym);
            }

            const Uint8 *keys = SDL_GetKeyboardState(NULL);

            Uint32 mouseButtonState = SDL_GetMouseState(&x, &y);
            xpos = (double)x;
            ypos = (double)y;

            double currentTime = SDL_GetTicks();

            switch (currentStatus)
            {
            case STATUS_AIM:
                if (keys[SDL_SCANCODE_W])
                {
                    camera->moveForward(deltaTime * speed);
                }

                if (keys[SDL_SCANCODE_S])
                {
                    camera->moveBackward(deltaTime * speed);
                }

                if (keys[SDL_SCANCODE_D])
                {
                    camera->moveRight(deltaTime * speed);
                }

                if (keys[SDL_SCANCODE_A])
                {
                    camera->moveLeft(deltaTime * speed);
                }

                if (keys[SDL_SCANCODE_E])
                {
                    camera->moveUpward(deltaTime * speed);
                }

                if (keys[SDL_SCANCODE_Q])
                {
                    camera->moveDownward(deltaTime * speed);
                }

                if (keys[SDL_SCANCODE_SPACE])
                {
                    currentStatus = STATUS_POINT;
                    SDL_ShowCursor(SDL_ENABLE);
                }

                /* Ignore mouse input less than 2 pixels from origin (smoothing) */
                if (abs(x - (int)floor(viewport[2] / 2.0)) < 2)
                    x = (int)floor(viewport[2] / 2.0);
                if (abs(y - (int)floor(viewport[3] / 2.0)) < 2)
                    y = (int)floor(viewport[3] / 2.0);

                xpos = (double)x;
                ypos = (double)y;

                SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
                SDL_WarpMouseInWindow(window, (int)(width / 2.0), (int)(height / 2.0));
                SDL_EventState(SDL_MOUSEMOTION, SDL_ENABLE);

                // Compute time difference between current and last frame
                deltaTime = currentTime - lastTime;
                lastTime = currentTime;

                camera->aim(
                    mouseSpeed * (floor(width / 2.0) - xpos),
                    mouseSpeed * (floor(height / 2.0) - ypos)
                    );

                break;
            case STATUS_POINT:
                if (mouseButtonState & SDL_BUTTON(SDL_BUTTON_LEFT))
                {
                    glm::mat4 invProjMat = glm::inverse(camera->projectionMatrix);
                    glm::mat4 invMVMat = glm::inverse(camera->modelViewMatrix);
                    glm::vec4 onScreenVector = glm::vec4((2.0 * xpos)/width - 1.0f, 1.0f - (2.0 * ypos) / height, -1.f, 1.f);
                    glm::vec4 rayView = invProjMat * onScreenVector;
                    rayView = glm::vec4(rayView.x, rayView.y, -1.f, 0.f);
                    glm::vec4 rayWorld = invMVMat * rayView;
                    glm::vec3 rayWorld3 = glm::normalize(glm::vec3(rayWorld.x, rayWorld.y, rayWorld.z));
                    btVector3 orig = btVector3(camera->position.x, camera->position.y, camera->position.z);
                    btVector3 dir = btVector3(rayWorld3.x, rayWorld3.y, rayWorld3.z);
                    btVector3 point = physInterface->rayPick(orig, dir);
                    if (orig.x() == point.x() && orig.y() == point.y() && orig.z() == point.z())
                    {
                        std::cout << "Missed." << std::endl;
                    }
                    else
                    {
                        std::cout << "Shot from [" << orig.x() << ", " << orig.y() << ", " << orig.z() << "], "
                            << "Hit [" << point.x() << ", " << point.y() << ", " << point.z() << "]" << std::endl;
                        glm::vec3 hitPoint = glm::vec3(point.x(), point.y(), point.z());
                        PosColorVertex psv;
                        psv.position[0] = hitPoint.x; psv.position[1] = hitPoint.y; psv.position[2] = hitPoint.z;
                        psv.color[0] = 1.0f; psv.color[1] = 1.0f; psv.color[2] = 0.0f;
                        if (points->size() > 0)
                        {
                            points->pop_back();
                        }
                        points->push_back(psv);
                        scene->setPointsToBeShown(points);
                    }
                }
                break;
            case STATUS_MOVE:

                break;
            }

            if (keys[SDL_SCANCODE_N])
            {
                btVector3 orig = btVector3(camera->position.x, camera->position.y, camera->position.z);
                btVector3 dir = btVector3(camera->direction.x, camera->direction.y, camera->direction.z);
                btVector3 point = physInterface->rayPick(orig, dir);
                if (orig.x() == point.x() && orig.y() == point.y() && orig.z() == point.z())
                {
                    std::cout << "Missed." << std::endl;
                }
                else
                {
                    std::cout << "Shot from [" << orig.x() << ", " << orig.y() << ", " << orig.z() << "], "
                        << "Hit [" << point.x() << ", " << point.y() << ", " << point.z() << "]" << std::endl;
                }
            }

            camera->update();

            // Render frame
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            renderer.render(camera);
            SDL_GL_SwapWindow(window);
        }
    }
};

int main(int argc, char** argv)
{
    MyGLApp app;
    app.start();
    return 0;
}
