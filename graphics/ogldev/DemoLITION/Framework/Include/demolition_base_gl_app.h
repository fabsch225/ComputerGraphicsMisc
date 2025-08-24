/*

        Copyright 2024 Etay Meiri

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "demolition_rendering_system.h"

#define GLFW_DLL
#include <GLFW/glfw3.h>


class BaseGLApp : public GameCallbacks
{
public:

    BaseGLApp(int WindowWidth, int WindowHeight, const char* pWindowName);

    ~BaseGLApp() {}

    virtual void OnFrame(long long DeltaTimeMillis);

    virtual void OnFrameEnd();

    virtual bool OnKeyboard(int key, int action);

    virtual bool OnMouseMove(int x, int y);

    virtual bool OnMouseButton(int Button, int Action, int Mode, int x, int y);

protected:

    virtual void OnFrameChild(long long DeltaTimeMillis) {};

    virtual void OnFrameGUI();

    RenderingSystem* m_pRenderingSystem = NULL;
    GLFWwindow* m_pWindow = NULL;

private:
    void InitGUI();
    
    bool m_isWireframe = false;
    bool m_leftMousePressed = false;
    bool m_midMousePressed = false;
    bool m_showGui = false;
    int m_isRefRefractEnabled = false;
    float m_reflectionFactor = 0.1f;
    float m_matRefRefractFactor = 0.5f;
    float m_indexOfRefraction = 1.0f;
    float m_fresnelPower = 1.0f;
};
