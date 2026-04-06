#pragma once

#include "PsdNamespace.h"
#include "PsdKey.h"
#include "list.h"
#include <codecvt>  // for codecvt_utf8_utf16
#include <locale>   // for wstring_convert
#include <string>

PSD_NAMESPACE_BEGIN

enum psd_status_t 
{
	PSD_STATUS_SUCCESS,
	PSD_STATUS_ERROR_NULL_BUFFER,
	PSD_STATUS_ERROR_NULL_DOCUMENT,
	PSD_STATUS_ERROR_NULL_HEADER,
	PSD_STATUS_ERROR_NULL_DESCRIPTOR,
	PSD_STATUS_ERROR_NULL_RESOURCE_SLICE,
	PSD_STATUS_ERROR_NULL_RESOURCE,
	PSD_STATUS_ERROR_NULL_TYPE_TOOL,
	PSD_STATUS_ERROR_NULL_BLEND_MODE,
	PSD_STATUS_ERROR_NULL_MASK,
	PSD_STATUS_ERROR_NULL_ANNOTATION,
	PSD_STATUS_ERROR_NULL_PATH_INFO,
	PSD_STATUS_ERROR_NULL_LAYER_CHANNEL,
	PSD_STATUS_ERROR_NULL_LAYER_INFO,
	PSD_STATUS_ERROR_NULL_LAYER,
	PSD_STATUS_ERROR_NULL_IMAGE,
	PSD_STATUS_ERROR_INVALID_HEADER_SIGNATURE = 100,
	PSD_STATUS_ERROR_INVALID_VERSION,
	PSD_STATUS_ERROR_INVALID_CHANNEL_RANGE,
	PSD_STATUS_ERROR_INVALID_SIZE_RANGE,
	PSD_STATUS_ERROR_INVALID_DEPTH_RANGE,
	PSD_STATUS_ERROR_INVALID_COLOR_MODE,
	PSD_STATUS_ERROR_INVALID_RESOURCE_SIGNATURE,
	PSD_STATUS_ERROR_INVALID_LAYER_INFO_SIGNATURE,
	PSD_STATUS_ERROR_UNSUPPORTED_IMAGE_COMPRESSION = 200,
	PSD_STATUS_ERROR_UNSUPPORTED_IMAGE_EXTRACTION,
	PSD_STATUS_ERROR_UNSUPPORTED_RESOURCE_SLICE_VERSION,
	PSD_STATUS_ERROR_UNSUPPORTED_RESOURCE_DESCRIPTOR_VERSION,
	PSD_STATUS_ERROR_SIZE_OVERFLOW = 300,
	PSD_STATUS_MAX_ENUM
};

