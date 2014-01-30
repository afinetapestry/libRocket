/*
 * This source file is part of libRocket, the HTML/CSS Interface Middleware
 *
 * For the latest information, see http://www.librocket.com
 *
 * Copyright (c) 2008-2010 CodePoint Ltd, Shift Technology Ltd
 * Copyright (c) 2014 Jon Hatchett
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#ifndef OPENGL32RENDERINTERFACE_HPP
#define OPENGL32RENDERINTERFACE_HPP

#include <memory>

#include <Rocket/Core.h>
#include <Rocket/Core/FileInterface.h>
#include <Rocket/Core/RenderInterface.h>

#include "glbits.h"

namespace {
	using namespace std;
}

/**
 * @author Jon Hatchett
 * @author Kyle (https://stackoverflow.com/users/453056/kyle) (https://stackoverflow.com/questions/20187797/getting-textures-to-work-in-opengl-3-2)
 * @author Lloyd Weehuizen
 */
class OpenGL32RenderInterface : public Rocket::Core::RenderInterface {
	int m_width = 0;
	int m_height = 0;

public:
	OpenGL32RenderInterface();
	~OpenGL32RenderInterface();

	void SetViewport(int width, int height);

	void RenderGeometry(Rocket::Core::Vertex * vertices, int num_vertices, int * indices, int num_indices, Rocket::Core::TextureHandle texture, const Rocket::Core::Vector2f & translation);

	Rocket::Core::CompiledGeometryHandle CompileGeometry(Rocket::Core::Vertex * vertices, int num_vertices, int * indices, int num_indices, Rocket::Core::TextureHandle texture);

	void RenderCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry, const Rocket::Core::Vector2f & translation);
	void ReleaseCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry);

	void EnableScissorRegion(bool enable);
	void SetScissorRegion(int x, int y, int width, int height);

	bool LoadTexture(Rocket::Core::TextureHandle & texture_handle, Rocket::Core::Vector2i & texture_dimensions, const Rocket::Core::String & source);
	bool GenerateTexture(Rocket::Core::TextureHandle & texture_handle, const Rocket::Core::byte * source, const Rocket::Core::Vector2i & source_dimensions);
	void ReleaseTexture(Rocket::Core::TextureHandle texture_handle);
};

#endif
