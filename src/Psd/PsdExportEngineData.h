#pragma once
#include <vector>
PSD_NAMESPACE_BEGIN

#pragma region Psd.ts

enum Justification
{
	left,
	right,
	center,
};

enum AntiAlias
{
	none,
	sharp,
	crisp,
	strong,
	smooth,
};

enum TextGridding
{
	//这里与上面重名了，改个名字
	none1,
};

enum Orientation
{
	horizontal,
	vertical,
};

enum ShapeType
{
	point,
	box,
};

struct Font
{
	wchar_t* name;
	uint64_t script;
	uint64_t type;
	uint64_t synthetic;
};

struct ParagraphStyle
{
	Justification Justification;
	float64_t FirstLineIndent;
	float64_t StartIndent;
	float64_t EndIndent;
	float64_t SpaceBefore;
	float64_t SpaceAfter;
	bool AutoHyphenate;
	uint64_t HyphenatedWordSize;
	uint64_t PreHyphen;
	uint64_t PostHyphen;
	uint64_t ConsecutiveHyphens;
	float64_t Zone;
	float64_t WordSpacing[3];
	float64_t LetterSpacing[3];
	float64_t GlyphSpacing[3];
	float64_t AutoLeading;
	uint64_t LeadingType;
	bool Hanging;
	bool Burasagari;
	uint64_t KinsokuOrder;
	bool EveryLineComposer;
};

struct TextStyle
{
	Font Font;
	float64_t FontSize;
	bool FauxBold;
	bool FauxItalic;
	bool AutoLeading;
	float64_t Leading;
	float64_t HorizontalScale;
	float64_t VerticalScale;
	uint64_t Tracking;
	bool AutoKerning;
	uint64_t Kerning;
	float64_t BaselineShift;
	uint64_t FontCaps; // 0 - none, 1 - small caps, 2 - all caps
	uint64_t FontBaseline; // 0 - normal, 1 - superscript, 2 - subscript
	bool Underline;
	bool Strikethrough;
	bool Ligatures;
	bool DLigatures;
	uint64_t BaselineDirection;
	float64_t Tsume;
	uint64_t StyleRunAlignment;
	uint64_t Language;
	bool NoBreak;
	float64_t FillColor[3];
	float64_t StrokeColor[3];
	bool FillFlag;
	bool StrokeFlag;
	bool FillFirst;
	uint64_t YUnderline;
	float64_t OutlineWidth;
	uint64_t CharacterDirection;
	bool HindiNumbers;
	uint64_t Kashida;
	uint64_t DiacriticPos;
};

struct ParagraphStyleRun
{
	uint64_t length;
	ParagraphStyle style;
};

struct TextStyleRun
{
	uint64_t length;
	TextStyle style;
};

struct TextGridInfo 
{
	bool isOn;
	bool show;
	uint64_t size;
	float64_t leading;
	float64_t color[3];
	float64_t leadingFillColor[3];
	bool alignLineHeightToGridFlags;
};

struct LayerTextData 
{
	wchar_t* text;
	wchar_t* font;
	float64_t transform[6];
	AntiAlias antiAlias;
	TextGridding gridding;
	Orientation orientation;
	uint64_t index;
	uint64_t top;
	uint64_t left;
	uint64_t bottom;
	uint64_t right;
	//warp没有了
	TextGridInfo gridInfo;
	bool useFractionalGlyphWidths;
	TextStyle style; // base style
	//这里有几个待定
	TextStyleRun styleRuns[2]; // spans of different style
	ParagraphStyle paragraphStyle; // base paragraph style
	//这里有几个待定
	ParagraphStyleRun paragraphStyleRuns[2]; // style for each line

	float64_t superscriptSize;
	float64_t superscriptPosition;
	float64_t subscriptSize;
	float64_t subscriptPosition;
	float64_t smallCapSize;

	ShapeType shapeType;
	float64_t pointBase[2];
	float64_t boxBounds[4];
};
#pragma endregion


#pragma region Text.ts
struct Adjustments
{
	float64_t Axis[3];
	float64_t XY[2];
};

struct TypeValues
{
	uint64_t Type;
	float64_t Values[4]; //Color
};

struct ParagraphProperties
{
	uint64_t Justification;
	float64_t FirstLineIndent;
	float64_t StartIndent;
	float64_t EndIndent;
	float64_t SpaceBefore;
	float64_t SpaceAfter;
	bool AutoHyphenate;
	uint64_t HyphenatedWordSize;
	uint64_t PreHyphen;
	uint64_t PostHyphen;
	uint64_t ConsecutiveHyphens;
	float64_t Zone;
	float64_t WordSpacing[3];
	float64_t LetterSpacing[3];
	float64_t GlyphSpacing[3];
	float64_t AutoLeading;
	uint64_t LeadingType;
	bool Hanging;
	bool Burasagari;
	uint64_t KinsokuOrder;
	bool EveryLineComposer;

