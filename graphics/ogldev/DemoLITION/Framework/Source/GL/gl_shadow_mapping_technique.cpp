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

#include "GL/gl_shadow_mapping_technique.h"

ShadowMappingTechnique::ShadowMappingTechnique()
{

}


bool ShadowMappingTechnique::Init()
{
    if (!Technique::Init()) {
        return false;
    }

    if (!AddShader(GL_VERTEX_SHADER, "Framework/Shaders/GL/shadow_map.vs")) {
        return false;
    }

    if (!AddShader(GL_FRAGMENT_SHADER, "../Common/Shaders/empty.fs")) {
        return false;
    }

    if (!Finalize()) {
        return false;
    }

    GET_UNIFORM_AND_CHECK(m_WVPLoc, "gWVP");
    GET_UNIFORM_AND_CHECK(m_VPLoc, "gVP");
    GET_UNIFORM_AND_CHECK(m_isIndirectRenderLoc, "gIsIndirectRender");
    GET_UNIFORM_AND_CHECK(m_isPVPLoc, "gIsPVP");

    return true;
}


void ShadowMappingTechnique::SetWVP(const Matrix4f& WVP)
{
    glUniformMatrix4fv(m_WVPLoc, 1, GL_TRUE, (const GLfloat*)WVP.m);
}


void ShadowMappingTechnique::ControlIndirectRender(bool IsIndirectRender)
{
    glUniform1i(m_isIndirectRenderLoc, IsIndirectRender);
}


void ShadowMappingTechnique::ControlPVP(bool IsPVP)
{
    glUniform1i(m_isPVPLoc, IsPVP);
}


void ShadowMappingTechnique::SetVP(const Matrix4f& VP)
{
    glUniformMatrix4fv(m_VPLoc, 1, GL_TRUE, (const GLfloat*)VP.m);
}
