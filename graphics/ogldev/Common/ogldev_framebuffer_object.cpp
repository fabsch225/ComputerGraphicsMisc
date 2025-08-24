/*
        Copyright 2011 Etay Meiri

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

#include <stdio.h>

#include "ogldev_types.h"
#include "ogldev_framebuffer_object.h"
#include "ogldev_util.h"
#include "ogldev_engine_common.h"

FramebufferObject::FramebufferObject()
{
    m_fbo = 0;
    m_depthBuffer = 0;
}

FramebufferObject::~FramebufferObject()
{
    if (m_fbo != 0) {
        glDeleteFramebuffers(1, &m_fbo);
    }

    if (m_depthBuffer != 0) {
        glDeleteTextures(1, &m_depthBuffer);
    }
}


bool FramebufferObject::Init(unsigned int Width, unsigned int Height, bool ForPCF)
{
    bool ret = false;

    if (IsGLVersionHigher(4, 5)) {
        ret = InitDSA(Width, Height, ForPCF);
    }
    else {
        ret = InitNonDSA(Width, Height, ForPCF);
    }

    return ret;
}


bool FramebufferObject::InitNonDSA(unsigned int Width, unsigned int Height, bool ForPCF)
{
    m_width = Width;
    m_height = Height;

    // Create the FBO
    glGenFramebuffers(1, &m_fbo);

    // Create the depth buffer
    glGenTextures(1, &m_depthBuffer);
    glBindTexture(GL_TEXTURE_2D, m_depthBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, Width, Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    GLint FilterType = ForPCF ? GL_LINEAR : GL_NEAREST;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, FilterType);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, FilterType);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthBuffer, 0);

    // Disable read/writes to the color buffer
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        printf("FB error, status: 0x%x\n", Status);
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}


bool FramebufferObject::InitDSA(unsigned int Width, unsigned int Height, bool ForPCF)
{
    m_width = Width;
    m_height = Height;

    // Create the FBO
    glCreateFramebuffers(1, &m_fbo);

    // Create the depth buffer
    glCreateTextures(GL_TEXTURE_2D, 1, &m_depthBuffer);

    int Levels = 1;
    glTextureStorage2D(m_depthBuffer, Levels, GL_DEPTH_COMPONENT32, Width, Height);

    GLint FilterType = ForPCF ? GL_LINEAR : GL_NEAREST;

    glTextureParameteri(m_depthBuffer, GL_TEXTURE_MIN_FILTER, FilterType);
    glTextureParameteri(m_depthBuffer, GL_TEXTURE_MAG_FILTER, FilterType);

    glTextureParameteri(m_depthBuffer, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTextureParameteri(m_depthBuffer, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTextureParameterfv(m_depthBuffer, GL_TEXTURE_BORDER_COLOR, borderColor);

    glTextureParameteri(m_depthBuffer, GL_TEXTURE_BASE_LEVEL, 0);

    glNamedFramebufferTexture(m_fbo, GL_DEPTH_ATTACHMENT, m_depthBuffer, 0);

    // Disable read/writes to the color buffer
    glNamedFramebufferReadBuffer(m_fbo, GL_NONE);
    glNamedFramebufferDrawBuffer(m_fbo, GL_NONE);

    GLenum Status = glCheckNamedFramebufferStatus(m_fbo, GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        printf("FB error, status: 0x%x\n", Status);
        return false;
    }

    return true;
}


void FramebufferObject::BindForWriting()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_width, m_height);  // set the width/height of the shadow map!
}


void FramebufferObject::BindForReading(GLenum TextureUnit)
{
    if (IsGLVersionHigher(4, 5)) {
        BindForReadingDSA(TextureUnit);
    }
    else {
        BindForReadingNonDSA(TextureUnit);
    }
}


void FramebufferObject::BindForReadingNonDSA(GLenum TextureUnit)
{
    glActiveTexture(TextureUnit);
    glBindTexture(GL_TEXTURE_2D, m_depthBuffer);
}


void FramebufferObject::BindForReadingDSA(GLenum TextureUnit)
{
    glBindTextureUnit(TextureUnit - GL_TEXTURE0, m_depthBuffer);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////


CascadedShadowMapFBO::CascadedShadowMapFBO()
{
    m_fbo = 0;
    ZERO_MEM(m_shadowMap);
}

CascadedShadowMapFBO::~CascadedShadowMapFBO()
{
    if (m_fbo != 0) {
        glDeleteFramebuffers(1, &m_fbo);
    }

    glDeleteTextures(ARRAY_SIZE_IN_ELEMENTS(m_shadowMap), m_shadowMap);
}

bool CascadedShadowMapFBO::Init(unsigned int WindowWidth, unsigned int WindowHeight)
{
    // Create the FBO
    glGenFramebuffers(1, &m_fbo);

    // Create the depth buffer
    glGenTextures(ARRAY_SIZE_IN_ELEMENTS(m_shadowMap), m_shadowMap);

    for (uint i = 0 ; i < ARRAY_SIZE_IN_ELEMENTS(m_shadowMap) ; i++) {
        glBindTexture(GL_TEXTURE_2D, m_shadowMap[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowMap[0], 0);

    // Disable writes to the color buffer
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        printf("FB error, status: 0x%x\n", Status);
        return false;
    }

    return true;
}


void CascadedShadowMapFBO::BindForWriting(uint CascadeIndex)
{
    assert(CascadeIndex < ARRAY_SIZE_IN_ELEMENTS(m_shadowMap));
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowMap[CascadeIndex], 0);
}


void CascadedShadowMapFBO::BindForReading()
{
    glActiveTexture(CASCACDE_SHADOW_TEXTURE_UNIT0);
    glBindTexture(GL_TEXTURE_2D, m_shadowMap[0]);

    glActiveTexture(CASCACDE_SHADOW_TEXTURE_UNIT1);
    glBindTexture(GL_TEXTURE_2D, m_shadowMap[1]);

    glActiveTexture(CASCACDE_SHADOW_TEXTURE_UNIT2);
    glBindTexture(GL_TEXTURE_2D, m_shadowMap[2]);
}
