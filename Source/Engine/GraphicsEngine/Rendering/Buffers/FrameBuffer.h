#pragma once
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.h>
#include <Tools/Utilities/LinearAlgebra/Vector4.hpp>
#include <array>

#define PAD Uint32 ADRIA_CONCAT(pad, __COUNTER__)

DECLSPEC_ALIGN(16) struct FrameBuffer
{
	Matrix view;
	Matrix projection;
	Matrix view_projection;
	Matrix inverse_view;
	Matrix inverse_projection;
	Matrix inverse_view_projection;
	Matrix prev_view;
	Matrix prev_projection;
	Matrix prev_view_projection;

	Vector3f camera_position;
	float pad1;

	Vector3f camera_forward; 
	float pad2; 

	Vector2f  camera_jitter;
	float  camera_near;
	float  camera_far;

	Vector4f ambient_color; 
	Vector4f sun_direction;
	Vector4f sun_color;
	Vector4f cascade_splits;

	Vector2ui  display_resolution;
	Vector2ui  render_resolution;

	Vector2f  mouse_normalized_coords;
	float  delta_time;
	float  total_time;

	// Should i be using cstint instead of normal?
	uint32_t render_mode; 
	int32_t  lights_idx;
	int32_t  light_count;
	int32_t  lights_matrices_idx;

	int32_t  env_map_idx;
	int32_t  meshes_idx;
	int32_t  materials_idx;
	int32_t  instances_idx;
};