enum psd_layer_info_key_t 
{
    PSD_LAYER_INFO_INVALID,
    PSD_LAYER_INFO_ANIMATION_EFFECTS           = util::Key<'a', 'n', 'F', 'X'>::VALUE,
    PSD_LAYER_INFO_ANNOTATIONS                 = util::Key<'A', 'n', 'n', 'o'>::VALUE,
    PSD_LAYER_INFO_BLACK_AND_WHITE             = util::Key<'b', 'l', 'w', 'h'>::VALUE,
    PSD_LAYER_INFO_BLEND_CLIPPING_ELEMENTS     = util::Key<'c', 'l', 'b', 'l'>::VALUE,
    PSD_LAYER_INFO_BLEND_INTERIOR_ELEMENTS     = util::Key<'i', 'n', 'f', 'x'>::VALUE,
    PSD_LAYER_INFO_BRIGHTNESS_AND_CONTRAST     = util::Key<'b', 'r', 'i', 't'>::VALUE,
    PSD_LAYER_INFO_CHANNEL_BLEND_RESTRICTION   = util::Key<'b', 'r', 's', 't'>::VALUE,
    PSD_LAYER_INFO_CHANNEL_MIXER               = util::Key<'m', 'i', 'x', 'r'>::VALUE,
    PSD_LAYER_INFO_COLOR_LOOKUP                = util::Key<'c', 'l', 'r', 'L'>::VALUE,
    PSD_LAYER_INFO_COTENT_GENERATOR_EXTRA_DATA = util::Key<'C', 'g', 'E', 'd'>::VALUE,
    PSD_LAYER_INFO_EFFECT_LAYER                = util::Key<'l', 'r', 'F', 'X'>::VALUE,
    PSD_LAYER_INFO_EXPOSURE                    = util::Key<'e', 'x', 'p', 'A'>::VALUE,
    PSD_LAYER_INFO_FILL_OPACITY                = util::Key<'i', 'O', 'p', 'a'>::VALUE,
    PSD_LAYER_INFO_FILTER_EFFECTS              = util::Key<'F', 'X', 'i', 'd'>::VALUE,
    PSD_LAYER_INFO_FILTER_EFFECTS2             = util::Key<'F', 'E', 'i', 'd'>::VALUE,
    PSD_LAYER_INFO_FILTER_MASK                 = util::Key<'F', 'M', 's', 'k'>::VALUE,
    PSD_LAYER_INFO_FOREIGN_EFFECT_ID           = util::Key<'f', 'f', 'x', 'i'>::VALUE,
    PSD_LAYER_INFO_GRADIENT                    = util::Key<'g', 'r', 'd', 'm'>::VALUE,
    PSD_LAYER_INFO_GRADIENT_FILL               = util::Key<'G', 'd', 'F', 'l'>::VALUE,
    PSD_LAYER_INFO_KNOCKOUT                    = util::Key<'k', 'n', 'k', 'o'>::VALUE,
    PSD_LAYER_INFO_LAYER_ID                    = util::Key<'l', 'y', 'i', 'd'>::VALUE,
    PSD_LAYER_INFO_LAYER_MASK_AS_GLOBAL_MASK   = util::Key<'l', 'm', 'g', 'm'>::VALUE,
    PSD_LAYER_INFO_LAYER_NAME_SOURCE           = util::Key<'l', 'n', 's', 'r'>::VALUE,
    PSD_LAYER_INFO_LAYER_SECTION_DIVIDER       = util::Key<'l', 's', 'c', 't'>::VALUE,
    PSD_LAYER_INFO_LAYER_VERSION               = util::Key<'l', 'y', 'v', 'r'>::VALUE,
    PSD_LAYER_INFO_LEGACY_TYPE_TOOL            = util::Key<'t', 'y', 'S', 'h'>::VALUE,
    PSD_LAYER_INFO_LOCKED                      = util::Key<'l', 's', 'p', 'f'>::VALUE,
    PSD_LAYER_INFO_LINKED_LAYER                = util::Key<'l', 'n', 'k', 'D'>::VALUE,
    PSD_LAYER_INFO_LINKED_LAYER2               = util::Key<'l', 'n', 'k', '1'>::VALUE,
    PSD_LAYER_INFO_LINKED_LAYER3               = util::Key<'l', 'n', 'k', '2'>::VALUE,
    PSD_LAYER_INFO_METADATA_SETTING            = util::Key<'s', 'h', 'm', 'd'>::VALUE,
    PSD_LAYER_INFO_NESTED_LAYER_DIVIDER        = util::Key<'l', 's', 'd', 'k'>::VALUE,
    PSD_LAYER_INFO_OBJECT_EFFECTS              = util::Key<'l', 'f', 'x', '2'>::VALUE,
    PSD_LAYER_INFO_PATTERN                     = util::Key<'P', 'a', 't', 't'>::VALUE,
    PSD_LAYER_INFO_PATTERN2                    = util::Key<'P', 'a', 't', '2'>::VALUE,
    PSD_LAYER_INFO_PATTERN3                    = util::Key<'P', 'a', 't', '3'>::VALUE,
    PSD_LAYER_INFO_PATTERN_DATA                = util::Key<'s', 'p', 'h', 'a'>::VALUE,
    PSD_LAYER_INFO_PATTERN_FILL                = util::Key<'P', 't', 'F', 'l'>::VALUE,
    PSD_LAYER_INFO_PHOTO_FILTER                = util::Key<'p', 'h', 'f', 'l'>::VALUE,
    PSD_LAYER_INFO_PLACED_LAYER                = util::Key<'S', 'o', 'L', 'd'>::VALUE,
    PSD_LAYER_INFO_PLACED_LAYER2               = util::Key<'p', 'l', 'L', 'd'>::VALUE,
    PSD_LAYER_INFO_REFERENCE_POINT             = util::Key<'f', 'x', 'r', 'p'>::VALUE,
    PSD_LAYER_INFO_SAVING_MERGED_TRANSPARENCY  = util::Key<'M', 't', 'r', 'n'>::VALUE,
    PSD_LAYER_INFO_SAVING_MERGED_TRANSPARENCY2 = util::Key<'M', 't', '1', '6'>::VALUE,
    PSD_LAYER_INFO_SAVING_MERGED_TRANSPARENCY3 = util::Key<'M', 't', '3', '2'>::VALUE,
    PSD_LAYER_INFO_SHEET_COLOR                 = util::Key<'l', 'c', 'l', 'r'>::VALUE,
    PSD_LAYER_INFO_SOLID_COLOR                 = util::Key<'S', 'o', 'C', 'o'>::VALUE,
    PSD_LAYER_INFO_TEXT_ENGINE_DATA            = util::Key<'T', 'x', 't', '2'>::VALUE,
    PSD_LAYER_INFO_TYPE_TOOL                   = util::Key<'T', 'y', 'S', 'h'>::VALUE,
    PSD_LAYER_INFO_TRANSPARENCY_SHAPE_LAYER    = util::Key<'t', 's', 'l', 'y'>::VALUE,
    PSD_LAYER_INFO_UNICODE_NAME                = util::Key<'l', 'u', 'n', 'i'>::VALUE,
    PSD_LAYER_INFO_UNICODE_PATH_NAME           = util::Key<'p', 't', 'h', 's'>::VALUE,
    PSD_LAYER_INFO_USER_MASK                   = util::Key<'L', 'M', 's', 'k'>::VALUE,
    PSD_LAYER_INFO_USING_ALIGNED_RENDERING     = util::Key<'s', 'n', '2', 'P'>::VALUE,
    PSD_LAYER_INFO_VECTOR_MASK                 = util::Key<'v', 'm', 's', 'k'>::VALUE,
    PSD_LAYER_INFO_VECTOR_MASK2                = util::Key<'v', 's', 'm', 's'>::VALUE,
    PSD_LAYER_INFO_VECTOR_MASK_AS_GLOBAL_MASK  = util::Key<'v', 'm', 'g', 'm'>::VALUE,
    PSD_LAYER_INFO_VECTOR_ORIGINATION          = util::Key<'v', 'o', 'g', 'k'>::VALUE,
    PSD_LAYER_INFO_VECTOR_STROKE               = util::Key<'v', 's', 't', 'k'>::VALUE,
    PSD_LAYER_INFO_VECTOR_STROKE_CONTENT       = util::Key<'v', 's', 'c', 'g'>::VALUE,
    PSD_LAYER_INFO_VIBRANCE                    = util::Key<'v', 'i', 'b', 'A'>::VALUE
};

