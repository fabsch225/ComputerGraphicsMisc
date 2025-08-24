/*

        Copyright 2022 Etay Meiri

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

#include "ogldev_cubemap_texture.h"
#include "Int/core_scene.h"

class RenderingSystemGL;

class GLScene : public CoreScene
{
public:
    GLScene(RenderingSystemGL* pRenderingSystem) : CoreScene((CoreRenderingSystem*)pRenderingSystem) {}

    ~GLScene();

    void LoadSkybox(const char* pFilename);

    BaseCubmapTexture* GetSkyboxTex() const { return m_pSkyboxTex; }

private:
    BaseCubmapTexture* m_pSkyboxTex = NULL;
};
