#define SDL_MAIN_HANDLED

#include "Common.h"
#include "SceneNode.h"
#include "Renderer.h"
#include "Linear.h"
#include "TestModel.h"
#include "Lightpath.h"
#include "Cost.h"
#include "Optimizer.h"
#include <husl/husl.h>
#include <chrono>

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
    enum Status { STATUS_AIM, STATUS_SRCPTR, STATUS_DSTPTR, STATUS_VISUALIZE, STATUS_MAX };

    SDL_Window* window;
    Scene *scene;
    PhysicsInterface *physInterface;
    Camera camera;
    Camera fixCam;
    SDL_GLContext glContext;
    SDL_Event event;

    int windowWidth = 1600;
    int windowHeight = 900;

    Renderer renderer = Renderer(windowWidth, windowHeight);

    double speed;
    double acceleration;
    double mouseSpeed;
    double deltaTime;
    int runLevel;
    double lastTime;
    Status currentStatus;
    bool visRefraction;
    bool visCurve;
    bool visPoint;

    TestModel *testMedium;
    Linear *linearMedium;
    Linear *constMedium;
    std::vector<Lightpath *>testLightpaths;
    std::vector<Lightpath *>straightLightpaths;

    bool invalidatePointList;
    bool invalidateColoredPointList;
    std::vector<std::pair<glm::vec3 *, glm::vec3 *>> *pointPairs;
    std::vector<PosColorVertex> *coloredPoints;

    Cost *cost;

    MyGLApp()
    {
        speed = .008f;
        acceleration = 1.f;
        mouseSpeed = 0.002f;
        runLevel = 1;
        lastTime = SDL_GetTicks();
        deltaTime = 0.0;
        window = 0;
        currentStatus = STATUS_AIM;
        pointPairs = new std::vector<std::pair<glm::vec3 *, glm::vec3 *>>();
        coloredPoints = new std::vector<PosColorVertex>();
        invalidatePointList = false;
        invalidateColoredPointList = false;
        visRefraction = false;
        visCurve = true;
        visPoint = true;
        testMedium = new TestModel(3, 1, 0.1, 1, 1, 0.1, 1, 10);
        linearMedium = new Linear(glm::vec3(0, 1, 0), 0.01, 1.0);
        constMedium = new Linear(glm::vec3(0, 1, 0), 0, 1.0);
        cost = new Cost();

        
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
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 4);

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
            //glDepthFunc(GL_LESS);

            // Cull triangles which normal is not towards the camera
            // Enable only if faces all faces are drawn counter-clockwise
            //glEnable(GL_CULL_FACE);

            checkForGLError();

            camera = Camera();

            scene = new Scene();
            scene->addWavefront("portland2.obj", glm::translate(glm::mat4(1.f), glm::vec3(0.0, 0.0, 0.0)));

            GLint viewport[4];
            glGetIntegerv(GL_VIEWPORT, viewport);

            double width = (double)viewport[2];
            double height = (double)viewport[3];

            glViewport(0, 0, viewport[2], viewport[3]);

            renderer.glInitFromScene(scene);

            physInterface = new PhysicsInterface();
            physInterface->addCollisionObjectsFromScene(scene);

            SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
            SDL_WarpMouseInWindow(window, (int) (width / 2.0), (int) (height / 2.0));
            SDL_EventState(SDL_MOUSEMOTION, SDL_ENABLE);
        }
    }

    ~MyGLApp()
    {
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
        switch (key)
        {
        case SDLK_SPACE:
            switch (currentStatus)
            {
            case STATUS_AIM:
                currentStatus = STATUS_SRCPTR;
                fixCam = camera;
                SDL_ShowCursor(SDL_ENABLE);
                break;
            case STATUS_SRCPTR:
                if (!pointPairs->empty() && pointPairs->back().second == NULL)
                {
                    currentStatus = STATUS_DSTPTR;
                }
                break;
            case STATUS_DSTPTR:
                if (pointPairs->back().second != NULL)
                {
                    currentStatus = STATUS_SRCPTR;
                }
                break;
            default:
                break;
            }
            break;
        case SDLK_BACKSPACE:
            switch (currentStatus)
            {
            case STATUS_AIM:
                break;
            case STATUS_SRCPTR:
                if (!pointPairs->empty() && pointPairs->back().second == NULL)
                {
                    pointPairs->pop_back();
                    if (pointPairs->empty())
                    {
                        currentStatus = STATUS_AIM;
                        SDL_ShowCursor(SDL_DISABLE);
                    }
                    else
                    {
                        currentStatus = STATUS_DSTPTR;
                    }
                    invalidatePointList = true;
                    invalidateColoredPointList = true;
                }
                break;
            case STATUS_DSTPTR:
                if (pointPairs->back().second != NULL)
                {
                    pointPairs->back().second = NULL;
                    currentStatus = STATUS_SRCPTR;
                    invalidatePointList = true;
                    invalidateColoredPointList = true;
                }
                break;
            case STATUS_VISUALIZE:
                currentStatus = STATUS_AIM;
                pointPairs->clear();
                for (const auto &curve : scene->curves)
                {
                    delete curve;
                }
                scene->curves.clear();
                for (const auto &path : cost->paths)
                {
                    delete path;
                }
                testLightpaths.clear();
                straightLightpaths.clear();
                cost->paths.clear();
                invalidatePointList = true;
                invalidateColoredPointList = true;
                delete testMedium;
                testMedium = new TestModel(3, 1, 0.1, 1, 3, 0.1, 1, 10);
                break;
            }
            break;
        case SDLK_r:
            if (currentStatus == STATUS_VISUALIZE)
            {
                visRefraction = !visRefraction;
            }
            break;
        case SDLK_f:
            camera = fixCam;
            break;
        case SDLK_1:
            visCurve = !visCurve;
            break;
        case SDLK_2:
            visPoint = !visPoint;
            break;
        case SDLK_RETURN:
            if (currentStatus == STATUS_DSTPTR && pointPairs->back().second != NULL)
            {
                camera = fixCam;
                currentStatus = STATUS_VISUALIZE;
                SDL_ShowCursor(SDL_DISABLE);
                // Optimize!
                if (invalidatePointList)
                {
                    cost->paths.clear();
                    for (const auto &pair : *pointPairs)
                    {
                        std::cout << "Point locations: [(" << pair.first->x << ", " << pair.first->y << ", " << pair.first->z << "), (" <<
                            pair.second->x << ", " << pair.second->y << ", " << pair.second->z << ")]" << std::endl;
                        std::cout << "Camera location: [origin=(" << camera.position.x << ", " << camera.position.y << ", " << camera.position.z << "), target=(" <<
                            camera.direction.x + camera.position.x << ", " << camera.direction.y + camera.position.y << ", " << camera.direction.z + camera.position.z << "), up=(" <<
                            camera.up.x << ", " << camera.up.y << ", " << camera.up.z << ")]" << std::endl;
                        glm::dvec3 normalized = glm::normalize(*pair.first - camera.position);

                        Lightpath *testLightpath = new Lightpath(testMedium);
                        testLightpath->solve2(0, 0.3, 1000, (glm::dvec3)camera.position + normalized * (double)camera.near, normalized);
                        testLightpath->setTargetPoint(*pair.second);
                        testLightpaths.push_back(testLightpath);
                        cost->paths.push_back(testLightpath);

                        Lightpath *straightLightpath = new Lightpath(constMedium);
                        straightLightpath->solve2(0, 0.3, 1000, camera.position, normalized);
                        straightLightpaths.push_back(straightLightpath);

                        std::vector<PosColorVertex> *vertices = new std::vector<PosColorVertex>();
                        const std::string *name = new std::string("Curve");
                        straightLightpath->getCurveVertices(vertices, 0.0, 0.8, 1);
                        Curve *curve = new Curve(name, vertices);
                        delete name;
                        curve->glInit();
                        scene->curves.push_back(curve);
                    }
                    std::chrono::time_point<std::chrono::steady_clock> startTime = std::chrono::steady_clock::now();
                    Optimizer::optimize(cost);
                    std::chrono::time_point<std::chrono::steady_clock> endTime = std::chrono::steady_clock::now();
                    std::chrono::milliseconds diff = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
                    std::cout << diff.count() << "ms elapsed" << std::endl;
                    for (const auto &testLightpath : testLightpaths)
                    {
                        std::vector<PosColorVertex> *vertices = new std::vector<PosColorVertex>();
                        testLightpath->getCurveVertices(vertices, 1, 0.8, 0.);
                        const std::string *name = new std::string("Curve");
                        Curve *curve = new Curve(name, vertices);
                        delete name;
                        curve->glInit();
                        scene->curves.push_back(curve);
                        /*
                        std::cout << "testMedium.f(x) logging:" << std::endl;
                        for (int i = 0; i < 300; ++i)
                        {
                            std::cout << "alt = " << 0.1 * i << " unit, n = " << testMedium->f(glm::vec3(0, 0.1 * i, 0)) << std::endl;
                        }*/
                    }
                    invalidatePointList = false;
                }
            }
            break;
        }
    }

    bool getHitPoint(double xpos, double ypos, double width, double height, glm::vec3 *hitPointOut)
    {
        glm::mat4 invProjMat = glm::inverse(camera.projectionMatrix);
        glm::mat4 invMVMat = glm::inverse(camera.modelViewMatrix);
        glm::vec4 onScreenVector = glm::vec4((2.0 * xpos) / width - 1.0f, 1.0f - (2.0 * ypos) / height, -1.f, 1.f);
        glm::vec4 rayView = invProjMat * onScreenVector;
        rayView = glm::vec4(rayView.x, rayView.y, -1.f, 0.f);
        glm::vec4 rayWorld = invMVMat * rayView;
        glm::vec3 rayWorld3 = glm::normalize(glm::vec3(rayWorld.x, rayWorld.y, rayWorld.z));
        btVector3 orig = btVector3(camera.position.x, camera.position.y, camera.position.z);
        btVector3 dir = btVector3(rayWorld3.x, rayWorld3.y, rayWorld3.z);
        btVector3 point = physInterface->rayPick(orig, dir);
        if (orig.x() == point.x() && orig.y() == point.y() && orig.z() == point.z())
        {
            return false;
        }
        else
        {
            hitPointOut->x = point.x();
            hitPointOut->y = point.y();
            hitPointOut->z = point.z();
            return true;
        }
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
            invalidateColoredPointList = false;

            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                {
                    runLevel = 0;
                    break;
                }
                else if (event.type == SDL_KEYDOWN)
                {
                    keyDown(event.key.keysym.sym);
                    if (event.key.keysym.mod & KMOD_SHIFT)
                    {
                        acceleration = 10.f;
                    }
                    else if (event.key.keysym.mod & KMOD_CTRL)
                    {
                        acceleration = 0.1f;
                    }
                    if (runLevel < 1)
                    {
                        break;
                    }
                }
                else if (event.type == SDL_KEYUP)
                {
                    keyUp(event.key.keysym.sym);
                    if (event.key.keysym.mod & KMOD_SHIFT ||
                        event.key.keysym.mod & KMOD_CTRL)
                    {
                        acceleration = 1.f;
                    }
                }
            }

            const Uint8 *keys = SDL_GetKeyboardState(NULL);

            Uint32 mouseButtonState = SDL_GetMouseState(&x, &y);
            xpos = (double)x;
            ypos = (double)y;

            double currentTime = SDL_GetTicks();

            switch (currentStatus)
            {
            case STATUS_AIM:
            case STATUS_VISUALIZE:
                if (keys[SDL_SCANCODE_W])
                {
                    camera.moveForward(deltaTime * speed * acceleration);
                }

                if (keys[SDL_SCANCODE_S])
                {
                    camera.moveBackward(deltaTime * speed * acceleration);
                }

                if (keys[SDL_SCANCODE_D])
                {
                    camera.moveRight(deltaTime * speed * acceleration);
                }

                if (keys[SDL_SCANCODE_A])
                {
                    camera.moveLeft(deltaTime * speed * acceleration);
                }

                if (keys[SDL_SCANCODE_E])
                {
                    camera.moveUpward(deltaTime * speed * acceleration);
                }

                if (keys[SDL_SCANCODE_Q])
                {
                    camera.moveDownward(deltaTime * speed * acceleration);
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

                camera.aim(
                    mouseSpeed * (floor(width / 2.0) - xpos),
                    mouseSpeed * (floor(height / 2.0) - ypos)
                    );

                break;
            case STATUS_SRCPTR:
                if (mouseButtonState & SDL_BUTTON(SDL_BUTTON_LEFT))
                {
                    glm::vec3 *hitPoint = new glm::vec3();
                    bool hit = getHitPoint(xpos, ypos, width, height, hitPoint);
                    if (hit)
                    {
                        if (pointPairs->empty() || pointPairs->back().second != NULL)
                        {
                            pointPairs->push_back(std::pair<glm::vec3 *, glm::vec3 *>(NULL, NULL));
                        }
                        if (pointPairs->back().first != NULL) delete pointPairs->back().first;
                        pointPairs->back().first = new glm::vec3(*hitPoint);
                        invalidateColoredPointList = true;
                        invalidatePointList = true;
                    }
                    delete hitPoint;
                }
                break;
            case STATUS_DSTPTR:
                if (mouseButtonState & SDL_BUTTON(SDL_BUTTON_LEFT))
                {
                    glm::vec3 *hitPoint = new glm::vec3();
                    bool hit = getHitPoint(xpos, ypos, width, height, hitPoint);
                    if (hit)
                    {
                        if (pointPairs->back().second != NULL) delete pointPairs->back().second;
                        pointPairs->back().second = new glm::vec3(*hitPoint);
                        invalidateColoredPointList = true;
                        invalidatePointList = true;
                    }
                    delete hitPoint;
                }
                break;
            }

            if (invalidateColoredPointList)
            {
                coloredPoints->clear();
                int pairIdx = 0;
                int pairNum = pointPairs->size();
                for (auto const& pointPair : *pointPairs)
                {
                    PosColorVertex psv_1, psv_2;
                    if (pointPair.first != NULL)
                    {
                        psv_1.position[0] = pointPair.first->x;
                        psv_1.position[1] = pointPair.first->y;
                        psv_1.position[2] = pointPair.first->z;
                        //HUSLtoRGB(&psv_1.color[0], &psv_1.color[1], &psv_1.color[2], 360.0 * pairIdx / pairNum, 100, 70);
                        psv_1.color[0] = 0; psv_1.color[1] = 0.5; psv_1.color[2] = 1;
                        coloredPoints->push_back(psv_1);
                    }
                    if (pointPair.second != NULL)
                    {
                        psv_2.position[0] = pointPair.second->x;
                        psv_2.position[1] = pointPair.second->y;
                        psv_2.position[2] = pointPair.second->z;
                        //HUSLtoRGB(&psv_2.color[0], &psv_2.color[1], &psv_2.color[2], 360.0 * pairIdx / pairNum, 100, 50);
                        psv_2.color[0] = 1; psv_2.color[1] = 0.5; psv_2.color[2] = 0;
                        coloredPoints->push_back(psv_2);
                    }
                    pairIdx++;
                }
                scene->setPointsToBeShown(coloredPoints);
                invalidateColoredPointList = false;
            }

            /*
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
            */
            camera.update();

            // Render frame
            //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            renderer.render(camera, visRefraction, visCurve, visPoint,testMedium);
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