enum psd_descriptor_key_t 
{
    PSD_DESCRIPTOR_INVALID,
    PSD_DESCRIPTOR_BOOL          = util::Key<'b', 'o', 'o', 'l'>::VALUE,
    PSD_DESCRIPTOR_CLASS         = util::Key<'t', 'y', 'p', 'e'>::VALUE,
    PSD_DESCRIPTOR_GLOBAL_CLASS  = util::Key<'G', 'l', 'b', 'C'>::VALUE,
    PSD_DESCRIPTOR_OBJECT        = util::Key<'O', 'b', 'j', 'c'>::VALUE,
    PSD_DESCRIPTOR_GLOBAL_OBJECT = util::Key<'G', 'l', 'b', 'O'>::VALUE,
    PSD_DESCRIPTOR_DOUBLE        = util::Key<'d', 'o', 'u', 'b'>::VALUE,
    PSD_DESCRIPTOR_ENUM          = util::Key<'e', 'n', 'u', 'm'>::VALUE,
    PSD_DESCRIPTOR_ALIAS         = util::Key<'a', 'l', 'i', 's'>::VALUE,
    PSD_DESCRIPTOR_FILEPATH      = util::Key<'P', 't', 'h', ' '>::VALUE,
    PSD_DESCRIPTOR_INT           = util::Key<'l', 'o', 'n', 'g'>::VALUE,
    PSD_DESCRIPTOR_LARGE_INT     = util::Key<'c', 'o', 'm', 'p'>::VALUE,
    PSD_DESCRIPTOR_LIST          = util::Key<'V', 'l', 'L', 's'>::VALUE,
    PSD_DESCRIPTOR_OBJECT_ARRAY  = util::Key<'O', 'b', 'A', 'r'>::VALUE,
    PSD_DESCRIPTOR_RAW           = util::Key<'t', 'd', 't', 'a'>::VALUE,
    PSD_DESCRIPTOR_REFERENCE     = util::Key<'o', 'b', 'j', ' '>::VALUE,
    PSD_DESCRIPTOR_UNICODE_TEXT  = util::Key<'T', 'E', 'X', 'T'>::VALUE,
    PSD_DESCRIPTOR_UNIT_DOUBLE   = util::Key<'U', 'n', 't', 'F'>::VALUE,
    PSD_DESCRIPTOR_UNIT_FLOAT    = util::Key<'U', 'n', 'F', 'l'>::VALUE,
    /* only in reference */									   
    PSD_DESCRIPTOR_PROPERTY      = util::Key<'p', 'r', 'o', 'p'>::VALUE,
    PSD_DESCRIPTOR_CLASS_REF     = util::Key<'C', 'l', 's', 's'>::VALUE,
    PSD_DESCRIPTOR_ENUM_REF      = util::Key<'E', 'n', 'm', 'r'>::VALUE,
    PSD_DESCRIPTOR_IDENTIFIER    = util::Key<'I', 'd', 'n', 't'>::VALUE,
    PSD_DESCRIPTOR_INDEX         = util::Key<'i', 'n', 'd', 'x'>::VALUE,
    PSD_DESCRIPTOR_NAME          = util::Key<'n', 'a', 'm', 'e'>::VALUE,
    PSD_DESCRIPTOR_OFFSET        = util::Key<'r', 'e', 'l', 'e'>::VALUE
};

