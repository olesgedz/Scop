/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scop.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jblack-b <jblack-b@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/19 17:11:31 by jblack-b          #+#    #+#             */
/*   Updated: 2020/02/19 18:16:56 by jblack-b         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef SCOP_H
#define SCOP_H

// #include "imgui.h"
// #include "imgui_impl_glfw.h"
// #include "imgui_impl_opengl3.h"
// #include <stdio.h>
//  #if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
// #include <GL/gl3w.h>    // Initialize with gl3wInit()
// #elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
// #include <GL/glew.h>    // Initialize with glewInit()
// #elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
// #include <glad/glad.h>  // Initialize with gladLoadGL()
// #elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING)
// #define GLFW_INCLUDE_NONE         // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
// #include <glbinding/glbinding.h>  // Initialize with glbinding::initialize()
// #include <glbinding/gl/gl.h>
// using namespace gl;
// #else
// #include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
// #endif
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include <glm/gtx/string_cast.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
// About Desktop OpenGL function loaders:
//  Modern desktop OpenGL doesn't have a standard portable header file to load OpenGL function pointers.
//  Helper libraries are often used for this purpose! Here we are supporting a few common ones (gl3w, glew, glad).
//  You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.



#endif