	bool isDefault;
};

struct ParagraphSheet
{
	wchar_t* Name;
	uint64_t DefaultStyleSheet;
	ParagraphProperties Properties;

	bool isDefault;
};

struct StyleSheetData
{
	uint64_t Font;
	float64_t FontSize;
	bool FauxBold;
	bool FauxItalic;
	bool AutoLeading;
	float64_t Leading;
	float64_t HorizontalScale;
	float64_t VerticalScale;
	uint64_t Tracking;
	bool AutoKerning;
	uint64_t Kerning;
	float64_t BaselineShift;
	uint64_t FontCaps;
	uint64_t FontBaseline;
	bool Underline;
	bool Strikethrough;
	bool Ligatures;
	bool DLigatures;
	uint64_t BaselineDirection;
	float64_t Tsume;
	uint64_t StyleRunAlignment;
	uint64_t Language;
	bool NoBreak;
	TypeValues FillColor;
	TypeValues StrokeColor;
	bool FillFlag;
	bool StrokeFlag;
	bool FillFirst;
	uint64_t YUnderline;
	float64_t OutlineWidth;
	uint64_t CharacterDirection;
	bool HindiNumbers;
	uint64_t Kashida;
	uint64_t DiacriticPos;

	bool isDefault;
	bool isFew;
};

struct FontSet
{
	wchar_t* Name;
	uint64_t Script;
	uint64_t FontType;
	uint64_t Synthetic;
};

struct ResourceDict
{
	struct
	{
		wchar_t* Name;
		wchar_t* NoStart;
		wchar_t* NoEnd;
		wchar_t* Keep;
		wchar_t* Hanging;
	}KinsokuSet[2];
	struct
	{
		wchar_t* InternalName;
	}MojiKumiSet[4];
	uint64_t TheNormalStyleSheet;
	uint64_t TheNormalParagraphSheet;
	ParagraphSheet ParagraphSheetSet[1];
	struct
	{
		wchar_t* Name;
		StyleSheetData StyleSheetData;
	}StyleSheetSet[1];
	FontSet FontSet[2];
	float64_t SuperscriptSize;
	float64_t SuperscriptPosition;
	float64_t SubscriptSize;
	float64_t SubscriptPosition;
	float64_t SmallCapSize;
};

struct ParagraphRun
{
	ParagraphSheet ParagraphSheet;
	Adjustments Adjustments;
};

struct StyleRun
{
	struct
	{
		StyleSheetData StyleSheetData;
	}StyleSheet;

	bool isDefault;
};

struct PhotoshopNode
{
	uint64_t ShapeType;
	float64_t PointBase[2];
	//float64_t BoxBounds[2];
	struct
	{
		uint64_t ShapeType;
		float64_t TransformPoint0[2];
		float64_t TransformPoint1[2];
		float64_t TransformPoint2[2];
	}Base;
};

struct EngineData
{
	struct
	{
		struct
		{
			wchar_t* Text;
		}Editor;

		struct
		{
			ParagraphRun DefaultRunData;
			std::vector<ParagraphRun> RunArray;
			std::vector<uint64_t> RunLengthArray;
			uint64_t IsJoinable;
		} ParagraphRun;

		struct
		{
			StyleRun DefaultRunData;
			//这里可以支持一段字符有多种颜色，暂时先不支持
			std::vector<StyleRun> RunArray;
			std::vector<uint64_t> RunLengthArray;
			uint64_t IsJoinable;
		}StyleRun;

		struct
		{
			bool GridIsOn;
			bool ShowGrid;
			float64_t GridSize;
			float64_t GridLeading;
			TypeValues GridColor;
			TypeValues GridLeadingFillColor;
			bool AlignLineHeightToGridFlags;
		}GridInfo;

		uint64_t AntiAlias;
		bool UseFractionalGlyphWidths;

		struct
		{
			uint64_t Version;

			struct
			{
				uint64_t WritingDirection;

				struct
				{
					uint64_t ShapeType;
					uint64_t Procession;

					struct
					{
						uint64_t WritingDirection;
						struct
						{
							//这里数组长度其实是0，注意
						}Children[1];
					}Lines;

					struct
					{
						PhotoshopNode Photoshop;
					}Cookie;
				}Children[1];
			}Shapes;
		}Rendered;
	}EngineDict;

	//这里变量名本来是大写
	ResourceDict resourceDict;
	ResourceDict DocumentResources;
};
#pragma endregion



PSD_NAMESPACE_END