enum psd_unit_key_t 
{
    PSD_UNIT_INVALID,
    PSD_UNIT_ANGLE       =  util::Key<'#', 'A', 'n', 'g'>::VALUE,
    PSD_UNIT_DENSITY     =  util::Key<'#', 'R', 's', 'l'>::VALUE,
    PSD_UNIT_DISTANCE    =  util::Key<'#', 'R', 'l', 't'>::VALUE,
    PSD_UNIT_NONE        =  util::Key<'#', 'N', 'n', 'e'>::VALUE,
    PSD_UNIT_PERCENT     =  util::Key<'#', 'P', 'r', 'c'>::VALUE,
    PSD_UNIT_PIXEL       =  util::Key<'#', 'P', 'x', 'l'>::VALUE,
    PSD_UNIT_MILLIMETERS =  util::Key<'#', 'M', 'l', 'm'>::VALUE,
    PSD_UNIT_POINTS      =  util::Key<'#', 'P', 'n', 't'>::VALUE
};

enum psd_blend_mode_key_t 
{
    PSD_BLEND_MODE_NULL,
    PSD_BLEND_MODE_NORMAL        =  util::Key<'n', 'o', 'r', 'm'>::VALUE,
    PSD_BLEND_MODE_DARKEN        =  util::Key<'d', 'a', 'r', 'k'>::VALUE,
    PSD_BLEND_MODE_LIGHTEN       =  util::Key<'l', 'i', 't', 'e'>::VALUE,
    PSD_BLEND_MODE_HUE           =  util::Key<'h', 'u', 'e', ' '>::VALUE,
    PSD_BLEND_MODE_SATURATION    =  util::Key<'s', 'a', 't', ' '>::VALUE,
    PSD_BLEND_MODE_COLOR         =  util::Key<'c', 'o', 'l', ' '>::VALUE,
    PSD_BLEND_MODE_LUMINOSITY    =  util::Key<'l', 'u', 'm', ' '>::VALUE,
    PSD_BLEND_MODE_MULTIPLY      =  util::Key<'m', 'u', 'l', ' '>::VALUE,
    PSD_BLEND_MODE_SCREEN        =  util::Key<'s', 'c', 'r', 'n'>::VALUE,
    PSD_BLEND_MODE_DISSOLVE      =  util::Key<'d', 'i', 's', 's'>::VALUE,
    PSD_BLEND_MODE_OVERLAY       =  util::Key<'o', 'v', 'e', 'r'>::VALUE,
    PSD_BLEND_MODE_HARD_LIGHT    =  util::Key<'h', 'L', 'i', 't'>::VALUE,
    PSD_BLEND_MODE_SOFT_LIGHT    =  util::Key<'s', 'L', 'i', 't'>::VALUE,
    PSD_BLEND_MODE_DIFFERENCE    =  util::Key<'d', 'i', 'f', 'f'>::VALUE,
    PSD_BLEND_MODE_EXCLUSION     =  util::Key<'s', 'M', 'u', 'd'>::VALUE,
    PSD_BLEND_MODE_COLOR_DODGE   =  util::Key<'d', 'i', 'v', ' '>::VALUE,
    PSD_BLEND_MODE_COLOR_BURN    =  util::Key<'i', 'd', 'i', 'v'>::VALUE,
    PSD_BLEND_MODE_LINEAR_BURN   =  util::Key<'l', 'b', 'r', 'n'>::VALUE,
    PSD_BLEND_MODE_LINEAR_DODGE  =  util::Key<'l', 'd', 'd', 'g'>::VALUE,
    PSD_BLEND_MODE_VIVID_LIGHT   =  util::Key<'v', 'L', 'i', 't'>::VALUE,
    PSD_BLEND_MODE_LINEAR_LIGHT  =  util::Key<'l', 'L', 'i', 't'>::VALUE,
    PSD_BLEND_MODE_PIN_LIGHT     =  util::Key<'p', 'L', 'i', 't'>::VALUE,
    PSD_BLEND_MODE_HARD_MIX      =  util::Key<'h', 'M', 'i', 'x'>::VALUE,
    PSD_BLEND_MODE_PASSTHROUGH   =  util::Key<'p', 'a', 's', 's'>::VALUE,
    PSD_BLEND_MODE_DARKER_COLOR  =  util::Key<'d', 'k', 'C', 'l'>::VALUE,
    PSD_BLEND_MODE_LIGHTER_COLOR =  util::Key<'l', 'g', 'C', 'l'>::VALUE,
    PSD_BLEND_MODE_SUBTRACT      =  util::Key<'f', 's', 'u', 'b'>::VALUE,
    PSD_BLEND_MODE_DIVIDE        =  util::Key<'f', 'd', 'i', 'v'>::VALUE
};

enum psd_color_mode_t 
{
	PSD_COLOR_MODE_BITMAP,
	PSD_COLOR_MODE_GRAY_SCALE,
	PSD_COLOR_MODE_INDEXED,
	PSD_COLOR_MODE_RGB,
	PSD_COLOR_MODE_CMYK,
	PSD_COLOR_MODE_HSL,
	PSD_COLOR_MODE_HSB,
	PSD_COLOR_MODE_MULTICHANNEL,
	PSD_COLOR_MODE_DUOTONE,
	PSD_COLOR_MODE_LAB,
	PSD_COLOR_MODE_GRAY16,
	PSD_COLOR_MODE_RGB48,
	PSD_COLOR_MODE_LAB48,
	PSD_COLOR_MODE_CMYK64,
	PSD_COLOR_MODE_DEEP_MULTICHANNEL,
	PSD_COLOR_MODE_DUOTONE16,
	PSD_COLOR_MODE_MAX_ENUM
};

typedef void (*psd_list_node_free_t)(void*, void*);
typedef uint8_t psd_uint8_t;
typedef int16_t psd_int16_t;
typedef uint16_t psd_uint16_t;
typedef int32_t psd_int32_t;
typedef uint32_t psd_uint32_t;
typedef int64_t psd_int64_t;
typedef uint64_t psd_uint64_t;
typedef float32_t psd_float32_t;
typedef float64_t psd_float64_t;
typedef size_t psd_rsize_t;

enum psd_bool_t 
{
	psd_false,
	psd_true,
	PSD_BOOL_SIZE_ALIGNMENT = 0x7fffffff
};

struct psd_descriptor_class_t 
{
	char* name;
	char* id;
};
struct psd_descriptor_enum_t 
{
	char* type;
	char* value;
};
struct psd_descriptor_path_t 
{
	char* signature;
	char* value;
};
struct psd_descriptor_enum_reference_t 
{
	psd_descriptor_class_t* _class;
	psd_descriptor_enum_t* _enum;
};

struct psd_descriptor_unit_t 
{
	psd_unit_key_t key;
	psd_float64_t value;
};

struct psd_descriptor_t
{
	psd_descriptor_class_t* _class;
	list_t* values;
};

struct psd_descriptor_value_t
{
	char* id;
	psd_descriptor_key_t key;
	union 
    {
		psd_bool_t bval;
		int ival;
		psd_uint64_t lval;
		psd_float64_t dval;
		psd_descriptor_enum_t* eval;
		psd_descriptor_class_t* cval;
		struct 
        {
			psd_uint64_t nitems;
			psd_descriptor_value_t** items;
		} a;
		int index;
		int offset;
		char* alias;
		psd_descriptor_path_t* path;
		struct
		{
			psd_uint64_t length;
			char* raw;
		}r;
		char* text;
		psd_descriptor_unit_t uval;
		psd_descriptor_t* descriptor;
	} u;
};

struct psd_size_t 
{
	psd_uint32_t rows;
	psd_uint32_t columns;
};

struct psd_header_t 
{
	psd_uint32_t signature;
	psd_uint32_t version;
	psd_uint32_t num_channels;
	psd_size_t size;
	psd_uint32_t depths;
	psd_color_mode_t color_mode;
	psd_uint8_t* color_mode_data;
};

struct psd_path_record_t 
{
	psd_uint16_t type;
	union 
    {
		int num_points;
		struct 
        {
			psd_float64_t predecent_vert;
			psd_float64_t predecent_horiz;
			psd_float64_t anchor_vert;
			psd_float64_t anchor_horiz;
			psd_float64_t leaving_vert;
			psd_float64_t leaving_horiz;
			psd_bool_t is_closed;
		} bezier;
		struct 
        {
			psd_float64_t top;
			psd_float64_t left;
			psd_float64_t bottom;
			psd_float64_t right;
			psd_float64_t resolution;
		} clipboard;
		int initial_fill;
	} u;
};

struct psd_layer_info_t 
{
	psd_layer_info_key_t key;
	psd_rsize_t length;
	const psd_uint8_t* data;
	struct 
    {
		psd_float64_t xx;
		psd_float64_t xy;
		psd_float64_t yx;
		psd_float64_t yy;
		psd_float64_t tx;
		psd_float64_t ty;
	} transform_matrix;
	union 
    {
		psd_bool_t blend_clipping_element_enabled;
		psd_bool_t blend_interior_element_enabled;
		struct 
        {
			int version;
			int monochrome;
			int values[10];
		} channel_mixer;
		psd_uint32_t fill_opacity_value;
		psd_descriptor_t* gradient_fill;
		psd_bool_t knockout_enabled;
		struct 
        {
			psd_bool_t is_folder;
			psd_bool_t is_hidden;
		} layer_group;
		psd_uint32_t layer_id;
		psd_uint32_t layer_name_source_id;
		struct 
        {
			psd_bool_t is_folder;
			psd_bool_t is_hidden;
			psd_blend_mode_key_t blend_mode;
			int sub_type;
		} layer_section_divider;
		psd_uint32_t layer_version;
		struct 
        {
			psd_bool_t transparency;
			psd_bool_t composite;
			psd_bool_t position;
		} locked;
		psd_descriptor_t* metadata_layer_composition;
		psd_descriptor_t* object_effect;
		/* pattern */
		psd_descriptor_t* placed_layer;
		struct 
        {
			psd_float64_t x;
			psd_float64_t y;
		} reference_point;
		struct 
        {
			psd_uint16_t _1;
			psd_uint16_t _2;
			psd_uint16_t _3;
			psd_uint16_t _4;
		} sheet_color;
		psd_descriptor_t* solid_color;
		psd_descriptor_t* type_tool;
		char* unicode_name;
		struct 
        {
			psd_uint32_t tags;
			psd_uint32_t nrecords;
			list_t* path_records;
		} vector_mask;
		psd_descriptor_t* vector_origination;
		psd_descriptor_t* vector_stroke;
		struct 
        {
			psd_uint32_t key;
			psd_descriptor_t* value;
		} vector_stroke_content;
		psd_bool_t using_aligned_rendering;
	} u;
};
PSD_NAMESPACE_END